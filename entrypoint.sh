#!/bin/bash

set -euo pipefail

# Logging function with timestamps
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $@"
}

download_and_extract() {
    local dir="$1"
    local path="./data/$dir"
    local repo_url="https://github.com/tibia-oce/assets/archive/refs/heads/master.zip"
    local zipfile="./data/$(basename "$repo_url")"

    # Check if the directory already exists to avoid redundant downloads
    if [ ! -d "$path" ]; then
        log "$dir directory not found."

        # Check if the zip file already exists
        if [ -f "$zipfile" ]; then
            log "$zipfile already exists. Skipping download."
        else
            log "Downloading $zipfile..."
            mkdir -p ./data
            if wget -q "$repo_url" -O "$zipfile"; then
                log "Downloaded $zipfile successfully."
            else
                log "Error downloading $zipfile from $repo_url"
                exit 1
            fi
        fi

        # Unzip the zip file and capture the extracted directory name dynamically
        if unzip -q "$zipfile"; then
            local extracted_dir=$(unzip -qql "$zipfile" | head -n1 | awk '{print $4}' | sed 's#/##')
            log "Extracted directory $extracted_dir from $zipfile successfully."
        else
            log "Error extracting $zipfile"
            exit 1
        fi

        # Handle nested directory structure
        local source_dir="$extracted_dir/$dir"
        if [ -d "$source_dir" ]; then
            # Ensure the nested target directory structure exists
            mkdir -p "$(dirname "$path")"
            mv "$source_dir" "$path"
            log "Moved $dir directory to target location."
        else
            log "Directory $dir not found in extracted content."
            exit 1
        fi

        # Clean up extracted content but retain the zipfile
        rm -rf "$extracted_dir"
    else
        log "$dir directory already exists. Skipping download."
    fi
}

# Download and extract necessary directories
download_and_extract "world"
download_and_extract "items"
download_and_extract "npc"
download_and_extract "monster/monsters"

# Start the executable
if [ -x "./Black-Tek-Server" ]; then
    log "Starting Black-Tek-Server..."
    exec ./Black-Tek-Server
else
    log "Executable ./Black-Tek-Server not found or not executable."
    exit 1
fi
