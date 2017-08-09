
open Migrate_parsetree.Ast_403;

let module Convert = Migrate_parsetree.Convert(Migrate_parsetree.OCaml_403)(Migrate_parsetree.OCaml_current);

let fixtures = [(/* (input, output) */
  [%str [%%import One][@@from Three]; ],
  [%str let module One = Three.One; ]
), (
  [%str [%%import two][@@from Three]; ],
  [%str let two = Three.two; ]
), (
  [%str
    [%%import (One, two)][@@from Three];
  ],
  [%str
    let module One = Three.One;
    let two = Three.two;
  ]
), (
  [%str
    [%%import (One, one, Two, two)][@@from Three];
  ],
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

/* Not implemented yet */
let todo = [(
  [%str [%%import One [@as One']][@@from Three]],
  [%str [%%import two [@as two']][@@from Three]],
  [%str [%%import (One [@as One'], two [@as two'])][@@from Three]],
  [%str [%import ExpressionLevel][@@from Somewhere]],
)];

let invalid = [
  [%str [%%import bad + contents][@@from Somewhere]],
  [%str [%%import 1][@@from Somewhere]],
  [%str [%%import][@@from Somewhere]],

  [%str [%%import thing][@@from bad + from]],
  [%str [%%import noFrom]],
];

  let test () => {
    let (total, failures) = List.fold_left (fun (total, failures) (input, expected) => {
      let result = Lib.mapper.structure Lib.mapper input;
      if (result != expected) {
        print_endline ">> Input:";
        Pprintast.structure Format.str_formatter (Convert.copy_structure input);
        print_endline @@ Format.flush_str_formatter();
        print_endline ">> Output:";
        Pprintast.structure Format.str_formatter (Convert.copy_structure result);
        print_endline @@ Format.flush_str_formatter();
        print_endline ">> Expected:";
        Pprintast.structure Format.str_formatter (Convert.copy_structure expected);
        print_endline @@ Format.flush_str_formatter();
        (total + 1, failures + 1)
      } else {
        (total + 1, failures)
      }
    }) (0, 0) fixtures;
    if (failures !== 0) {
      Printf.printf "Total: %d, Failures: %d" total failures;
    exit 1;
  } else {
    Printf.printf "All %d succeeded!" total;
    exit 0;
  }
};

test ();
