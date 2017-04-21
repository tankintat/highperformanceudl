#!/bin/bash

for file in $HOME/hpc-project/testbed/*
do
    echo "Running $file"
    echo "============" >> $HOME/hpc-project/sourcecode/results
    echo "File $file: " >> $HOME/hpc-project/sourcecode/results
    echo "Serial"
    echo "Serial: " $(./knapsackDYN $file) >> $HOME/hpc-project/sourcecode/results
    echo "Omp2"
    echo "OMP 2: " $(./knapsackDYNOMP $file 2) >> $HOME/hpc-project/sourcecode/results
    echo "Omp4"
    echo "OMP 4: " $(./knapsackDYNOMP $file 4) >> $HOME/hpc-project/sourcecode/results
    echo "Omp8"
    echo "OMP 8: " $(./knapsackDYNOMP $file 8) >> $HOME/hpc-project/sourcecode/results
    echo "===========" >> $HOME/hpc-project/sourcecode/results
done
