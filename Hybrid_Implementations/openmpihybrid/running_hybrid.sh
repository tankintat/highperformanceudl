#!/bin/bash

for file in $HOME/hpc-project/testbed/*
do
	qsub run-hybrid.sh $file
done

