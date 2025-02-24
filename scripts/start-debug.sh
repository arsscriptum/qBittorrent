#!/bin/bash

# Find the process ID of qbittorrent
PID=$(ps -A | grep qbittorrent | awk '{print $1}')

# Check if a process ID was found
if [[ -z "$PID" ]]; then
    echo "Error: qbittorrent process not found."
    exit 1
fi

killall gdbserver

# Start gdbserver and attach to the process
echo "Starting gdbserver on qbittorrent (PID: $PID) at port 9999..."sudo /usr/bin/gdbserver :9999 --attach "$PID"
