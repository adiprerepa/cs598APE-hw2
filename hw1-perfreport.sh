#!/bin/bash

# ANSI color codes
GREEN="\e[32m"
RED="\e[31m"
RESET="\e[0m"

# Parse flags
PIANOROOM=false
ELEPHANT=false
GLOBE=false

while getopts "PEG" opt; do
  case ${opt} in
    P ) PIANOROOM=true ;;
    E ) ELEPHANT=true ;;
    G ) GLOBE=true ;;
    \? ) echo "Usage: $0 [-P] [-E] [-G]"; exit 1 ;;
  esac
done

make -j
rm -rf perf && mkdir perf && chmod 777 perf
cp main.exe perf/
mkdir -p perf/pianoroom perf/elephant perf/globe

# Function to compare generated output with baseline
compare_outputs() {
  local scene=$1
  local output_prefix=$2
  local baseline_prefix=$3
  
  echo "Comparing outputs for $scene..."
  
  diff "output/$output_prefix.ppm" "baseline-outputs/$baseline_prefix.ppm" &>/dev/null
  if [ $? -eq 0 ]; then
    echo -e "[${GREEN}PASS${RESET}] $output_prefix.ppm matches baseline."
  else
    echo -e "[${RED}FAIL${RESET}] $output_prefix.ppm differs from baseline."
  fi
  
  # Compare multiple frame outputs if they exist
  for i in $(seq -f "%07g" 0 23); do
    if [ -f "output/$output_prefix.mp4.tmp.$i.ppm" ]; then
      diff "output/$output_prefix.mp4.tmp.$i.ppm" "baseline-outputs/$baseline_prefix.mp4.tmp.$i.ppm" &>/dev/null
      if [ $? -eq 0 ]; then
        echo -e "[${GREEN}PASS${RESET}] Frame $i for $output_prefix matches baseline."
      else
        echo -e "[${RED}FAIL${RESET}] Frame $i for $output_prefix differs from baseline."
      fi
    fi
  done
}

if [ "$PIANOROOM" = true ]; then
  perf record -o perf/pianoroom/perf.data -F 99 -g -- ./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500
  perf stat -o perf/pianoroom/perfstat.txt -- ./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500
  perf script -i perf/pianoroom/perf.data | perl FlameGraph/stackcollapse-perf.pl | perl FlameGraph/flamegraph.pl --width 2400 --height 48 > perf/pianoroom/flamegraph.svg
  compare_outputs "PianoRoom" "pianoroom" "pianoroom"
fi

if [ "$ELEPHANT" = true ]; then
  perf record -o perf/elephant/perf.data -F 99 -g -- ./main.exe -i inputs/elephant.ray --ppm -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/elephant.mp4
  perf stat -o perf/elephant/perfstat.txt -- ./main.exe -i inputs/elephant.ray --ppm -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/elephant.mp4
  perf script -i perf/elephant/perf.data | perl FlameGraph/stackcollapse-perf.pl | perl FlameGraph/flamegraph.pl --width 2400 --height 48 > perf/elephant/flamegraph.svg
  compare_outputs "Elephant" "elephant" "elephant"
fi

if [ "$GLOBE" = true ]; then
  perf record -o perf/globe/perf.data -F 99 -g -- ./main.exe -i inputs/globe.ray --ppm -a inputs/globe.animate --movie -F 24 -W 100 -H 100 -o output/globe.mp4
  perf stat -o perf/globe/perfstat.txt -- ./main.exe -i inputs/globe.ray --ppm -a inputs/globe.animate --movie -F 24 -W 100 -H 100 -o output/globe.mp4
  perf script -i perf/globe/perf.data | perl FlameGraph/stackcollapse-perf.pl | perl FlameGraph/flamegraph.pl --width 2400 --height 48 > perf/globe/flamegraph.svg
  compare_outputs "Globe" "globe" "globe"
fi
