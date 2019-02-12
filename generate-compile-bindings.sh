#!/bin/bash

cd godot-cpp
scons platform=linux headers_dir=../godot_headers generate_bindings=yes custom_api_file=../godot_headers/api.json use_custom_api_file=yes bits=64
cd ..
