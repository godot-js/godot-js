#!/bin/bash

cd godot-cpp
scons platform=osx bits=64 -c
scons platform=osx generate_bindings=yes bits=64
cd ..
