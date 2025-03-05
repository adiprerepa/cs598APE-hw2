#!/bin/bash

# ANSI color codes
GREEN="\e[32m"
RED="\e[31m"
RESET="\e[0m"

# Parse flags
DIABETES=false
CANCER=false
HOUSING=false
ALL=false

while getopts "DCHAh" opt; do
  case ${opt} in
    D ) DIABETES=true ;;
    C ) CANCER=true ;;
    H ) HOUSING=true ;;
    A ) ALL=true ;;
    h ) echo "Usage: $0 [-D] [-C] [-H] [-A] (Diabetes, Cancer, Housing, All)"; exit 0 ;;
    \? ) echo "Usage: $0 [-D] [-C] [-H] [-A] (Diabetes, Cancer, Housing, All)"; exit 1 ;;
  esac
done

# If no arguments, show help and exit
if [ $OPTIND -eq 1 ]; then
  echo "Usage: $0 [-D] [-C] [-H] [-A] (Diabetes, Cancer, Housing, All)"
  exit 1
fi

# If -A is used, run all benchmarks
if [ "$ALL" = true ]; then
  DIABETES=true
  CANCER=true
  HOUSING=true
fi

# Build the benchmark executable
make -j

# Create perf directory structure
rm -rf perf && mkdir perf
mkdir -p perf/diabetes perf/cancer perf/housing

if [ "$DIABETES" = true ]; then
  echo -e "Running ${GREEN}Diabetes${RESET} benchmark..."
  perf record -o perf/diabetes/perf.data -F 99 -g -- ./genetic_benchmark diabetes
  perf stat -o perf/diabetes/perfstat.txt -- ./genetic_benchmark diabetes
  perf script -i perf/diabetes/perf.data | perl FlameGraph/stackcollapse-perf.pl > perfstacks/diabetes.folded
  perf script -i perf/diabetes/perf.data | perl FlameGraph/stackcollapse-perf.pl | perl FlameGraph/flamegraph.pl --width 2400 --height 48 > perf/diabetes/flamegraph.svg
fi

if [ "$CANCER" = true ]; then
  echo -e "Running ${GREEN}Cancer${RESET} benchmark..."
  perf record -o perf/cancer/perf.data -F 99 -g -- ./genetic_benchmark cancer
  perf stat -o perf/cancer/perfstat.txt -- ./genetic_benchmark cancer
  perf script -i perf/cancer/perf.data | perl FlameGraph/stackcollapse-perf.pl > perfstacks/cancer.folded
  perf script -i perf/cancer/perf.data | perl FlameGraph/stackcollapse-perf.pl | perl FlameGraph/flamegraph.pl --width 2400 --height 48 > perf/cancer/flamegraph.svg
fi

if [ "$HOUSING" = true ]; then
  echo -e "Running ${GREEN}Housing${RESET} benchmark..."
  perf record -o perf/housing/perf.data -F 99 -g -- ./genetic_benchmark housing
  perf stat -o perf/housing/perfstat.txt -- ./genetic_benchmark housing
  perf script -i perf/housing/perf.data | perl FlameGraph/stackcollapse-perf.pl > perfstacks/housing.folded
  perf script -i perf/housing/perf.data | perl FlameGraph/stackcollapse-perf.pl | perl FlameGraph/flamegraph.pl --width 2400 --height 48 > perf/housing/flamegraph.svg
fi

echo -e "${GREEN}Performance reports generated in the 'perf' directory.${RESET}"