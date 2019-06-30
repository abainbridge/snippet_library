#!/bin/bash

set -e # Exit immediately on error.
set -u # Exit if unset variable encountered.

# Prompt for the remote password if not specified as an argument to this script
if [ $# -eq 0 ] ; then
    read -s -p '[sudo] password: ' pass
else
    pass=$1
fi


source ~/exports.sh


declare -A commands

commands["host01"]=$(cat <<-END
cd
sudo rm -rf proj_perf_tests
git clone "https://Perf test CI server:$GIT_PASS@github.com/proj" proj_perf_tests
cd proj_perf_tests/libs/rkvs
./build_tests.sh
END
)

commands["host02"]=${commands["host01"]}
commands["host03"]=${commands["host01"]}
commands["host05"]=${commands["host01"]}


source run_commands.sh
