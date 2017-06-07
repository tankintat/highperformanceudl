#!/bin/bash

## Specifies the interpreting shell for the job.
#$ -S /bin/bash

## Specifies that all environment variables active within the qsub utility be exported to the context of the job.
#$ -V

## Execute the job from the current working directory.
#$ -cwd

## Parallel programming environment (mpich) to instantiate and number of computing slots.
#$ -pe mpich 4

## The  name  of  the  job.
#$ -N knapsackDYNOutput

## Send an email at the start and the end of the job.
#$ -m be

## The email to send the queue manager notifications. 
#$ 

## The folders to save the standard and error outputs.
#$ -o $HOME/hpc-project/mpisourcecode/knapsackResult/
#$ -e $HOME/hpc-project/mpisourcecode/knapsackResult/

MPICH_MACHINES=$TMPDIR/mpich_machines
cat $PE_HOSTFILE | awk '{print $1":"$2}' > $MPICH_MACHINES


## In this line you have to write the command that will execute your application.
mpiexec -f $MPICH_MACHINES -n $NSLOTS $HOME/hpc-project/mpisourcecode/knapsackDYN-mpi $1

rm -rf $MPICH_MACHINES

