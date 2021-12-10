#include "cult.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>

int main(int argc, char const** argv)
{
    util::CliFramework<cult::CultRails> app;

    app.setArgs(argc, argv);

    app.run();

    auto help = std::find(begin(app.interface.args), end(app.interface.args), std::string("--help"));
    auto command = std::find(begin(app.interface.args), end(app.interface.args), std::string("-c"));
    auto file = std::find(begin(app.interface.args), end(app.interface.args), std::string("-f"));
    auto version = std::find(begin(app.interface.args), end(app.interface.args), std::string("-v"));

    if(help != end(app.interface.args))
    {
        std::cerr << "usage: cult " << std::endl;
        std::cerr << "--help: Print this message" << std::endl;
        std::cerr << "-c <sexprs>: Evaluate a sexpr" << std::endl;
        std::cerr << "-f <file>: Evaluate a file" << std::endl;
        std::cerr << "-v : Print Version Information" << std::endl;
        app.interface.return_code = 1;
        return app.returnCode();
    }
    else if(command != end(app.interface.args))
    {
        command++;
        if(command == end(app.interface.args))
        {
            std::cout << "usage: cult -c <sexpr>" << std::endl;
            app.interface.return_code = 1;
        }

        cult::lisp::ParserEdn parser;
        auto batch = std::make_unique<cult::lisp::ParserEdn::Batch>(*command);
        batch = parser.process(std::move(batch));

        std::cout << batch->nodes->size() << std::endl;

        
        return app.returnCode();
    }
    else if(file != end(app.interface.args))
    {
        file++;
        if(file == end(app.interface.args))
        {
            std::cerr << "usage: cult -f <filename>" << std::endl;
            app.interface.return_code = 1;
        }

        auto p = std::filesystem::path(*file);

        if(!std::filesystem::exists(p))
        {
            std::cerr << "No Such File: " << *file << std::endl;
            app.interface.return_code = -1;
            return;
        }

        std::ifstream ifs(p);

        auto text = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        cult::lisp::ParserEdn parser;
        auto batch = std::make_unique<cult::lisp::ParserEdn::Batch>(text);
        batch = parser.process(std::move(batch));

        std::cout << batch->nodes->size() << std::endl;

        
        return app.returnCode();
    }
    else
    {
        std::cerr << "usage: cult -f <filename>" << std::endl;
        return app.returnCode();
    }



    return app.returnCode();
}
