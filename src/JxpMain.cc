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
    std::cout << "Usage: jxp --xpath=\"xpath\"" << std::endl;
    std::cout << "Evaluates a xpath expression against a JSON object." << std::endl;
    std::cout << "JSON data is read from stdin and result is printed on stdout." << std::endl; 
}

}

int
main (int argc, char* argv[])
{
    std::string xpath;
    std::string json;
    int c;
    while (true) {
        static struct option long_options[] = {
            {"help",    no_argument,       0, 'h'},
            {"version", no_argument,       0, 'v'},
            {"json",    optional_argument, 0, 'j'},
            {"xpath",   required_argument, 0, 'x'},
            {0, 0, 0, 0}
        };
      
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hvx:", long_options, &option_index);

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
            return 0;
        case 'v':
            std::cout << Jstr::getVersion() << std::endl;
            return 0;
        case 'j':
            json = optarg;
            break;
        case 'x':
            xpath = optarg;
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
    if (xpath.empty()) {
        printHelp();
        return -1;
    }
    try {
        nlohmann::json d;
        if (json.empty()) {
            std::cout << "jxp: waiting for data on stdin." << std::endl;
            d = nlohmann::json::parse(std::cin);
        } else {
            std::ifstream ifs(json);
            if (!ifs.good()) {
                return -1;
            }
            d = nlohmann::json::parse(ifs);
        }
        Jstr::Xpath::Value value = Jstr::Xpath::eval(xpath, d);
        std::cout << value << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "jxp, exception: " << e.what() << std::endl;
    }
    return 0;
}
