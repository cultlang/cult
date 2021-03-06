Lets describe the type system.

First we have **Abstract** types, which are the base of the type system, an empty named type. They support abstract inheritance, which is basically a way to tag other types as a "is-a" of the abstract type, to build ontological hierarchies. They also support abstract instantiation, which is a way to tag other types as instances of an abstract (and may be overridden in some cases to prevent the creation of types). Everything described below is represented with abstract types for proper execution.

From here we have the concrete memory types. **Fundamental** types which are fixed number of bits and often follow an encoding of some sort, for example, character, byte, integer, float, pointer, and so on. From there we have **Structure** types which are compositions of fundamentals and structures. While structures support structural inheritance, it will be resolved as if it were a form of implicit field.

Next we have the abstract **Object** types, which are a pseudo meta-object system. Objects can include many features, including: lowering an object to a structure/fundamental to "render" it into memory (with overloads for the context of it's usage), inheritance of behavior via abstract is-a inheritance, allowing for reference/pointer/managed types.

Now we can describe functionality — which are listed as types, all loaded code is stored as types, including temporary functions that are instantiated beyond their scope — starting with **Functions** which are existing executable types with precise type limitations, primarily describing already compiled code (usually external). Executable types can be marked to run at different phases (including parse, resolve, compile, load, and so on) and may be ran as macro-esque forms. Local code is then described with **Methods** which may resolve into multiple functions over their lifetime — or may just always be evaluated dynamically — depending on how they are called, from runtime to resolve time (a macro method would return code, which is an instantiation, and likely not be functiontized, a normal method is likely to be called in a way conducive to optimization).

Most importantly are **Dispatchers** which describe the rules of a dispatch, and track the registered resolutions of the dispatch, whether they be function, method, or further dispatches. **Registrations**, like all types, are tracked to allow hot reloading of the relevant code, and are the binder between subroutines and dispatchers (even if they usually always map directly to subroutine). **PartialDispatches** are then types which are interested in maintaining (or at least querying) a constrained set of a dispatch.

Finally we come full circle and have **Concepts** which are abstract types that participate in the objects system (like everything does) that use partial dispatches and other type trait queries to create, enforce, and make use of conceptual interfaces. Traits that can be checked include the memory size ones, object features, dispatches, and so on. Concepts can then be used to create new dispatch registrations. Concepts can also be asserted to ensure that objects obey them as expected.





### Random Ideas

- Fields are conventionally at `.foo` as a symbol holding the "field dispatcher".
- Visibility can be handled on a per registration basis, at compile/resolve time, allowing complex visibility checks?
- A namespace macro/loading function will be basically *necessary* to use the system out of box. Just to get most of the system pieces to load for the first time.
  - uses a `default` namespace
  - e.g. `system` is a module defined in C++ which invokes the internal lisp registration features to build up the namespace.
    - this allows it to be hot reloaded by just recalling the function once it's loaded.
  - The first file ran will setup a lot of the environment, this will be on purpose.
- Namespaces will have a top level event counter
  - To ensure hot reloads get put in the right spot
    - (hot reloads will also be their own events)
  - (some mechanism to pause a load until another file finishes loading?)
- Methods with an "auto" tag which handles the registration automatically.



### Form List

List of necessary forms:

- `(define name (function &Return [(name &Type)*] &{meta} rest*))` (omitted types are assumed to be "any")
- `(define name (method (&sub-name [bindings*])*))` uses "declaim" syntax to create rules, and has possibility for multiple variations.
- `(define name Dispatch)`