(suggested by github: "how about `improved-meme`")

Another attempt at cultlang. But this time as a more minimalist attempt, focused around doing Advent of Code stuff and things.

#### Ideas

Ideas to do this better this time

- Using an ECS for the type system, rather than our custom graph system. (library flecs)
- Supporting exactly EDN. (custom parser)
- Better AST as type-traits design from the beginning. (custom built into our type system)
- Simpler module/namespace/symbol/loading system. (custom built into our type system)
- No compiler till much later. (llvm, but not till later)
  - Maybe a simple optimizer pass. (custom pass system)
- Simpler evaluator from the beginning.
- Better baked in data structures. (library immer)
- Thread/Execution stack first design. (no globals or thread locals, passing the stack context as a variable)

#### Features

Features to die for, these are the things I really want:

- Context sensitive ASTs / symbol resolvers
  - A better way to do "macros" where we can make context sensitive AST/resolvers.
  - Eventually to the point of invoking custom parsers! (an EDN extension? more powerful version of `#` basically, triple back tick perhaps?)
  - "namespaces" as resolution spaces
- Function dispatch feedback
  - A scaling level of dispatch feedback.
    - At dynamic mode, the dynamic evaluation ensures the final state is correct.
    - At some level of static evaluation, "best partial evaluation possible"
  - Multi methods are overloads are context sensitive functions are templates.
    - (e.g. "ENV/environment style markers on the dispatch")
  - Compile/optimized a function dispatch for performance improvements.
    - Scaling improvements depending on how much information can be guaranteed at call time.
    - function dispatch *is* the compiler.
    - A way to signal a change to dependent objects, so this can be invalidated / recompiled.
- Signal/Condition/Restart
  - Dynamic dispatches as "stack / context" dispatch resolution of functions (as described above), as the signal system
    - Just can resolve to a "throw to point".
    - Some sort of "context" for catch / throw / call-through.
- Declaim / Compiler Args
  - Declaring how strong the compiler will work, or ref-counting vs. GC, or other options at "global" / "thread local" / "call local" levels.
  - These variables are part of the "dispatch environment" so that they can be used to lower how the functions are called.
    - So a compiler is just a `{:env {:compiler [:llvm]}}` which then changes the dispatch of the function to be a compiler internal (e.g. arguments might now be translated to LLVM refs)
    - An optimizer might then be `{:env {:optimize '(> memory speed size)}}` and then arguments would be compile time ones.
- C++ style "full control" object descriptions
  - e.g. including how it operates when assigned, or other operations are done to it.
    - The mechanism for this is by the powerful dispatch system which can reach out of the function to grab things like "variables".
  - but with "prove traits exist".
- Arbitrary precision numbers by semi-default?
- syntax projection…
  - My white whale
  - Ability to fixup `(ref X)` to `#r(X)` to `&X` depending on people's preferences.
    - This follows into full on syntax preferences.
    - Need to write a parser optimizer in code.
  -  Language replacements of symbols.
  - Set a "projection preference" for the code to be stored in.

#### Design Questions

(and attempted answers)

How will dispatch work?

> The first entry in the list being evaluated will be evaluated and resulting entity will be dispatched against.
>
> - A callable entity is one which has a component which is tagged as callable (because it has a component that allows it to be executed, or it may be executed directly as a C function).
>   - A callable entity may have an `ArgumentList` Component which describes the argument list names of it. And an `ArgumentTypeList` which describes the type checks of the list of arguments (and perhaps calling convention if some of them are literals, but we won't support that at first)
> - A dispatching entity will have component relationships which describe all the possible dispatches. From that list a query for the pair `(DispatchesAs, Entity)` will return the entire dispatch table which can then be sorted and called by the evaluator.
>   - e.g. if "Foo" is an entity that is a dispatch. Then each dispatch instance will have a relationship `FooForSomeCase, DispatchesAs, Foo`
>   - Will have to decide on a "most specific" rule.
>   - Support wrapping types? (before, after, around, and keyword mix-in calls?)
>   - `ArgumentTypeList` for these has to be similar to (but maybe not match) the dispatch entity.
