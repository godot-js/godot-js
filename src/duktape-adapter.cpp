#include "duktape-adapter.h"

#include "duktape.h"
#include "dukglue.h"
#include "duk_logging.h"
#include "duk_console.h"

bool DukTapeAdapter::init() {
    godot::Godot::print("DukTapeAdapter::init()\n");

    // duktape setup
    duk_context *ctx = NULL;
    ctx = duk_create_heap_default();
    if (!ctx) {
        printf("Failed to create a Duktape heap.\n");
        exit(1);
    }

    /* After initializing the Duktape heap or when creating a new
    * thread with a new global environment:
    */
    duk_console_init(ctx, 0 /*flags*/);

    // bind logger
    // JSLogger* logger = new JSLogger();
    // dukglue_register_global(ctx, logger, "console");

    // dukglue_register_function(ctx, &godot::NativeLogger::log, "godot_print");

    // example exec
    char line[4096];
    char idx;
    int ch;

    push_file_as_string(ctx, "index.js");
    if (duk_peval(ctx) != 0) {
        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
    }

    duk_pop(ctx);  /* ignore result */

    //duk_get_prop_string(ctx, -1, "stack");
    //std::cout << duk_safe_to_string(ctx, -1) << std::endl;
    //duk_pop(ctx);
    return true;
}

void DukTapeAdapter::shutdown() {
    godot::Godot::print("DukTapeAdapter::shutdown()\n");
    JSAdapter::shutdown();
}

/* For brevity assumes a maximum file length of 16kB. */
void push_file_as_string(duk_context *ctx, const char *filename) {
    FILE *f;
    size_t len;
    char buf[16384];

    f = fopen(filename, "rb");
    if (f) {
        len = fread((void *) buf, 1, sizeof(buf), f);
        fclose(f);
        duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
    } else {
        duk_push_undefined(ctx);
    }
}