#!/bin/bash

for file in $HOME/hpc-project/testbed/*
do
	qsub run-extended.sh $file
done
