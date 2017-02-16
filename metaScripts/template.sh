#!/bin/bash
#PBS -l select=1:ncpus=1:mem=$MEM_PBS:scratch_local=1gb
#PBS -l walltime=$WALLTIME_PBS

un=`uname -a`;

trap 'clean_scratch' TERM EXIT

export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/storage/praha1/home/penicrob/opt/lib
export PATH=/storage/praha1/home/penicrob/opt/bin:$$PATH
export PKG_CONFIG_PATH=/storage/praha1/home/penicrob/opt/lib/pkgconfig:$$PKG_CONFIG_PATH
cp -R /storage/praha1/home/penicrob/gacr16/* $$SCRATCHDIR
cd $$SCRATCHDIR/vns_dopn


uname -a >> /storage/praha1/home/penicrob/run.log
echo "scratchdir=$$SCRATCHDIR"
date

make clean
make

OUTPUT_FOLDER="$OUTPUT_FOLDER"
PROBLEM="$PROBLEM"
RADIUS=$RADIUS
RESOLUTION=$RESOLUTION
RVNS=$RVNS
NUM_ITERS=$NUM_ITERS
NUM_ITERS_UNIMPR=$NUM_ITERS_UNIMPR
NAME="$PROBLEM_NAME"
NEIGHBORHOOD_RADIUS=$NEIGHBORHOOD_RADIUS
NEIGHBORHOOD_RESOLUTION=$NEIGHBORHOOD_RESOLUTION
MAX_COMP_TIME_MIN=$MAX_COMP_TIME_MIN

./main --gui="none" --name="$$NAME" --nowait=1 --dop-initial-resolution=$$RESOLUTION --dop-radius=$$RADIUS --output="$$OUTPUT_FOLDER" --problem="$$PROBLEM" --use-rvns=$$RVNS --num-iterations=$$NUM_ITERS --num-iterations-unimproved=$$NUM_ITERS_UNIMPR --neighborhood-radius=$$NEIGHBORHOOD_RADIUS --neighborhood-resolution=$$NEIGHBORHOOD_RESOLUTION --maximal-calculation-time-min=$$MAX_COMP_TIME_MIN

date

mkdir -p /storage/praha1/home/penicrob/vns_dopn/results/$$OUTPUT_FOLDER
cp -r $$SCRATCHDIR/vns_dopn/$$OUTPUT_FOLDER/* /storage/praha1/home/penicrob/vns_dopn/results/$$OUTPUT_FOLDER

