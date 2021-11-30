
#include "lisp/lisp.hpp"

#include "cult.hpp"

int main(int argc, char const** argv)
{
    util::CliFramework<cult::CultRails> app;

    app.setArgs(argc, argv);

    app.run();

    return app.returnCode();
}
