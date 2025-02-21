#!/bin/bash


SCRIPT_PATH=$(realpath "$BASH_SOURCE")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

tmp_root=$(pushd "$SCRIPT_DIR/.." | awk '{print $1}')
ROOT_DIR=$(eval echo "$tmp_root")

VERSION_FILE=$ROOT_DIR/version.nfo
BUILD_FILE=$ROOT_DIR/build.nfo

# Get current version from version.nfo (assuming the format is major.minor.build)
current_version=$(cat "$VERSION_FILE")
IFS='.' read -r major minor build <<< "$current_version"

# Increment build number
build=$((build + 1))
new_version="$major.$minor.$build"

# Write the new version back to the version.nfo file
echo "$new_version" > "$VERSION_FILE"

# Get Git info
current_branch=$(git branch --show-current)
head_rev=$(git log --format=%h -1)
last_rev=$(git log --format=%h -2 | tail -n 1)

# Write the Git branch and revision information to build.nfo
{
    echo "$current_branch"
    echo "$head_rev"
} > "$BUILD_FILE"


echo "Version updated to $new_version"
echo "Branch and revision info saved to $BUILD_FILE"
