# ConFinat

ConFinat is a configuration file parser for C++. 
The design is inspired by INI files, and in some cases it might be compatible. 
However there are some features that are not supported and some that have been added as it was needed for my projects.


## Installation

ConFinat is just a header file which you can include in any of your projects.
Clone the repository or just download ConFinat.hpp and make sure it is in the include path.

## Usage

Confinat is defined within it's own namespace. It can read from a string or a file and returns a ConfData type, 
which is a std::vector of Section. Section is an struct with a name (std::string) and a pair key, value.

Since ConFinat supports several types for the values those are defined as a struct containing the type of the data
and an object for each of the possible data types.

See the example on how to use it!

## Syntax
ConFinat uses it's own syntax which is similar to that of INI files.

Sections are defined within `[` `]` and should be just a single word.
Empty lines and text after a `#` or `;` are ignored.

Each of the entries can be one of this types:
- String: Must be within double quotes `"`. It supports using `\\` to escape special characters.
- Number: A float number, it can be negative or decimal.
- Vector: A vector of floats. Should start with `{`, elements can be separated by `,` or ` ` and the closing `}` is not required. 
    - It also supports replicating multiple values with `(N, a)` which would add N times a in the vector. This can be combined like `{300, (4, 100)}`
- Boolean: A boolean value, either `True`, `TRUE` or `true` for a true value and `False`, `FALSE` or `false` for a false one.
