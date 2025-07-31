#!/bin/bash

HOST="127.0.0.1"
PORT="3490"
N=50 # Number of concurrent requests

for i in $(seq 1 $N); do
    {
        # Prepare unique message
        FILE_CONTENT="file$i"
        MSG="<|COMMAND|>cat x<|FILES|><|>x,$FILE_NAME<|><|END|>"

        echo -e "$MSG \n" | nc -q 1 $HOST $PORT
    } &
done

wait
echo "All requests sent."