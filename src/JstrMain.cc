// MIT license
//
// Copyright 2023 Per Nilsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <Jstr.hh>

namespace {

void
printHelp() {
    std::cout << "Usage: jstr --schema=<schematron>" << std::endl;
    std::cout << "Validates json data against a schematron file." << std::endl;
    std::cout << "Json data is read from stdin and the result is printed on stdout." << std::endl;
}
    
}

int
main (int argc, char* argv[])
{
    std::string schema;
    int c;
    while (true) {
        static struct option long_options[] = {
            {"help",    no_argument,       0, 'h'},
            {"version", no_argument,       0, 'v'},
            {"schema",  required_argument, 0, 's'},
            {0, 0, 0, 0}
        };
      
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "h:v:s:", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) {
            break;
        }

        switch (c) {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0) {
                break;
            }
            printf ("option %s", long_options[option_index].name);
            if (optarg) {
                printf (" with arg %s", optarg);
                printf ("\n");
            }
            break;
        case 'h':
            printHelp();
            break;
        case 'v':
            std::cout << Jstr::getVersion() << std::endl;
            break;
        case 's':
            schema = optarg;
            break;
        case '?':
            /* getopt_long already printed an error message. */
            break;
        default:
            return -1;
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    if (schema.empty()) {
        printHelp();
        return -1;
    }
    std::ifstream ifs(schema);
    if (!ifs.good()) {
        std::cerr << "jstr: could not open schematron file: " << schema << std::endl;
        return -1;
    }
    nlohmann::json s = nlohmann::json::parse(ifs);
    nlohmann::json d = nlohmann::json::parse(std::cin);
    return Jstr::Schematron::eval(s, d, std::cout) ? 0 : -1;
}

