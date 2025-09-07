#!/bin/bash
set -e  # Exit on any error

# Build using qmake
echo "Building with qmake..."
qmake
make -j2

echo "Build completed successfully!"

# Run core functionality test
echo "Building and running unit tests..."
pushd tests
qmake
make -j2
./test_bloodgasanalyzer
popd
