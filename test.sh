#!/bin/bash

for x in {0..10};
do
    if [ `./a.out < ./tests/inputfile$x | diff -B -w - ./tests/outputfile$x | wc -l` == 0 ]; then
        echo "Test $x passed";
    else
        echo "Test $x failed";
        ./a.out < ./tests/inputfile$x | diff -B -w - ./tests/outputfile$x
    fi
done
