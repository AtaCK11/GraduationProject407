#!/bin/bash

# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
RESET='\033[0m'

# Initialize associative arrays to keep track of lines added, deleted, and net lines by each author
declare -A author_lines_added
declare -A author_lines_deleted

# Fetch all commit statistics and only process the top author
git log --numstat --pretty=format:'%an' | awk '
BEGIN {
    FS="\t";
    OFS="\t";
}
{
    if (NF == 1) {
        author = $1;
    } else if (NF == 3) {
        added[author] += $1;
        deleted[author] += $2;
    }
}
END {
    for (author in added) {
        print author, added[author], deleted[author], added[author] - deleted[author];
    }
}' | sort -k2,2nr | head -n 1 | while read -r author added_lines deleted_lines net_lines; do
  # Display the result for the top author
  echo -e "${MAGENTA}${author}${RESET}: ${GREEN}Added:${RESET} ${added_lines} lines, ${RED}Deleted:${RESET} ${deleted_lines} lines, ${BLUE}Net:${RESET} ${net_lines} lines"
done

echo -e "${BLUE}Press any key to exit...${NC}"
read -n 1 -s
