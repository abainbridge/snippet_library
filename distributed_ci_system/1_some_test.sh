#!/bin/bash

source ~/exports.sh


declare -A commands

commands["host01"]=$(cat <<-END
sudo pkill 4_batch_perf
cd $HOME/proj_perf_tests/tests
sudo ./4_batch_perf -l 10,12,14 -- 192.168.100.1 192.168.100.2 192.168.100.3
END
)

commands["host02"]=$(cat <<-END
sudo pkill 4_batch_perf
cd $HOME/proj_perf_tests/tests
sudo ./4_batch_perf -l 10,12,14 -- 192.168.100.2 192.168.100.1 192.168.100.3
END
)

commands["host03"]=$(cat <<-END
sudo pkill 4_batch_perf
cd $HOME/proj_perf_tests/tests
sudo ./4_batch_perf -l 10,12,14 -- 192.168.100.3 192.168.100.1 192.168.100.2
END
)


source run_commands.sh



#
# Process results
#

avgs=`grep Avg out_*.txt`
echo >>~/ci_results/1_results.txt "$avgs"
avgs=`echo "$avgs" | sed 's/\.txt/-/' | sed 's/://g' | sed 's/-/_/g' | sed 's/out_//' | awk '{ printf("%s_%s_%s=%s ", $1, $2, $3, $4) }'`
