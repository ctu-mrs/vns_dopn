OUTPUT_FOLDER="result_r_0.0_nr_0.0_p_P3_b_095_run_0"
PROBLEM="etc/tsiligrides_problem3/tsiligirides_problem_3_budget_095.txt"
RADIUS=0.0
RESOLUTION=16
RVNS=1
NUM_ITERS=10000
NUM_ITERS_UNIMPR=5000
NAME="Tsiligirides Set 3"
NEIGHBORHOOD_RADIUS=0.0
NEIGHBORHOOD_RESOLUTION=16
MAX_COMP_TIME_MIN=110

#valgrind --leak-check=full -v 
./main --gui="none" --name="$NAME" --nowait=1 --dop-initial-resolution=$RESOLUTION --dop-radius=$RADIUS --output="$OUTPUT_FOLDER" --problem="$PROBLEM" --use-rvns=$RVNS --num-iterations=$NUM_ITERS --num-iterations-unimproved=$NUM_ITERS_UNIMPR --neighborhood-radius=$NEIGHBORHOOD_RADIUS --neighborhood-resolution=$NEIGHBORHOOD_RESOLUTION --maximal-calculation-time-min=$MAX_COMP_TIME_MIN 
