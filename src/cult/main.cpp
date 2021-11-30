#include "cult.hpp"

#include <iostream>

int main(int argc, char const** argv)
{
    util::CliFramework<cult::CultRails> app;

    app.setArgs(argc, argv);

    app.run();

    cult::lisp::ParserEdn parser;
    auto batch = std::make_unique<cult::lisp::ParserEdn::Batch>("((((a b c) d) e) {} [])");
    batch = parser.process(std::move(batch));

    std::cout << batch->nodes->size() << std::endl;

    return app.returnCode();
}
