#!/bin/bash

for script in './get_cache.lua'
do
    for i in 1 2 5 10 20
    do
        echo "Script: $script, threads: $i, connections: $i"
        wrk -d 10 -t $i -c $i --latency -s $script http://localhost:8082/
    done
done