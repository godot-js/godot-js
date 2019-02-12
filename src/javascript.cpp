#include "javascript.h"
#include <iostream>
#include "String.hpp"
#include "javascript-adapter.h"

using namespace godot;

#define DOIT_MINARGS 2

void GDJavaScript::_register_methods() {
    register_method("_process", &GDJavaScript::_process);
    // register_property<GDJavaScript, float>("amplitude", &GDJavaScript::amplitude, 10.0);
    // register_property<GDJavaScript, float>("speed", &GDJavaScript::set_speed, &GDJavaScript::get_speed, 1.0);
    // register_signal<GDJavaScript>((char *)"position_changed", "node", GODOT_VARIANT_TYPE_OBJECT, "new_pos", GODOT_VARIANT_TYPE_VECTOR2);
}

GDJavaScript::GDJavaScript() {}

GDJavaScript::~GDJavaScript() {
    // add your cleanup here
}

void GDJavaScript::_init() {
    Godot::print("GDJavaScript(): init()");
    // fprintf(stderr, "GDJavaScript(): init()");

    JSAdapter adapter;
    adapter.init();
}

void GDJavaScript::_process(float delta) { 
    // Godot::print("GDJavaScript(): process()");
    // fprintf(stderr, "GDJavaScript(): process()");
}
