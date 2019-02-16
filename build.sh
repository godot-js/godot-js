#!/bin/bash

scons platform=osx bits=64 -c
scons platform=osx headers_dir=./godot_headers custom_api_file=./godot_headers/api.json use_custom_api_file=yes godotbinpath=~/bin/godot
#scons platform=linux headers_dir=./godot_headers generate_bindings=yes custom_api_file=./godot_headers/api.json use_custom_api_file=yes godotbinpath=~/bin/godot
