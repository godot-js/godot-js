#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Godot.hpp"

#include "javascript-adapter.h"

bool JSAdapter::init() {
    godot::Godot::print("JSAdapter::init()\n");
    return true;
}

void JSAdapter::shutdown() {
    godot::Godot::print("JSAdapter::shutdown()\n");
}

// JSLogger

void JSLogger::log(std::string message) {
    // MethodToBind();
    printf("log(): %s", message.c_str());
    godot::Godot::print(message.c_str());
    return; //no args and no return value, no need to act on JS stack
}