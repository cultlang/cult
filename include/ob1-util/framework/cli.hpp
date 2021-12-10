#pragma once

#include <cassert>
#include <string>
#include <vector>


namespace util
{
    struct CliInterface
    {
        int return_code;
        int argc;
        std::vector<std::string> args;
        char const** env;
    };

    template <typename TRails>
    struct CliFramework
    {
    public:
        CliInterface interface;
        TRails rails;

    private:
        bool _setupDone_args = false;
        bool _setupDone_env = false;
        
    public:
        inline void setArgs(int argc, char const** argv)
        {
            assert(!_setupDone_args);

            interface.args.reserve(argc);
            for (int i = 0; i < argc; ++i)
            {
                interface.args.push_back(argv[i]);
            }
            _setupDone_args = true;
        }

        inline int returnCode()
        {
            return interface.return_code;
        }

        inline void run()
        {
            

            // cult::lisp::ParserEdn parser;
            // auto batch = std::make_unique<cult::lisp::ParserEdn::Batch>("((((a b c) d) e) {} [])");
            // batch = parser.process(std::move(batch));

            // std::cout << batch->nodes->size() << std::endl;
        }
    };
}
