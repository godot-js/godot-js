#!/bin/bash

cd duktape && \
rm -fR src-custom && \
python tools/configure.py \
          --source-directory src-input \
          --output-directory src-custom \
          --config-metadata config \
          -DDUK_USE_FASTINT && \

# NOTE: osx / linux are the only two targets that have been tested thusfar. YMMV
# run this from within your duktape 2.x release tarball and then copy the .a files to demo/bin/<PLATFORM>/ where <PLATFORM> is osx / x11 / linux
g++ -c -static -o duktape.a -Isrc-custom/ -x c src-custom/duktape.c
g++ -c -static -o duktape_logging.a -Isrc-custom/ -Iextras/logging/ -I. -x c extras/logging/duk_logging.c
g++ -c -static -o duk_console.a -Isrc-custom/ -Iextras/console/ -I. -x c extras/console/duk_console.c

cd ..