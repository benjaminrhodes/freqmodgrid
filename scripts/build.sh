#!/bin/bash
# Build script for FreqmodGrid FM Synthesizer Plugin

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "${SCRIPT_DIR}")"
BUILD_DIR="${PROJECT_DIR}/build"

echo "=== FreqmodGrid FM Synth Build Script ==="

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install CMake 3.20+"
    exit 1
fi

# Initialize submodules if needed
if [ ! -d "${PROJECT_DIR}/iPlug2/IPlug" ]; then
    echo "Initializing iPlug2 submodule..."
    cd "${PROJECT_DIR}"
    git submodule update --init --recursive
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring project..."
cmake "${PROJECT_DIR}" \
    -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
cmake --build . --config Release --target FreqmodGrid

echo ""
echo "=== Build Complete ==="
echo "Plugin location: ${BUILD_DIR}/FreqmodGrid.vst3"
