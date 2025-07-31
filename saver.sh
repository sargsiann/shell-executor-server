#!/bin/bash

echo "🔁 Watching for changes..."

while true; do
    inotifywait -e modify,create,delete -r . --exclude '(\.git|\.o$|a\.out)' >/dev/null
    echo "🛠 Rebuilding..."
    make
    echo "✅ Done."
done
