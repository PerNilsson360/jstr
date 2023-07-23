# jstr - Json Schematron

An implementation of XPath 1.0 and a subset of Schematron for JSON.
It contains a library libjstr and two binaries **jxp** and **jstr**.

## Installation

./configure\
./make\
./make check\
./make install

The following will remove installed files: \
./make uninstall

## Using the library.
- Include Jstr.hh
- Link with libjstr.a

## Overview
XPath [1] is a domain specific language that is designed for XML. It has been
extensivly used as an emebeded langague. The following are some examples.

- XSLT [2], transformation of XML to XML.
- YANG [3], semantic validation data (enforcing constraints between entities).
- Netconf [4], query langauge for data.
- Schematron [5], semantic validation.

With this software it is possible to use XPath as a query language for JSON 
data. It is also possible to enforce semantic constraints on JSON data using
schematron. Structural and "simple" data constraints should be enforced by
validation using JSON schema [6].

Schematron [5] is a XML language but this library implement it using JSON.
Schematron is a realtive simple language and relies on XPath to query and
compare data.

XPath has some features that are XML specic these are skipped in this 
implementation. The following are some examples:

- Attributes.
- Namespaces.
- Processing instructions.

Also since there are diferences between the internal data trees used by JSON and
XML there might be subtle semantic differencis between this implementation of
XPath 1.0 and a XML implementation. Having said that the intention is to follow 
[1] as much as possible.

Also note that XPath 1.0 is a small language compared to XPath 3.1 this has some
benefits and ofcourse also som advantages.

## XPath 1.0
The following is a tutorial in XPath and it also shows how to use the tool jxp.

### Basic types
XPath 1.0 has the following types of values.

- Numbers, (floating point numbers).
- Booleans
- Strings
- NodeSets.

On numbers you can use many of the normal mathematical functions. The following
is an example.

`echo '{}' | jxp --xpath="(5.5 + 1) div 2"
3.25`

It shows how to use the commandline program **jxp**. Since it reads json from 
stdin we use echo and pipe a trivial JSON object to jxp. The result is shown on
the second line.

There are no boolean literals supported but there are functions that can be used
instead.

`echo '{}' | jxp --xpath="(false() and false()) or not(false())"
true`

This example also illustrates boolean conectives. Also not that the output from
jxp is compatible with JSON syntax.

XPath supports string literals enclosed in single and double quotes. The 
following example also shows some string functions.

`echo '{}' | jxp --xpath="string-length('foo') + string-length(substring-before(\"foo-bar\", 'bar'))"
7`

The primitive types has casting functions.

`echo '{}' | jxp --xpath="string(number(not(boolean(0))) + 1)"
"2"`

These functions also works for node sets. In XPath a node is defined as a node
in the DOM tree and these are XML concepts. In this implementaion  we can think
of a node as a json object or primitive value.



## References
[1] XPath 1.0 (https://www.w3.org/TR/1999/REC-xpath-19991116/#NT-Number)
[2] XSLT 3.0 (https://www.w3.org/TR/xslt-30/)
[3] YANG rfc7950 (https://datatracker.ietf.org/doc/html/rfc7950)
[4] Netconf rfc6241 (https://datatracker.ietf.org/doc/html/rfc6241)
[5] Schematron (https://www.iso.org/standard/74515.html)
[6] JSON schema (https://json-schema.org/)
