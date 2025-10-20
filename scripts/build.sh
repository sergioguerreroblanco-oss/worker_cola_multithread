#!/usr/bin/env bash
set -e

# Default preset
preset="release"

# Parse arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    --preset)
      preset="$2"
      shift 2
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

echo "Building project with preset: $preset"

# Run CMake configure and build
cmake --preset "$preset"
cmake --build --preset "$preset"

echo "Build completed."
echo "Binary available at: build/$preset/worker_cola_multithread[.exe]"
