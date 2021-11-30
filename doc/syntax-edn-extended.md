The EDN syntax

https://github.com/edn-format/edn

But with some extensions.

- Need a way to specify attributes (ala XML) in a rest form.

  - As an example:
  
    ```lisp
    (while condition
        (+what-goes-here+ :the-loop)
        (set a 15))
    ```
  
  - A slash, like a namespacing operation for the top symbol `while/label` (this one is used in code examples)
  
  - A dot, like a field access for the top symbol `while.label`
  
  - A special syntax `^.label` "meta data, subfield", resolving to "current expression subfield"
  
- Need a way to expand syntax from a parent form

  - As an example:

    ```lisp
    (while condition
        (cond
            ((> x 5) (set x (+ x 1)))
            +what-goes-here+))
    ```

  - Same options as above really

    - a slash `(while/break)`
    - a dot `(while.break)`
    - a metadata? `(^.break)`
    - maybe a dispatch would be better? `(#^/break)` as "dispatch, to the meta object, access break"

- Need a way to describe blocks of other syntax parsing.

  - As an example:
    ```lisp
    (do
        (set a 10)
        +what-goes-here+ "SELECT * FROM DB WHERE A=$a"
        (set a 15))
    ```
    
  - I like, in theory
    ```lisp
    (do
        (set a 10)
        ``sql ; probably 3 actually
        SELECT * FROM DB WHERE A=$a"
        ``
        (set a 15))
    ```
    
  - I also want a math syntax
  
    - one of?
  
      ````
      `m x := x + 5`
      `= x := x + 5`
      `# x := x + 5`
      `math x := x + 5`
      `$ x := x + 5`
      ````

