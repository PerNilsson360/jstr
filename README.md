# jstr - JSON Schematron

An implementation of XPath 1.0 and a subset of Schematron for JSON. It
contains a library libjstr and two binaries **jxp** and **jstr**.

## Installation

The library depends on nlohmann json [1]. The following commands will
install **jxp** and **jstr** binaries, the **libjstr.a** library and
the header file **Jstr.hh**.

./configure\
./make\
./make check\
./make install

The following will remove installed files:

./make uninstall

Note that install and unistall targets most likely needs to be
prefixed with "sudo".

## Using binaries
``` 
jxp --help
Usage: jxp --xpath="xpath"
Evaluates a xpath expression against a JSON object.
JSON data is read from stdin and result is printed on stdout. 
```

``` 
jstr --help
Usage: jstr --schema=schematron-file
Validates json data against a schematron file.
JSON data is read from stdin and the result is printed on stdout. 
```

## Using the library.
- Include Jstr.hh
- Link with libjstr.a

## Overview
XPath [1] is a domain specific language that is designed for XML. It
has been extensivly used as an emebeded langague. The following are
some examples.

- XSLT [3], XPath is used when doing transformation of XML to XML.
- YANG [4], XPath is used as a constraint language (semantic validation). 
- Netconf [5], XPath is used as a query langauge for data.
- Schematron [6], XPath is used for semantic validation.

With this software it is possible to use XPath as a query language for
JSON encoded data. It is also possible to enforce semantic constraints
on JSON data using schematron. Structural and "simple" datac onstraints
should be enforced by validation using JSON schema [7].

Schematron [6] is a XML language but this software implements a subset of it 
using JSON. Schematron is a realtive simple language and relies on XPath to 
query and compare data.

XPath has some features that are XML specific these are skipped in this 
implementation. The following are some examples:

- Attributes.
- Namespaces.
- Processing instructions.

Also since there are diferences between the internal data trees used by JSON and
XML there might be subtle semantic differencis between this implementation of
XPath 1.0 and a standard complient XML implementation. Having said that the 
intention is to follow [1] as much as possible.

Also note that XPath 1.0 is a small language compared to XPath 3.1 this has some
benefits and ofcourse also some drawbacks.

## XPath 1.0 tutorial
The following is a XPath 1.0 tutorial. It also shows how to use the tool jxp.

### Basic types
XPath 1.0 has values of the the following types.

- Numbers, (floating point numbers including NaN and Infinity).
- Booleans.
- Strings.
- Node sets.

With numbers you can use many of the normal mathematical functions. The 
following is an example.
``` 
echo '{}' | jxp --xpath="(5.5 + 1) div 2"
3.25 
```
It shows how to use the commandline program *jxp*. Since *jxp* reads JSON 
from stdin we use *echo* to pipe a trivial JSON object to *jxp*. The result of 
the command is shown on the second line.

XPath 1.0 does not support boolean literals supported there are functions that 
can be used instead.
``` 
echo '{}' | jxp --xpath="(false() and false()) or not(false())"
true 
```
This example also illustrates boolean connectives. Also note that the output 
from *jxp* is compatible with JSON syntax.

XPath supports string literals enclosed in single and double quotes. The 
following example also shows some string functions.
```
echo '{}' | jxp --xpath="string-length('foo') + string-length(substring-before(\"foo-bar\", 'bar'))"
7
```
The primitive types has casting functions.

``` 
echo '{}' | jxp --xpath="string(number(not(boolean(0))) + 1)"
"2" 
```
So boolean(0) evaluates to false and number(true) is 1. Finally the string 
representation of 1 + 1 which is 2 is returned as a result.

These functions also works for node sets. In XPath a node is defined as a node
in a XML DOM tree. In this implementaion we can think of a node as a JSON 
object or primitive value.

## Node sets
When a XPath expression is evaluated it needs to have access to a start node. 
This node is called the *context node*. In these examples it is always the 
root node. Evaluating an expression results in a value which can be of the 
above mentioned types. The following is an example showing a result with a 
node set with cardinality one.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/"
[{"a":{"b":1,"c":true,"d":"foo"}}] 
```
Note that array notation is used and nodes sets are printed as json and "/" 
denotes the root node.

## Abreviated path expressions
Path expressions are used to retreive subsets of data nodes. Path Expressions 
come in two flavors, abreviated and non abreviated. The follwing shows a simple
usage of using node names to select child node sets.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/b"
[1] 
```
The requested b node is printed as 1 which is its JSON value. The implemetation
of the node also records the name of the node which can be seen in the 
following example.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="local-name(/a/b)"
"b" 
```
The XPath function "name" is not supported since it should print XML namespaces
which JSON does not support. To get all children of a node the wildcard "*" is 
used.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/*"
[1, true, "foo"] 
```
Now we can explore what string function returns for a node set, from [2].

"The string function converts an object to a string as follows:
 - A node-set is converted to a string by returning the string-value of the 
   node in the node-set that is first in document order. If the node-set is 
   empty, an empty string is returned."

"The string-value of an element node is the concatenation of the string-values 
of all text node descendants of the element node in document order."
```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="string(/a/*)"
"1"
```
The first node in the above nodes set is the node with local-name "b" its value
is 1. This value is returned as a string.
```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="string(/a)"
"1truefoo"
```
In this case the result is the concatenation of all the descendants values. To 
get the number of nodes of a node set the function "count" is used.

``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="count(/a/*)" \
3 
```
It is also possible to get all the descendents of a node. The following seraches
for all descendants.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="//*"
[{"b":1,"c":true,"d":"foo"}, 1, true, "foo"] 
```
The following finds all "d" descendants.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="//d"
["foo"] 
```
To navigate towards the root of the data tree ".." is used.
``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/b/.."
[{"b":1,"c":true,"d":"foo"}] 
```
## Comparing values
One of the most powerful XPath features are the equality operators "=" and 
"!=". On primitive values they work pretty much as expected. There are some 
conversions between types that needs to be considered [2]. If at least one 
value is of node set type, these operators are similar to existential 
quantification in logic. This means that it can be used as universial 
quantification using a well known conversion shown below.

Assume X is a set of numbers and we want to assert that all these values are 
equal to 1 then we can write the following (in pseudo logic syntax).

```
forall x in X. x = 1
```
This is equvivalent to the following (where ~ denotes not).

```
~(exist x in X. x != 1)
```
The following examples illustrates this.

```
echo '{"a":{"b":[2, 2, 3]}}' | jxp --xpath="/a/b = 1"
false
```
No be b nodes are equal to 1.

```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="/a/b = 1"
true
```
There exist a b node that is equal to 1.

```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="not(/a/b != 1)"
false
```
All b nodes are not equal to 1.
```
echo '{"a":{"b":[1, 1, 1]}}' | jxp --xpath="not(/a/b != 1)"
true
```
Finally all b nodes are equal to 1. Ordering relations <, <=, >, >= are only 
available for numbers.
```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="/a/b < 1"
jxp, exception: Value::checkOrderingRelationArgs, can not compare node sets
```
The following example shows that if there is one value in a node set it is 
allowed when using oredering relations.
```
echo '{"a":{"b":[1]}}' | jxp --xpath="/a/b < 1"
false
```
## Filters
A filter in XPath is denoted by a pair of square brackets. It is typicaly 
applied to path expressions which returns node sets. But they can also be 
applied on primitive values. Conceptually each filter expressions is evaluated 
on each value of a node set and if it evaluates to true the corresponding node 
is kept, otherwise it is filtered away. The following example filters away all 
"b" nodes that are not equalt to 1.
```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="/a/b[. = 1]"
[1]
```
Using the count function we can check if all b nodes are less than 4.
```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="count(/a/b[. < 4]) = count(/a/b)"
true
```
The following contrived example illustrates that filters can be applied to 
primitive values and that filters can contain abritrary XPath expressions.
```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="1[count(/a/b) = 3]"
1
```
In this example a empty node set is returned as an indication of that "no value"
was returned.
```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="1[count(/a/b) = 2]"
[]
```
## Non abreviated path expression 

# Schematron

## References
[1] nlohmann json (https://json.nlohmann.me/) \
[2] XPath 1.0 (https://www.w3.org/TR/1999/REC-xpath-19991116/#NT-Number) \
[3] XSLT 3.0 (https://www.w3.org/TR/xslt-30/) \
[4] YANG rfc7950 (https://datatracker.ietf.org/doc/html/rfc7950) \
[5] Netconf rfc6241 (https://datatracker.ietf.org/doc/html/rfc6241) \
[6] Schematron (https://www.iso.org/standard/74515.html) \
[7] JSON schema (https://json-schema.org/) \
