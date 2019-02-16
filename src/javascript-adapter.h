#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Godot.hpp"

class JSAdapter {
    public:
        JSAdapter() {}
        virtual ~JSAdapter() {}
        virtual bool init();
        virtual void shutdown();
};

class JSLogger {
    public:
        JSLogger() {}
        virtual ~JSLogger() {}
        void log(std::string message);
};
