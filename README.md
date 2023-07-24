# jstr - JSON Schematron

An implementation of XPath 1.0 and a subset of Schematron for JSON. It contains
a library libjstr and two binaries **jxp** and **jstr**.

## Installation

The library depends on nlohmann json [1]. The following commands will install
binaries a library and header a header file.

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

- XSLT [3], transformation of XML to XML.
- YANG [4], semantic validation data (enforcing constraints between entities).
- Netconf [5], query langauge for data.
- Schematron [6], semantic validation.

With this software it is possible to use XPath as a query language for JSON 
data. It is also possible to enforce semantic constraints on JSON data using
schematron. Structural and "simple" data constraints should be enforced by
validation using JSON schema [7].

Schematron [6] is a XML language but this library implement it using JSON.
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
benefits and ofcourse also some drawbacks.

## XPath 1.0
The following is a tutorial of XPath 1.0 usage. It also shows how to use the 
tool jxp.

### Basic types
XPath 1.0 has the following types of values.

- Numbers, (floating point numbers including Nan and Infinity).
- Booleans.
- Strings.
- NodeSets.

With numbers you can use many of the normal mathematical functions. The 
following is an example.

`echo '{}' | jxp --xpath="(5.5 + 1) div 2" \
3.25`

It shows how to use the commandline program **jxp**. Since it reads JSON from 
stdin we use echo and pipe a trivial JSON object to jxp. The result is shown on
the second line.

There are no boolean literals supported but there are functions that can be used
instead.

`echo '{}' | jxp --xpath="(false() and false()) or not(false())" \
true`

This example also illustrates boolean conectives. Also note that the output from
jxp is compatible with JSON syntax.

XPath supports string literals enclosed in single and double quotes. The 
following example also shows some string functions.

`echo '{}' | jxp --xpath="string-length('foo') + string-length(substring-before(\"foo-bar\", 'bar'))" \
7`

The primitive types has casting functions.

`echo '{}' | jxp --xpath="string(number(not(boolean(0))) + 1)" \
"2"`

These functions also works for node sets. In XPath a node is defined as a node
in a XML DOM tree. In this implementaion we can think of a node as a JSON 
object or primitive value.

## Node sets
When a XPath expression is evaluated it needs to have a start node in these
examples it is always the root node. Evaluating an expression results in a 
value which can be of the above mentioned types the following is an example
showing a result with a node set with cardinality one.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/" \ 
[{"a":{"b":1,"c":true,"d":"foo"}}]`

Note that array notation is used and nodes are printed as json. 

## Abreviated path expressions
To retreive subsets of the data path expressions are used. They come in two
flavors. Abreviated and non abreviated. The follwing shows a simeple usage
using node names.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/b" \
[1]`

The requested b node is printed as 1 which is its JSON value. The implemetation
of the node also records the name of the node which can be seen in the 
following example.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="local-name(/a/b)" \
"b"`

The XPath function name is not supported since it should print XML namespaces. 
To get all children of a node the wildcard "*" is used.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/*" \
[1, true, "foo"]`

To get the number of nodes of the function "count" is used.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="count(/a/*)" \
3`

It is also possible to get all the descendents of a node. The following seraches
for all descendants.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="//*" \
[{"b":1,"c":true,"d":"foo"}, 1, true, "foo"]`

The following finds all "d" descendants.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="//d" \ 
["foo"]`

To navigate towards the root of the data tree ".." is used.

`echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/b/.." \
[{"b":1,"c":true,"d":"foo"}]`

## Comparing values

## Filters

## Non abreviated path expression 

# Schematron examples

## References
[1] nlohmann json (https://json.nlohmann.me/) \
[2] XPath 1.0 (https://www.w3.org/TR/1999/REC-xpath-19991116/#NT-Number) \
[3] XSLT 3.0 (https://www.w3.org/TR/xslt-30/) \
[4] YANG rfc7950 (https://datatracker.ietf.org/doc/html/rfc7950) \
[5] Netconf rfc6241 (https://datatracker.ietf.org/doc/html/rfc6241) \
[6] Schematron (https://www.iso.org/standard/74515.html) \
[7] JSON schema (https://json-schema.org/) \
