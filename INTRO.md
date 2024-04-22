See also the [BNF description of the grammar](BNF.txt) for my quantum programming language. This [grammar](GRAMMAR.txt) was distilled from the `yacc` source for my code.

In this folder are the source code and Makefile for my quantum compiler. Note that the compiler, while functional, is partially incomplete (specifically, the “include” directive is not yet implemented), and still has some bugs (a few memory leaks, some known, and certainly some as yet undiscovered). While the compiler has been tested on simple test cases, it is not even guaranteed at this point that it is completely functionally and logically correct.

Certain error-checking is still lacking. Here are a couple of things that come to mind that slip through:

```
my_transformation(a, a, a)
{ ... }

Cnot(a, a);

R(3.14, 1.57; a);
```

Also included are some sample programs in the language. They may be compiled into matrices by running:

```
qcc < sampN.Q
```

Note that the four standard quantum gates are hard–coded into the language. Their syntax is as follows:

- C[not] — `Cnot(trace1, trace2);`
- Rotate — `R(angle; trace1);`
- Phase-shift 1 — `P1(angle; trace1);`
- Phase-shift 2 — `P2(angle; trace2);`

where _angle_ is replaced by a suitable real number. Other gates and circuits may be built up out of these using transformation definitions. The transformation named “main” is the one that is used to generate the top–level matrix definition for the given input.

The compiler attempts to provide helpful error messages for most error scenarios. Unfortunately, line numbers are in some cases lacking. One of many potential future developments is to incorporate line numbering information into the parse tree data, to permit the reflection of that information in the parse stage.

_Scott Moonen_

