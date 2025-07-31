#!/bin/bash

while true; do
    inotifywait -e modify,create,delete -r . --exclude '(\.git|\.o$|a\.out)' >/dev/null
    make
done
