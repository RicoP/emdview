#!/bin/sh
clang -g createlookup.cpp -o createlookup
./createlookup > ../src/charToFloatString.h
