#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Godot.hpp"

#include <v8.h>
#include <libplatform/libplatform.h>

class JSAdapter {
	public:
    JSAdapter();
    bool init();
    void shutdown();
    v8::Local<v8::Context> CreateShellContext(v8::Isolate* isolate);
};

bool ExecuteString(v8::Isolate* isolate, v8::Local<v8::String> source, v8::Local<v8::Value> name, bool print_result, bool report_exceptions);
v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name);
const char* ToCString(const v8::String::Utf8Value& value);
void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
void Version(const v8::FunctionCallbackInfo<v8::Value>& args);

static int RunMain(v8::Isolate* isolate, v8::Platform* platform);
static void RunShell(v8::Local<v8::Context> context, v8::Platform* platform);
static void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);
