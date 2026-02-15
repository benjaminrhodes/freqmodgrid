#!/bin/bash
# Build script for FreqmodGrid VST3 Plugin

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VST3_SDK_VERSION="v3.7.10"
BUILD_DIR="${PROJECT_DIR}/build"
VST3_SDK_DIR="${PROJECT_DIR}/vst3sdk"

echo "=== FreqmodGrid VST3 Plugin Build Script ==="

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found. Please install CMake 3.20+"
    exit 1
fi

# Check for C++ compiler
if ! command -v clang++ &> /dev/null; then
    echo "Warning: clang++ not found, trying g++"
fi

# Clone VST3 SDK if not present
if [ ! -d "${VST3_SDK_DIR}" ]; then
    echo "Cloning VST3 SDK ${VST3_SDK_VERSION}..."
    git clone --depth 1 --branch ${VST3_SDK_VERSION} https://github.com/steinbergmedia/vst3sdk.git "${VST3_SDK_DIR}"
else
    echo "VST3 SDK already present at ${VST3_SDK_DIR}"
fi

# Initialize and update submodules
echo "Initializing VST3 SDK submodules..."
cd "${VST3_SDK_DIR}"
git submodule update --init --recursive

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring project..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DVST3_SDK_DIR="${VST3_SDK_DIR}" \
    -DPLUGIN_NAME="FreqmodGrid"

# Build
echo "Building..."
cmake --build . --config Release --target FrequadMod

echo ""
echo "=== Build Complete ==="
echo "Plugin location: ${BUILD_DIR}/FrequadMod.vst3"
