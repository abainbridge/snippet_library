#!/bin/bash

# This script runs batches of commands on different machines and displays their
# output in realtime, by automatically splitting the terminal window into
# chunks, one for each machine.
#
# Inputs:
# * The batches of commands must be provided in an associative array called
#   "commands". The key is hostname. The value is a single string containing the
#   commands, separated by newlines. 
# * $1 can optionally be the password for the remote machine. This is always sent
#   to the remote machine over the ssh sessions stdin, where it will be consumed
#   by the first use of sudo. If $1 is not present, you will be prompted for the
#   password interactively.
#
# Once the array is defined, then source this script.

set -e # Exit immediately on error.
set -u # Exit if unset variable encountered.

# Prompt for the remote password if not specified as an argument to this script
if [ $# -eq 0 ] ; then
    read -s -p '[sudo] password: ' pass
else
    pass=$1
fi

rsa_key=""

do_host() {
    host=$1
    cmd=$2
    echo $pass | ssh -tt -oBatchMode=yes -o StrictHostKeyChecking=no $rsa_key $host "$cmd" &> out_$1.txt
}

let "lines_per_cmd = $(tput lines) / ${#commands[@]} - 4"
rm out_*.txt

# Start the commands on the remote machines and get the PIDs of the ssh sessions.
declare -A pids
for hostname in ${!commands[@]}; do
    echo "Starting command on $hostname"
    do_host $hostname "${commands[$hostname]}" &
    pids[$hostname]=$!
done

# Monitor the ssh PIDs until they all cease to exist. Scrap the remote outputs
# repeatedly and show the last few lines of each.
keep_going=true
while $keep_going; do
    # Echo enough blank lines to make sure that we've moved output from the
    # previous iteration of this loop off the screen.
    for i in $(seq 1 ${#commands[@]}); do echo; done

    current_pids=`ps | awk '{ print $1 }'`

    keep_going=false
    for hostname in ${!commands[@]}; do
        if [[ $current_pids == *${pids[$hostname]}* ]]; then
            keep_going=true
        fi
        
        echo - $hostname -------------------------------------------------------------
        echo
        lines=`tail -n $lines_per_cmd out_$hostname.txt`
        echo -e "$lines"

        # Print some padding lines if the output file was less than $lines_per_cmd lines long.
        num_lines=`echo "$lines" | wc -l`
        for i in $(seq $num_lines $lines_per_cmd); do echo; done
    done

    sleep 1
done
