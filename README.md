# c2cppcoment

C++ supports two kinds of comments, C-style comments (/* comment */) and C++ comments (// comment).
C-style comments can span multiple lines, while C++ comments end at the end of the line.

c2cppcomment is a small word processor that reads a C++ source file and replaces C-style comments with C++ comments, if possible.
If a C-style comment does not end at the end of a line (ignoring whitespaces), it needs to be kept.

NOTE:  C++ syntax rules are not to be considered, including nested comments.

# build

`clang++ c2cppcomment.cpp -o c2cppcomment -std=c++14`

# usage

`./c2cppcomment input-file [output-file]`

NOTE: `./c2cppcomment` without an argument for print usage 
