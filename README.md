
Requirements
============
Godot 3.1 build (https://downloads.tuxfamily.org/godotengine/3.1/beta1/)  
godot-cpp (https://github.com/GodotNativeTools/godot-cpp)  
DukTape (https://duktape.org/duktape-2.3.0.tar.xz)  
dukglue (https://github.com/Aloshi/dukglue.git)  

Setup
=====

Godot
-----
Place the Godot binary on your path as ```godot```.

godot-js
--------
Fetch this repository with --recursive or fetch the submodules after checkout with ```git submodule update --init --recursive```.

Build the godot-cpp bindings:

```
./generate-compile-bindings.sh
```

DukTape
-------
unxz your DukTape bundle inside the godot-js directory:

```
unxz duktape-2.3.0
cd duktape-2.3.0

# build static libs for duktape
../duktape_build.sh

# for OSX
cp duktape.a duktape_logging.a duk_console.a ../demo/bin/osx/

# for Linux
cp duktape.a duktape_logging.a duk_console.a ../demo/bin/x11/

cd ..
```

dukglue
-------
From the godot
git clone https://github.com/Aloshi/dukglue.git

Building
========

SCons is already configured to include the headers and link to the static libs we just built, which we can kick off with:

```
# for osx
./build.sh osx

# for x11 / linux
./build x11
```

This will generate demo/bin/<PLATFORM>/libgdjavascript.(so|dylib), while is loaded into your godot project. A demo godot project is included in demo/ which you can open and run. Currently, the library only wires up console for logging and will execute ```index.js``` in the main project directory (e.g. demo/index.js).

