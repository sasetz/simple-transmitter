# simple-transmitter
Simple transmitter written in C++ for sending text and files

## Used libraries

- ZLib
- Doctest

## How to build

Requirements:

- CMake, version 3.25^
- ZLib
- Git

```
git clone https://github.com/sasetz/simple-transmitter.git
mkdir build && cmake -S . -B build/ && cmake --build build/
```

This produces binary file `simple-transmitter`. This file can then be placed anywhere you like and run using

```
build/src/simple-transmitter [-r]
```

The `-r` flag is used to signal to the program that it is going to be operating in receiving (server) mode
