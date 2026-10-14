#!/bin/sh
# Configure and build the release preset into build/release/.
set -e
cmake --preset release
cmake --build --preset release
