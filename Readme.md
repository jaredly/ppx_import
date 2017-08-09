# ppx_import - javascript-style imports for reason/ocaml

Now you get to
```
[%%import thing][@@from Source1];
[%%import Sub][@@from Source2];
[%%import (one, Two.Three ((), four, Five.Six))][@@from Source3];
```

which turns into
```
let thing = Source1.thing;
module Sub = Source2.Sub;
let one = Source3.one;
module Three = Source3.Two.Three;
let four = Source3.Two.Three.Three.four;
module Six = Source3.Two.Three.Five.Six;
```

See [test.re](test/test.re) for more examples.