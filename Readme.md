# JSON
A one-pass JSON parser with no memory overhead over the raw bytes with no
memory copying. The implementation is very simple but should parse any valid
json input, but trades validation for efficency.
One pass of the parser only reads up to the first depth of object value
and pair(key,value), but deeper levels can be reached with the parsed output.

## Features
    - written in C
    - fast
    - tiny
    - simple
    - no allocation
    - utf8 support
    - minimal validation

## Prerequisites
To build and use the JSON parser you only need to drop the two
Source files into your project and offer following environment:
    - GCC or clang compiler
    - standard library

# License
    (The MIT License)
