#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Godot.hpp"

#include <libplatform/libplatform.h>
#include <v8.h>

#include "javascript-adapter.h"

bool shutdownNow = false;

JSAdapter::JSAdapter() {
  // do nothing
}

bool JSAdapter::init() {
    godot::Godot::print("JSAdapter::init()\n");
    // fprintf(stderr, "JSAdapter(): init()");
    v8::V8::InitializeICUDefaultLocation(".");
    v8::V8::InitializeExternalStartupData(".");
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
    // v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);

    int result;
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = CreateShellContext(isolate);
        if (context.IsEmpty()) {
            //fprintf(stderr, "Error creating V8 context\n");
            godot::Godot::print_error("Error creating V8 context", "javascript-adapter", "", 0);
            return 1;
        }
        v8::Context::Scope context_scope(context);

        result = RunMain(isolate, platform.get());
    }

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return result;
}

void JSAdapter::shutdown() {
    godot::Godot::print("JSAdapter::shutdown()\n");
    // fprintf(stderr, "JSAdapter(): shutdown()");
    // shutdown V8 here
    shutdownNow = true;
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            godot::Godot::print(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        godot::Godot::print(cstr);
    }
    godot::Godot::print("\n");
}

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) { return *value ? *value : "<string conversion failed>"; }

// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
void Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() != 1) {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Bad parameters", v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::String::Utf8Value file(args.GetIsolate(), args[0]);
    if (*file == NULL) {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file", v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::Local<v8::String> source;
    if (!ReadFile(args.GetIsolate(), *file).ToLocal(&source)) {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file", v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }

    args.GetReturnValue().Set(source);
}

// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
void Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value file(args.GetIsolate(), args[i]);
        if (*file == NULL) {
            args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file", v8::NewStringType::kNormal).ToLocalChecked());
            return;
        }
        v8::Local<v8::String> source;
        if (!ReadFile(args.GetIsolate(), *file).ToLocal(&source)) {
            args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file", v8::NewStringType::kNormal).ToLocalChecked());
            return;
        }
        if (!ExecuteString(args.GetIsolate(), source, args[i], false, false)) {
            args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Error executing file", v8::NewStringType::kNormal).ToLocalChecked());
            return;
        }
    }
}

// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // fprintf(stdout, "Quit()");

    // If not arguments are given args[0] will yield undefined which
    // converts to the integer value 0.
    int exit_code = args[0]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromMaybe(0);
    // fflush(stdout);
    // fflush(stderr);
    exit(exit_code);
}

void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion(), v8::NewStringType::kNormal).ToLocalChecked());
}

// Reads a file into a v8 string.
v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL) return v8::MaybeLocal<v8::String>();

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            return v8::MaybeLocal<v8::String>();
        }
    }
    fclose(file);
    v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, chars, v8::NewStringType::kNormal, static_cast<int>(size));
    delete[] chars;
    return result;
}

// Process remaining command line arguments and execute files
static int RunMain(v8::Isolate* isolate, v8::Platform* platform) {
    // Use all other arguments as names of files to load and run.
    v8::Local<v8::String> file_name = v8::String::NewFromUtf8(isolate, "index.js", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::String> source;
    if (!ReadFile(isolate, "index.js").ToLocal(&source)) {
        char buffer[64];
        sprintf(buffer, "Error reading '%s'\n", "index.js");
        godot::Godot::print_error(buffer, "javascript-adapter", "", 0);
        return 0;
    }
    bool success = ExecuteString(isolate, source, file_name, false, true);
    while (v8::platform::PumpMessageLoop(platform, isolate) && !shutdownNow) continue;
    if (!success) return 1;
    return 0;
}

// The read-eval-execute loop of the shell.
static void RunShell(v8::Local<v8::Context> context, v8::Platform* platform) {
    char buffer[64];
    sprintf(buffer, "V8 version %s [sample shell]\n", v8::V8::GetVersion());
    godot::Godot::print_error(buffer, "javascript-adapter", "", 0);

    static const int kBufferSize = 256;
    // Enter the execution environment before evaluating any code.
    v8::Context::Scope context_scope(context);
    v8::Local<v8::String> name(v8::String::NewFromUtf8(context->GetIsolate(), "(shell)", v8::NewStringType::kNormal).ToLocalChecked());
    while (true && !shutdownNow) {
        char buffer[kBufferSize];
        // fprintf(stderr, "> ");
        godot::Godot::print_error("> ", "javascript-adapter", "", 0);
        char* str = fgets(buffer, kBufferSize, stdin);
        if (str == NULL) break;
        v8::HandleScope handle_scope(context->GetIsolate());
        ExecuteString(context->GetIsolate(), v8::String::NewFromUtf8(context->GetIsolate(), str, v8::NewStringType::kNormal).ToLocalChecked(), name, true,
                      true);
        while (v8::platform::PumpMessageLoop(platform, context->GetIsolate()) && !shutdownNow) continue;
    }
    godot::Godot::print_error("\n", "javascript-adapter", "", 0);
}

static void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(isolate, try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Local<v8::Message> message = try_catch->Message();

    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        // fprintf(stderr, "%s\n", exception_string);
        godot::Godot::print_error(exception_string, "javascript-adapter", "", 0);
    } else {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber(context).FromJust();
        godot::Godot::print_error(exception_string, "javascript-adapter", filename_string, linenum);

        // Print line of source code.
        v8::String::Utf8Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
        const char* sourceline_string = ToCString(sourceline);
        godot::Godot::print_error(sourceline_string, "javascript-adapter", filename_string, linenum);
        
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start; i++) {
          godot::Godot::print_error(" ", "javascript-adapter", filename_string, linenum);
          // fprintf(stderr, " ");
        }

        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++) {
          godot::Godot::print_error("^", "javascript-adapter", filename_string, linenum);
          // fprintf(stderr, "^");
        }
        godot::Godot::print_error("\n", "javascript-adapter", filename_string, linenum);

        v8::Local<v8::Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) && stack_trace_string->IsString() &&
          v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
          v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
          const char* stack_trace_string = ToCString(stack_trace);
          // fprintf(stderr, "%s\n", stack_trace_string);
          godot::Godot::print_error(stack_trace_string, "javascript-adapter", filename_string, linenum);
          godot::Godot::print_error("\n", "javascript-adapter", filename_string, linenum);
        }
    }
}

// Executes a string within the current v8 context.
bool ExecuteString(v8::Isolate* isolate, v8::Local<v8::String> source, v8::Local<v8::Value> name, bool print_result, bool report_exceptions) {
    v8::HandleScope handle_scope(isolate);
    v8::TryCatch try_catch(isolate);
    v8::ScriptOrigin origin(name);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, source, &origin).ToLocal(&script)) {
        // Print errors that happened during compilation.
        if (report_exceptions) ReportException(isolate, &try_catch);
        return false;
    } else {
        v8::Local<v8::Value> result;
        if (!script->Run(context).ToLocal(&result)) {
            assert(try_catch.HasCaught());
            // Print errors that happened during execution.
            if (report_exceptions) ReportException(isolate, &try_catch);
            return false;
        } else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                // If all went well and the result wasn't undefined then print
                // the returned value.
                v8::String::Utf8Value str(isolate, result);
                const char* cstr = ToCString(str);
                // printf("%s\n", cstr);
                godot::Godot::print(cstr);
                godot::Godot::print("\n");
            }
            return true;
        }
    }
}

// Creates a new execution environment containing the built-in
// functions.
v8::Local<v8::Context> JSAdapter::CreateShellContext(v8::Isolate* isolate) {
    // Create a template for the global object.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    // Bind the global 'print' function to the C++ Print callback.
    global->Set(v8::String::NewFromUtf8(isolate, "print", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, &Print));
    // Bind the global 'read' function to the C++ Read callback.
    global->Set(v8::String::NewFromUtf8(isolate, "read", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Read));
    // Bind the global 'load' function to the C++ Load callback.
    global->Set(v8::String::NewFromUtf8(isolate, "load", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Load));
    // Bind the 'quit' function
    global->Set(v8::String::NewFromUtf8(isolate, "quit", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Quit));
    // Bind the 'version' function
    global->Set(v8::String::NewFromUtf8(isolate, "version", v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Version));

    return v8::Context::New(isolate, NULL, global);
}
