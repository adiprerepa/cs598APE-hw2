#!/bin/bash

# ANSI color codes`2`
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
make clean && make -j && rm -rf output && mkdir output

# Create perf directory structure
chmod 777 perf && rm -rf perf && mkdir perf && chmod 777 perf
mkdir -p perf/diabetes perf/cancer perf/housing

if [ "$DIABETES" = true ]; then
  echo -e "Running ${GREEN}Diabetes${RESET} benchmark..."
  perf record -o perf/diabetes/perf.data -F 1000 --call-graph dwarf -g -- ./genetic_benchmark diabetes
  perf stat -j -o perf/diabetes/perfstat.json -- ./genetic_benchmark diabetes > output/diabetes.txt
  perf script --no-demangle -i perf/diabetes/perf.data | perl FlameGraph/stackcollapse-perf.pl > perfstacks/diabetes.folded
fi

if [ "$CANCER" = true ]; then
  echo -e "Running ${GREEN}Cancer${RESET} benchmark..."
  perf record -o perf/cancer/perf.data -F 1000 --call-graph dwarf -g -- ./genetic_benchmark cancer
  perf stat -j -o perf/cancer/perfstat.json -- ./genetic_benchmark cancer > output/cancer.txt
  perf script --no-demangle -i perf/cancer/perf.data | perl FlameGraph/stackcollapse-perf.pl > perfstacks/cancer.folded
fi

if [ "$HOUSING" = true ]; then
  echo -e "Running ${GREEN}Housing${RESET} benchmark..."
  perf record -o perf/housing/perf.data -F 1000 --call-graph dwarf -g -- ./genetic_benchmark housing
  perf stat -j -o perf/housing/perfstat.json -- ./genetic_benchmark housing > output/housing.txt
  perf script --no-demangle -i perf/housing/perf.data | perl FlameGraph/stackcollapse-perf.pl > perfstacks/housing.folded
fi

echo -e "${GREEN}Performance reports generated in the 'perf' directory.${RESET}"