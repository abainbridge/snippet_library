#!/bin/bash
cmds=$(grep -h "pkill [^\$]" *.sh | grep -v grep | sort | uniq)
for i in $cmds; do
    echo pkilling $i
    pkill $i
done
