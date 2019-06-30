#!/bin/bash

set -u # Exit if unset variable encountered.

# Prompt for the remote password if not specified as an argument to this script
if [ $# -eq 0 ] ; then
    read -s -p '[sudo] password: ' pass
else
    pass=$1
fi

./remote_setup.sh $pass

./1_rkvs.sh $pass
