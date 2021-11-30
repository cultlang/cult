#pragma once
#include <vector>

namespace cult::lisp
{
    // a context, required for most evaluation features
    class LispContext;
    // a "world", this contains the entire execution environment
    class LispEnvironment;
    // a thread local wrapper around the world/environment, has a context
    class LispThreadEnvironment;
    // a call chain to a context, this is the first argument for most "inline lisp" functions
    class LispContextChain;


    // a "world", this contains the entire execution environment
    class LispContext
    {
    public:
        LispEnvironment* environment;
        LispThreadEnvironment* thread;

    };

    // a "world", this contains the entire execution environment
    class LispEnvironment
    {
    public:
        std::vector<LispThreadEnvironment*> known_threads;

    };

    // a "world", this contains the entire execution environment
    class LispThreadEnvironment
    {
    public:
        static thread_local LispThreadEnvironment* l_this;
    };

    // a "world", this contains the entire execution environment
    class LispContextChain
    {
    public:
        LispContext* base_context;
    };
}
