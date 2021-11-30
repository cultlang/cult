#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace cult::lisp
{
    struct EdnNode
    {
        enum Flag : uint64_t
        {
            LiteralSymbol = 0,              // sym_ex/sym (no namespace if sym_ex is 0)
            LiteralKeyword = 1 << 0,        // :sym_ex/sym (no namespace if sym_ex is 0)
            LiteralNumber = 1 << 4,         // 0sym0sym_ex (no literal mode if sym_ex is 0)
            LiteralNumberFloat = 1 << 5,    // 0.sym0sym_ex (no literal mode if sym_ex is 0)
            LiteralString = 1 << 6,         // "sym"sym_ex (no literal mode if sym_ex is 0)

            DelimParen = 1 << 8,            // ( )
            DelimBrace = 1 << 9,            // { }
            DelimBracket = 1 << 10,         // [ ]
            DelimAngle = 1 << 11,           // < >

            DelimTopLevel = 1 << 15,        // SOF EOF

            MarkQuote = 1 << 16,            // '

            MarkDispatch = 1 << 20,         // #sym DELIM/LITERAL
            MarkMeta = 1 << 21,             // ^sym ?DELIM/LITERAL

            // anything larger than this is cannonically ignored
            RemarkComment = 1ll << 48l,        // ;
            RemarkDiscard = 1ll << 49l,        // #_ and similar
        };

        Flag flag;

        // symbols are an index into the symbol table of the content file, or the global table, depending on stage of resolution
        // for literals, they are "indexs" into the literal table of that type

        // primary symbol
        size_t symbol;
        // secondary symbol
        size_t symbol_ex;

        // TODO immer?
        std::vector<EdnNode const*> nodes;

        EdnNode(uint64_t flag, size_t symbol = 0, size_t symbol_ex = 0)
            : flag(static_cast<Flag>(flag)), symbol(symbol), symbol_ex(symbol_ex), nodes()
            { }
    };

    class ParserEdn
    {
    public:
        struct Batch
        {
            // The content that was parsed
            std::string content;

            // the symbol <-> content index. For both literals and symbols.
            // TODO, make string view
            std::unordered_map<std::string, size_t> contentIndex;

            // The actual store of edn nodes.
            std::shared_ptr<std::vector<EdnNode> const> nodes;

            enum class Phase
            {
                // Default
                INIT,
                // The content string has been set
                PREPARED,
                // The content string has been parsed into the index / nodes
                PARSED,
                // The nodes have been remppaed to the global data
                // content can be blanked
                REMAPPED,
                // The vector has been stolen and content blanked
                EMPTIED
            };

            Phase phase;

            Batch(std::string content)
                : content(content), contentIndex()
                , nodes(), phase(Phase::INIT)
            { }
        };

        std::unique_ptr<Batch> process(std::unique_ptr<Batch> batch, bool source_map=false);
    };
}
