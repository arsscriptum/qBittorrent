#!/bin/bash


SERVER_ID=$(/usr/bin/speedtest-cli --list | head -n 2 | tail -n 1 | awk '{print $1}')
CLEAN_SERVER_ID=$(echo "$SERVER_ID" | tr -d ')')

/usr/bin/speedtest-cli --json --server $CLEAN_SERVER_ID --bytes --no-upload 