# simple-transmitter
Simple transmitter written in C++ for sending text and files

## How to build

Requirements:

- CMake, version 3.25^
- G++

```
git clone https://github.com/sasetz/simple-transmitter.git
mkdir build && cmake -S . -B build/ && cmake --build build/
```

This produces binary file `simple-transmitter`. This file can then be placed anywhere you like and run using

```
./simple-transmitter
```
