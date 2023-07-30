# jstr - JSON Schematron

This software implements XPath 1.0 and a subset of Schematron for
JSON. It contains a library libjstr and two binaries **jxp** and
**jstr**.

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

Note that install and uninstall targets most likely needs to be
prefixed with "sudo".

## Using binaries

``` 
jxp --help
Usage: jxp --json=<optional file> --xpath="xpath"
Evaluates a xpath expression against a JSON object.
JSON data is either read from stdin or file.
Result is printed on stdout.
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

The following is small complete example that is using libjstr to
evaluate XPath expressions.

```
#include <cassert>
#include <Jstr.hh>

using namespace Jstr::Xpath;

int
main (int argc, char *argv[])
{
    nlohmann::json json = nlohmann::json::parse(R"({"a":{"b":1}})");
    // Create a document to manage memory for internal Node obejcts.
    Document document(json);
    // Search for a all the b nodes. It happens to be only one so
    // it can be used as a XPath context node.
    // Here we are using the simple eval interface.
    Value val = eval("//b", document);
    // Here we are using the more complex Expression, Env interface.
    // First "compile" a XPath expression.
    Expression exp("count(../b) = 1");
    // Create an Env object. It represents the XPath context.
    Env env(val);
    // Evaluate the expression given a XPath context.
    Value result = exp.eval(env);
    assert(result.getBoolean()); 
    return 0;
}
``` 

## Overview

XPath [1] is a domain specific language that is designed for XML. It
has been extensivly used as an embedded language in other
languages. The following are some examples.

- XSLT [3], XPath is used when doing transformation of XML.
- YANG [4], XPath is used as a constraint language (semantic validation). 
- Netconf [5], XPath is used as a query language for data.
- Schematron [6], XPath is used for semantic validation.

With this software it is possible to use XPath as a query language for
JSON encoded data. It is also possible to enforce semantic constraints
on JSON data using schematron. Structural and "simple" data
constraints should be enforced by JSON schema [7] validation.

Schematron [6] is a XML language but this software implements a subset
of it using JSON. Schematron is a relative simple language and relies
on XPath to query and compare data.

XPath has some features that are XML specific these are skipped in
this implementation. The following are some examples:

- Attributes.
- Namespaces.
- Processing instructions.

Also since there are differences between the internal data trees used
by JSON and XML there might be subtle semantic differences between
this implementation of XPath 1.0 and a standard compliant
implementation targeting XML. Having said that the intention is to
follow [1] as much as possible.

Also note that XPath 1.0 is a small language compared to XPath
3.1. This has some benefits and of course also some drawbacks.

## XPath 1.0 tutorial

The following is a XPath 1.0 tutorial. It also shows how to use the
tool jxp.

### Basic types

XPath 1.0 has values of the the following types.

- Numbers, (floating point numbers including NaN and Infinity).
- Booleans.
- Strings.
- Node sets.

With numbers you can use many of the normal mathematical
functions. The following is an example.

``` 
echo '{}' | jxp --xpath="(5.5 + 1) div 2"
3.25 
```

It shows how to use the command line program *jxp*. Since *jxp* can
read JSON from stdin we use *echo* to pipe a trivial JSON object to
*jxp*. The result of the command is shown on the second line.

XPath 1.0 does not support boolean literals but there are functions
that can be used instead.

``` 
echo '{}' | jxp --xpath="(false() and false()) or not(false())"
true 
```

This example also illustrates boolean connectives. Also note that the
output from *jxp* is compatible with JSON syntax.

XPath supports string literals enclosed in single and double
quotes. The following examplifies this also shows some string
functions.

```
echo '{}' | jxp --xpath="string-length('foo') + string-length(substring-before(\"foo-bar\", 'bar'))"
7
```

The primitive types has casting functions.


``` 
echo '{}' | jxp --xpath="string(number(not(boolean(0))) + 1)"
"2" 
```

So boolean(0) evaluates to false and number(true) is 1. Finally the
string representation of 1 + 1 which is 2 is returned as a result.

These functions also works for node sets. In XPath a node is defined
as a node in a XML DOM tree. In this implementation we can think of a
node as a JSON object or primitive value.

## Node sets

When a XPath expression is evaluated it needs to have access to a
start node.  This node is called the *context node*. In these examples
it is always the root node. Evaluating an expression results in a
value which can be of the above mentioned types. The following is an
example showing a result with a node set with cardinality one.

``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/"
[{"a":{"b":1,"c":true,"d":"foo"}}] 
```

Note that array notation is used and nodes sets are printed as
json. Also note that "/" denotes the root node.

## Abbreviated path expressions

Path expressions are used to retrieve subsets of data nodes. Path
Expressions come in two flavors, abreviated and non abreviated. The
following shows a simple usage of using node names to select child
node sets.

``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/b"
[1] 
```

The requested b node is printed as 1 which is its JSON value. The
implementation of the node also records the name of the node which can
be seen in the following example.

``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="local-name(/a/b)"
"b" 
```

The XPath function "name" is not supported since it should print XML
namespaces which JSON does not support. To get all children of a node
the wildcard "*" is used.

``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/a/*"
[1, true, "foo"] 
```

Now we can explore what string function returns for a node set, from
[2].

"The string function converts an object to a string as follows:
 - A node-set is converted to a string by returning the string-value
   of the node in the node-set that is first in document order. If the
   node-set is empty, an empty string is returned."

"The string-value of an element node is the concatenation of the
string-values of all text node descendants of the element node in
document order."

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="string(/a/*)"
"1"
```

The first node in the above nodes set is the node with local-name "b"
its value is 1. This value is returned as a string.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="string(/a)"
"1truefoo"
```

In this case the result is the concatenation of all the descendants
values. To get the number of nodes of a node set the function "count"
is used.


``` 
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="count(/a/*)"
3 
```

It is also possible to get all the descendants of a node. The
following searches for all descendants.

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

One of the most powerful XPath features are the equality operators "="
and "!=". On primitive values they work pretty much as expected, but
there are some conversions between types that needs to be considered
for more information see [2]. If at least one value is of node set
type, these operators are similar to existential quantification in
logic. This means that it can be used as universal quantification
using a well known conversion shown below.

Assume X is a set of numbers and we want to assert that all these
values are equal to 1 then we can write the following (in pseudo logic
syntax).

```
forall x in X. x = 1
```

This is equivalent to the following (where ~ denotes not).

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

Finally all b nodes are equal to 1. Ordering relations <, <=, >, >=
are only available for numbers.

```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="/a/b < 1"
jxp, exception: Value::checkOrderingRelationArgs, can not compare node sets
```

The following example shows that if there is one value in a node set
it is allowed when using ordering relations.

```
echo '{"a":{"b":[1]}}' | jxp --xpath="/a/b < 1"
false
```

## Filters

A filter in XPath is denoted by a pair of square brackets. It is
typically applied to path expressions which returns node sets, but
they can also be applied on primitive values. Conceptually each filter
expressions is evaluated on each value of a node set and if the filter
evaluates to true the corresponding node is kept, otherwise it is
filtered away. The following example filters away all "b" nodes that
are not equal to 1.

```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="/a/b[. = 1]"
[1]
```
Note that . represents the current context item in the filter.
Using the count function we can check if all b nodes are less than 4.

```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="count(/a/b[. < 4]) = count(/a/b)"
true
```

The following contrived example illustrates that filters can be
applied to primitive values and that filters can contain arbitrary
XPath expressions.


```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="1[count(/a/b) = 3]"
1
```

In this example an empty node set is returned as an indication of that
"no value" was returned.

```
echo '{"a":{"b":[1, 2, 3]}}' | jxp --xpath="1[count(/a/b) = 2]"
[]
```
## Non abbreviated path expression

XPath 1.0 supports a big selection of non abbreviated path
expressions. Some seems to be adedd just for completenes. The the
following example shows the non abbreviated way of selecting child
node sets.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/child::a/child::b"
[1]
```

In the above XPath expression we have two steps separated by the '/'
character. Looking at the grammar in [2] we can see that a non
abbreviated step contains an axis name and a node test separated by
"::". The "attribute" axis and the "namspace" axis are not supported
for obvious reasons. Also node names and the wild card '*' are the only
supported node tests. The following illustrates the wild chard node
test.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/child::a/child::*"
[1, true, "foo"]
```

The following two examples illustrates the descendant axis. First with
a name as a node test.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/descendant::d"
["foo"]
```
Then with the wildchard as node test.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/descendant::*"
[{"b":1,"c":true,"d":"foo"}, 1, true, "foo"]
```

With the ancestor axis we can search towards the root for nodes.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/descendant::d/ancestor::a"
[{"b":1,"c":true,"d":"foo"}]
```

What is returned is a set of nodes which the following shows.

```
echo '{"a":{"b":1,"c":true,"d":"foo"}}' | jxp --xpath="/descendant::*/ancestor::a"
jxp: waiting for data on stdin.
[{"b":1,"c":true,"d":"foo"}]
```
# Schematron

The following Schematron example illustrates how XPath expressions are
used to validate semantic constraints using schematron.

```
{
  "pattern": {
    "name": "test-b2",
    "rule": {
      "context": "/a/b",
      "assert": {
        "test": ". < 3",
        "message": "all b < 3"
      }
    }
  }
}
```

Patterns are used to to group rules and they also have a name that is
used in error reporting. Rules has a context and one or more
asserts. Asserts has a test and a message. The following example uses
json arrays for patterns, rules and asserts.

```
cat schematron-2.json 
{
  "pattern": [
    {
      "name": "test-a",
      "rule": [
        {
          "context": "/",
          "assert": [
            {
              "test": "count(a)=2",
              "message": "number of a items must be 2"
            }
          ]
        }
      ]
    }
  ]
}
```
Here is the result when evaluating it against a small json payload.

```
echo '{"a":3}' | jstr --schema=schematron-2.json
test-a, error: number of a items must be 2
```

# Performance
The following is a quick test that was done just the have an intial
feeling for the performace.  Xalan xslt interpreter was used to have
something to compare jstr with. A payload with 30000 entries was
used. The following is a part of a the xml payload (the json was similar).

```
<?xml version="1.0"?>
<?xml-stylesheet type="text/xsl" href="example.xsl"?>
<root><upper-limit>2</upper-limit><a><b>1</b></a><a><b>1</b></a><a><b>1</b></a>
...
```

So it is a bunch of b nodes with a 1 as text element. The following
was the xslt that was used.

```
cat example.xsl
<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text"/>
  <xsl:template match="/">
    Sum - <xsl:value-of select="sum(/root/a/b[. &lt; ../../upper-limit])"/>
  </xsl:template>
</xsl:stylesheet>
```

It is essentially summing up all the text nodes after filtering out
all the ones that are not less than two. When running the following
script (on an office type laptop).

```
cat timed-test.sh
#! /bin/bash
JSON_FILE=data.json
XML_FILE=data.xml
time xalan -in data.xml -xsl example.xsl
time jxp --json=$JSON_FILE --xpath="sum(/root/a/b[. < ../../upper-limit])"
```

I get this.

```
./timed-test.sh

    Sum - 30000
real	0m12,871s
user	0m12,545s
sys	0m0,325s
30000

real	0m1,956s
user	0m0,984s
sys	0m0,972s
```

In this case jxp is significatly faster. If the xpath is changed so it
compares with literal 2 i.e. "sum(/root/a/b[. < 2])" The following is
printed.

```
./timed-test.sh

    Sum - 30000
real	0m0,670s
user	0m0,366s
sys	0m0,305s
30000

real	0m1,724s
user	0m0,804s
sys	0m0,920s
```

So in this case Xalan is faster. In any case jxp seems reasonable fast.

## References

[1] nlohmann json (https://json.nlohmann.me/) \
[2] XPath 1.0 (https://www.w3.org/TR/1999/REC-xpath-19991116/#NT-Number) \
[3] XSLT 3.0 (https://www.w3.org/TR/xslt-30/) \
[4] YANG rfc7950 (https://datatracker.ietf.org/doc/html/rfc7950) \
[5] Netconf rfc6241 (https://datatracker.ietf.org/doc/html/rfc6241) \
[6] Schematron (https://www.iso.org/standard/74515.html) \
[7] JSON schema (https://json-schema.org/) \

# License

MIT license

Copyright 2023 Per Nilsson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
