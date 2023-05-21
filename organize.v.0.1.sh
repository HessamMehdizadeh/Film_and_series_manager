#!/bin/bash

download_dir="D:/Series/TEST"
series_name="*"

for file in $(find $download_dir -type f -name "$series_name*.mkv"); do
    # Extract the series name, season, and episode number
    series=$(echo $file | sed -r 's/^(.*)\.[s|S]([0-9]+)[e|E]([0-9]+).*$/\1/' | tr '.' ' ')
    season=$(echo $file | sed -r 's/^.*\.[s|S]([0-9]+).*$/\1/')
    episode=$(echo $file | sed -r 's/^.*\.[s|S][0-9]+[e|E]([0-9]+).*$/\1/')
    # check if the variables are numeric
    if ! [[ $season =~ ^[0-9]+$ ]]; then
        echo "Could not extract season from file: $file"
        continue
    fi
    if ! [[ $episode =~ ^[0-9]+$ ]]; then
        echo "Could not extract episode from file: $file"
        continue
    fi
    # Create the folder structure
    mkdir -p "$series/Season $season/Episode $episode"

    # Move the file
    mv "$file" "$series/Season $season/Episode $episode"
done
