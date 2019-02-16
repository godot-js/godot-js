#!/bin/bash

PLATFORM=$1

if [[ "$PLATFORM" == "osx" ]]; then
    echo "Building for OSX"
elif [[ "$PLATFORM" == "x11" ]]; then
    echo "Building for X11"
elif [[ "$PLATFORM" == "linux" ]]; then
    echo "Building for Linux"
else
    echo "usage: ./build.sh <platform>"
    echo "  platform: (osx|x11|linux)"
    exit 1;
fi

scons platform=$PLATFORM bits=64 -c
scons platform=$PLATFORM headers_dir=./godot-cpp/godot_headers custom_api_file=./godot-cpp/godot_headers/api.json use_custom_api_file=yes godotbinpath=~/bin/godot

#scons platform=linux headers_dir=./godot_headers generate_bindings=yes custom_api_file=./godot_headers/api.json use_custom_api_file=yes godotbinpath=~/bin/godot
