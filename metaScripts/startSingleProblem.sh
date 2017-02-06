#!/bin/bash
#PBS -N dopTest 
#PBS -l walltime=2h
#PBS -l nodes=1:ppn=1
#PBS -j oe
#PBS -l scratch=1GB

un=`uname -a`;
if [ -z $SCRATCHDIR ]; then
   SCRATCHDIR="/scratch/penicrob/$PBS_JOBID"
   echo "setting scratchdir manually to $SCRATCHDIR on $un" >> /storage/praha1/home/penicrob/run.log
fi

mkdir $SCRATCHDIR
trap "rm -r $SCRATCHDIR" TERM EXIT

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/storage/praha1/home/penicrob/opt/lib
export PATH=/storage/praha1/home/penicrob/opt/bin:$PATH
cp -R /storage/praha1/home/penicrob/icra17dop/gacr16/* $SCRATCHDIR
cd $SCRATCHDIR/h_dop

uname -a >> /storage/praha1/home/penicrob/run.log
echo "scratchdir=$SCRATCHDIR"

OUTPUT_FOLDER="rusultsPokus"
PROBLEM="etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt"
RADIUS=1.3
RVNS=1
NUM_ITERS=10000
NUM_ITERS_UNIMPR=3000

./h_dop --gui="none" --nowait=1 --dop-initial-resolution=16 --dop-radius=$RADIUS --output="$OUTPUT_FOLDER" --problem="$PROBLEM" --use-rvns=$RVNS --num-iterations=$NUM_ITERS --num-iterations-unimproved=$NUM_ITERS_UNIMPR


mkdir -p /storage/praha1/home/penicrob/icra17dop/results/$OUTPUT_FOLDER
cp $SCRATCHDIR/h_dop/$OUTPUT_FOLDER /storage/praha1/home/penicrob/icra17dop/results/$OUTPUT_FOLDER
