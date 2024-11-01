#!/bin/bash

set -euo pipefail

ASSETS_REPOSITORY="${ASSETS_REPOSITORY:-https://github.com/tibia-oce/assets}"
REPOSITORY_BRANCH="${REPOSITORY_BRANCH:-master}"
repo_url="${ASSETS_REPOSITORY}/archive/refs/heads/${REPOSITORY_BRANCH}.zip"

# Print single log line
echo ""
echo "Assets from: ${ASSETS_REPOSITORY}/tree/${REPOSITORY_BRANCH}"

download_and_extract() {
    local dir="$1"
    local path="./data/$dir"
    if [ -d "$path" ]; then return; fi

    # Download zip if it doesn't exist
    mkdir -p ./data
    local zipfile=$(mktemp ./data/assets-XXXXXX.zip)
    if ! wget -q "$repo_url" -O "$zipfile"; then
        echo "Error: Failed to download assets from $repo_url" >&2
        exit 1
    fi

    # Extract and determine the actual folder name
    if ! unzip -qo "$zipfile" -d ./data; then
        echo "Error: Failed to unzip $zipfile" >&2
        exit 1
    fi
    local extracted_dir=$(unzip -qql "$zipfile" | head -n1 | awk '{print $4}' | sed 's#/##')
    local source_dir="./data/$extracted_dir/$dir"

    # Check and move specified directory
    if [ -d "$source_dir" ]; then
        mkdir -p "$(dirname "$path")"
        mv "$source_dir" "$path"
    else
        echo "Error: Directory $dir not found in extracted content." >&2
        exit 1
    fi

    # Clean up
    rm -rf "./data/$extracted_dir" "$zipfile"
}

download_and_extract "world"
download_and_extract "items"
download_and_extract "npc"
download_and_extract "monster/monsters"

if [ -x "./Black-Tek-Server" ]; then
    exec ./Black-Tek-Server
else
    echo "Error: Executable ./Black-Tek-Server not found or not executable." >&2
    exit 1
fi
