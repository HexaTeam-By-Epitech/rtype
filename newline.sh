#!/usr/bin/env bash

# Usage: ./add_newline.sh [path]
# If no path is given, it defaults to the current directory.

TARGET="${1:-.}"

# Iterate through all regular files
find "$TARGET" -type f | while read -r file; do
    # Skip binary files (simple heuristic)
    if grep -qI . "$file"; then
        # Check if the last byte is a newline
        if [ -s "$file" ] && [ "$(tail -c1 "$file" | wc -l)" -eq 0 ]; then
            echo >> "$file"
            echo "Added newline to: $file"
        fi
    fi
done
