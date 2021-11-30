#include "lisp/syntax/parser_edn.hpp"
#include "lisp/lisp.hpp"

#include "src/lisp/internal.h"

#include <deque>
#include <stack>

#include "tao/pegtl.hpp"

using namespace cult::lisp;

namespace edn_grammar
{
    // Actual grammar

    // Use grammar library
    using namespace tao::pegtl;

    // Forward declare
    struct anything;
    struct delim_paren;

    // sexpr primitives
    struct delim_paren_open : one< '(' > {};
    struct delim_paren_close : one < ')' > {};

    // vector primitives
    struct delim_bracket_open : one< '[' > {};
    struct delim_bracket_close : one < ']' > {};

    // map primitives
    struct delim_brace_open : one< '{' > {};
    struct delim_brace_close : one < '}' > {};

    // Comments are introduced by a ';' and proceed to the end-of-line/file,
    // or until the matching paren, e.g. ;(comment)
    struct line_comment : until< eolf > {};
    struct sexpr_comment : if_must< at< delim_paren_open >, disable< delim_paren > > {};

    struct comment : if_must< one< ';' >, sor< sexpr_comment, line_comment > > {};

    // Nothing combination
    struct nothing : sor< space, one< ',' >, comment > {};

    // Symbol types
    // TODO add unicode support by including the ICU library
    struct symbol_first_char : sor< ascii::alpha,
        one< '.' >, one< '*' >, one< '+' >, one< '-' >, one< '/' >, one< '!' >,
        one< '_' >, one< '?' >, one< '$' >, one< '%' >, one< '&' >, one< '=' >,
        one< '<' >, one< '>' >
    > {};
    struct symbol_next_char : sor < ascii::digit, symbol_first_char > {};

    struct symbol_chars : seq< symbol_first_char, star < symbol_next_char > > {};
    struct symbol : symbol_chars {};
    struct keyword : seq< one< ':' >, symbol_chars > {};

    struct literal_plus_or_minus : sor < one < '-' >, one < '+' > > {};
    struct literal_float_seq : sor < 
        seq< plus< digit >, one< '.' >, star< digit > >,
        seq< star< digit >, one< '.' >, plus< digit > >
    > {};
    struct literal_float : seq< opt< literal_plus_or_minus >, literal_float_seq, opt< symbol > > {};
    struct literal_int : seq< opt< literal_plus_or_minus >, plus< digit >, opt< symbol > > {};
    struct number : sor<literal_float, literal_int> {};

    struct null : tao::pegtl::string< 'n', 'u', 'l', 'l' > {};
    struct true_ : tao::pegtl::string< 't', 'r', 'u', 'e' > {};
    struct false_ : tao::pegtl::string < 'f', 'a', 'l', 's', 'e' > {};
    

    struct triplequote : tao::pegtl::string<'"', '"', '"'> {};

    struct triplestring : if_must< triplequote, until<triplequote> > {};
    struct singlestring : if_must< one< '"' >, until< one< '"' > > > {};
    struct strings : sor< triplestring, singlestring > {};

    struct atom : sor< null, true_, false_, strings, number, keyword, symbol > {};

    // List type
    struct delim_paren : if_must< delim_paren_open, until< delim_paren_close, anything > > {};
    struct delim_bracket : if_must< delim_bracket_open, until< delim_bracket_close, anything > > {};
    struct delim_brace : if_must< delim_brace_open, until< delim_brace_close, anything > > {};

    struct sexpr : sor< delim_paren, delim_bracket, delim_brace > {};

    // Meta
    struct something : sor< sexpr, atom > {};
    struct anything : sor< nothing, something > {};

    struct grammar : until< tao::pegtl::eof, anything > {};

    // States
    struct ParseState
    {
        ParserEdn::Batch& batch;

        std::deque<EdnNode> nodes;
        std::stack<EdnNode*> stack;

        size_t next_sym;

        ParseState(ParserEdn::Batch& batch)
            : batch(batch), nodes(), stack(), next_sym(1)
        {
            auto ptr = &nodes.emplace_back(EdnNode::DelimTopLevel);
            stack.push(ptr);
        }

        inline size_t symbol(std::string s)
        {
            if (auto it { batch.contentIndex.find(s) }; it != std::end(batch.contentIndex))
                return it->second;
            else
                return  batch.contentIndex[s] = next_sym++;
        }

        template<typename... TArgs>
        inline EdnNode* next(TArgs&&... args)
        {
            auto ptr = &nodes.emplace_back(std::forward<TArgs>(args)...);
            stack.top()->nodes.push_back(ptr);
            return ptr;
        }

        template<typename... TArgs>
        inline EdnNode* push(TArgs&&... args)
        {
            auto ptr = next(std::forward<TArgs>(args)...);
            stack.push(ptr);
            return ptr;
        }

        inline void pop()
        {
            stack.pop();
        }

        inline void finalize()
        {
            std::vector<EdnNode> batch_nodes;
            batch_nodes.reserve(nodes.size());

            for (auto n : nodes)
            {
                batch_nodes.emplace_back(n);
            }

            batch.nodes = std::make_shared<std::vector<EdnNode> const>(std::move(batch_nodes));
            batch.phase = ParserEdn::Batch::Phase::PARSED;
        }
    };

    // Actions

    template< typename Rule >
    struct lisp_action
        : tao::pegtl::nothing< Rule >
    {};
    
    template<>
    struct lisp_action< delim_paren_open >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.push(EdnNode::DelimParen);
        }
    };

    template<>
    struct lisp_action< delim_paren_close >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.pop();
        }
    };

    template<>
    struct lisp_action< delim_bracket_open >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.push(EdnNode::DelimBracket);
        }
    };

    template<>
    struct lisp_action< delim_bracket_close >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.pop();
        }
    };

    template<>
    struct lisp_action< delim_brace_open >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.push(EdnNode::DelimBrace);
        }
    };

    template<>
    struct lisp_action< delim_brace_close >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.pop();
        }
    };

    template<>
    struct lisp_action< symbol >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.next(EdnNode::LiteralSymbol, ps.symbol(in.string()));
        }
    };

    template<>
    struct lisp_action< keyword >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.next(EdnNode::LiteralKeyword, ps.symbol(in.string()));
        }
    };

    template<>
    struct lisp_action< literal_int >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.next(EdnNode::LiteralNumber, ps.symbol(in.string()));
            /*
            ps.top()->cell_locs.push_back(in.position().byte);
            std::string s = in.string();
            auto ind = s.find('i');
            if (ind == s.npos)
            {
                ps.top()->cells.push_back(craft::types::cpptype<int64_t>::typeDesc().getFeature<PParse>()->parse(s));
            }
            else
            {
                auto suf = s.substr(ind);
                if (suf == "") ps.top()->cells.push_back(craft::types::cpptype<int64_t>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "i8") ps.top()->cells.push_back(craft::types::cpptype<int8_t>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "i16") ps.top()->cells.push_back(craft::types::cpptype<int16_t>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "i32") ps.top()->cells.push_back(craft::types::cpptype<int32_t>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "i64") ps.top()->cells.push_back(craft::types::cpptype<int64_t>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else throw stdext::exception("No Such Integer Literal Suffix: {0}", suf);
            }
            */
        }
    };

    template<>
    struct lisp_action< literal_float >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            ps.next(EdnNode::LiteralNumberFloat, ps.symbol(in.string()));
            /*
            ps.top()->cell_locs.push_back(in.position().byte);
            std::string s = in.string();
            auto ind = s.find('f');
            if (ind == s.npos)
            {
                ps.top()->cells.push_back(craft::types::cpptype<double>::typeDesc().getFeature<PParse>()->parse(s));
            }
            else
            {
                auto suf = s.substr(ind);
                if (suf == "") ps.top()->cells.push_back(craft::types::cpptype<double>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "f") ps.top()->cells.push_back(craft::types::cpptype<float>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "d") ps.top()->cells.push_back(craft::types::cpptype<double>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "f32") ps.top()->cells.push_back(craft::types::cpptype<float>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else if (suf == "f64") ps.top()->cells.push_back(craft::types::cpptype<double>::typeDesc().getFeature<PParse>()->parse(s.substr(0, ind)));
                else throw stdext::exception("No Such Float Literal Suffix: {0}", suf);
            }
            */
        }
    };

    template<>
    struct lisp_action< false_ >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            //ps.next(EdnNode::LiteralFalse)
        }
    };

    template<>
    struct lisp_action< true_ >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            //ps.next(EdnNode::LiteralTrue)
        }
    };

    template<>
    struct lisp_action< null >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            //ps.next(EdnNode::LiteralNull)
        }
    };

    template<>
    struct lisp_action< triplestring >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            //std::string pstr = in.string();
            //ps.top()->cells.push_back(instance<std::string>::make(pstr.substr(1 * 3, pstr.size() - 2 * 3)));
        }
    };

    template<>
    struct lisp_action< singlestring >
    {
        template<typename Input>
        static void apply(Input const& in, ParseState& ps)
        {
            //ps.top()->cell_locs.push_back(in.position().byte);
            //std::string pstr = in.string();
            //ps.top()->cells.push_back(instance<std::string>::make(pstr.substr(1, pstr.size() - 2)));
        }
    };
}

std::unique_ptr<ParserEdn::Batch> ParserEdn::process(std::unique_ptr<ParserEdn::Batch> batch, bool source_map)
{
    using namespace edn_grammar;

    ParseState parse_state = { *batch };

    string_input<> in(batch->content, std::string(""));
    parse< grammar, lisp_action >(in, parse_state);

    try
    {
        parse< grammar, lisp_action >(in, parse_state);
        
        // TODO, need a way to partially parse parts, and resume at the right spot
        if (parse_state.stack.size() != 1)
            throw std::runtime_error("Parse went horribly wrong.");

    }
    catch (...)
    {

    }

    parse_state.finalize();

    return std::move(batch);
}