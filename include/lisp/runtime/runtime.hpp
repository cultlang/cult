#pragma once
#include <vector>

namespace cult::lisp
{
    // a context, required for most evaluation features
    class CultContext;
    // a "world", this contains the entire execution environment
    class CultEnvironment;
    // a thread local wrapper around the world/environment, has a context
    class CultThreadEnvironment;
    // a call chain to a context, this is the first argument for most "inline lisp" functions
    class CultContextChain;
}

#include "lisp/interpreter/interpreter.hpp"

namespace
{
    // a "world", this contains the entire execution environment
    class CultContext
    {
    public:
        CultEnvironment* environment;
        CultThreadEnvironment* thread;


        CultInterpreter interpreter;
    };

    // a "world", this contains the entire execution environment
    class CultEnvironment
    {
    public:
        std::vector<CultThreadEnvironment*> known_threads;

    };

    // a "world", this contains the entire execution environment
    class CultThreadEnvironment
    {
    public:
        static thread_local CultThreadEnvironment* l_this;
    };

    // a "world", this contains the entire execution environment
    class CultContextChain
    {
    public:
        CultContext* base_context;
    };
}
