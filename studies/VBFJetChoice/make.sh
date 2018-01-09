#!/bin/bash

echo 'compiling...'
c++ -lm -o VBFJetChoice VBFJetChoice.cpp `root-config --glibs --cflags` -I ../../interface/
echo 'compiled; executing...'
./VBFJetChoice
