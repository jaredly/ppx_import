# ppx_import - javascript-style imports for reason/ocaml

## Why?

This is more concise than doing a ton of lets, and more specific than an `open`.

## What does it look like?

Now you get to
```
[%%import thing][@@from Source1];
[%%import (Sub1, Sub2)][@@from Source2];
[%%import (one, Two.Three ((), four, Five.Six))][@@from Source3];
```

which turns into
```
let thing = Source1.thing;
module Sub1 = Source2.Sub1;
module Sub2 = Source2.Sub2;
let one = Source3.one;
module Three = Source3.Two.Three;
let four = Source3.Two.Three.four;
module Six = Source3.Two.Three.Five.Six;
```

See [test.re](test/test.re) for more examples.