#pragma once
#include <vector>

namespace cult::lisp
{
    // the root and fallback bootstrap interpreter
    class CultInterpreter;
}

#include "lisp/runtime/runtime.hpp"

namespace cult::lisp
{
    class CultInterpreter
    {
        void evaluate();
    };
}
