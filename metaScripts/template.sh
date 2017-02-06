#!/bin/bash
#PBS -l select=1:ncpus=1:mem=1gb:scratch_local=1gb
#PBS -l walltime=02:00:00

un=`uname -a`;

trap 'clean_scratch' TERM EXIT

export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/storage/praha1/home/penicrob/opt/lib
export PATH=/storage/praha1/home/penicrob/opt/bin:$$PATH
cp -R /storage/praha1/home/penicrob/icra17dop/gacr16/* $$SCRATCHDIR
cd $$SCRATCHDIR/h_dop


uname -a >> /storage/praha1/home/penicrob/run.log
echo "scratchdir=$$SCRATCHDIR"
date

OUTPUT_FOLDER="$OUTPUT_FOLDER"
PROBLEM="$PROBLEM"
RADIUS=$RADIUS
RESOLUTION=$RESOLUTION
RVNS=$RVNS
NUM_ITERS=$NUM_ITERS
NUM_ITERS_UNIMPR=$NUM_ITERS_UNIMPR
NAME="$PROBLEM_NAME"
MAX_L=$MAX_L
NEIGHBORHOOD_RADIUS=$NEIGHBORHOOD_RADIUS
NEIGHBORHOOD_RESOLUTION=$NEIGHBORHOOD_RESOLUTION

./main --gui="none" --name="$$NAME" --nowait=1 --dop-initial-resolution=$$RESOLUTION --dop-radius=$$RADIUS --output="$$OUTPUT_FOLDER" --problem="$$PROBLEM" --use-rvns=$$RVNS --num-iterations=$$NUM_ITERS --num-iterations-unimproved=$$NUM_ITERS_UNIMPR --neighborhood-radius=$$NEIGHBORHOOD_RADIUS --neighborhood-resolution=$$NEIGHBORHOOD_RESOLUTION

date

mkdir -p /storage/praha1/home/penicrob/icra17dop/results/$$OUTPUT_FOLDER
cp -r $$SCRATCHDIR/h_dop/$$OUTPUT_FOLDER/* /storage/praha1/home/penicrob/icra17dop/results/$$OUTPUT_FOLDER

