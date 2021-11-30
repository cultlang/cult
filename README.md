(suggested by github: "how about `improved-meme`")

Another attempt at cultlang. But this time as a more minimalist attempt, focused around doing Advent of Code stuff and things.

#### Ideas

- Using an ECS for the type system, rather than our custom graph system. (flecs)
- Supporting exactly EDN
- Better AST as type-traits design from the beginning.
- Simpler module/namespace/symbol/loading system.
- No compiler till much later.
- Simpler evaluator from the beginning.
- Better baked in data structures.
- Thread/Execution stack first design (no globals or thread locals, passing the stack context as a variable)

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
