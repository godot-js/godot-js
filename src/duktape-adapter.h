#include "javascript-adapter.h"
#include "duktape.h"

class DukTapeAdapter: public JSAdapter {
	public:
    DukTapeAdapter() {};
    virtual ~DukTapeAdapter() {}
    bool init();
    virtual void shutdown();
};

void push_file_as_string(duk_context *ctx, const char *filename);