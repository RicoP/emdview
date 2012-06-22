#!/bin/sh
cd src
clang -Wno-deprecated-writable-strings -g *.cpp -o ../export 
