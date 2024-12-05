#!/bin/bash

outputs_directory="./outputs"
if [ -n "$(ls -A "$outputs_directory")" ]; then
    rm "$outputs_directory"/*
fi

# input_file_name="inp.txt"
# g++ -std=c++17 seq.cpp && ./a.out "$input_file_name"

# To loop through all files in the subfolders of the 'testcases' directory and run all the test cases
for input_file_name in ./inputs/*.txt; do
    g++ -std=c++17 sequential.cpp && ./a.out "$input_file_name"
    echo "--------------------------"
    g++ -std=c++17 parallel.cpp && ./a.out "$input_file_name"
    echo "--------------------------"
    python3 python_Code.py "$input_file_name"
    echo "==================================================="
done