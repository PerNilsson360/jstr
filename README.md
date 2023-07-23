# jstr - Json Schematron

An implementation of XPath 1.0 and a subset of Schematron for json.
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
XPath [1] is a domain specific language that was designed for XML. It has been
extensivly used as an emebeded langague. The following are some examples.

- XSLT [2], transformation of XML to XML.
- YANG [3], semantic validation data (enforcing constraints between entities).
- Netconf [4], query langauge for data.
- Schematron [5], semantic validation.

With this library its possible to use XPath as a query language for JSON data.
It is also possible to enforce semantic constraints on JSON data using
schematron. Structural and "simple" data constraints should be enforced by
validating data using JSON schema [6].

Schematron [5] is a XML language but this library implement it using JSON.
Schematron is a realtive simple language and relies on XPath to query and
compares data.

XPath has some features that are XML specic these are not implemented by this
library. The following are some examples:

- Attributes.
- Namespaces.
- Processing instructions.

Also since there are diferences between the interal data trees used by JSON and
XML there might be subtle semantic differencis between this implementation of
XPath 1.0 and a XML implementation. Having said that the intention is to follow 
[1] as much as possible.

## XPath 1.0

## References
[1] [XPath 1.0] (https://www.w3.org/TR/1999/REC-xpath-19991116/#NT-Number)