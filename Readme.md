Readme.md

```re
/* from test/test.re */

let fixtures = [(/* (input, output) */
  [%str [%%import One][@@from Three]],
  [%str let module One = Three.One]
), (
  [%str [%%import two][@@from Three]],
  [%str let two = Three.two]
), (
  [%str [%%import (One, two)][@@from Three]],
  [%str
    let module One = Three.One;
    let two = Three.two;
  ]
), (
  [%str [%%import (One, one, Two, two)][@@from Three]],
  [%str
    let module One = Three.One;
    let one = Three.one;
    let module Two = Three.Two;
    let two = Three.two;
  ]
), (
  [%str
    [%%import (One, two)][@@from Three];
    let x = 10;
  ],
  [%str
    let module One = Three.One;
    let two = Three.two;
    let x = 10;
  ]
)];
```
