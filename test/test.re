
open Migrate_parsetree.Ast_403;

let module Convert = Migrate_parsetree.Convert(Migrate_parsetree.OCaml_403)(Migrate_parsetree.OCaml_current);

let show_structure structure => {
  Pprintast.structure Format.str_formatter (Convert.copy_structure structure);
  Format.flush_str_formatter();
};

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
  [%str [%%import (One child, Two (Child (grandchild, grandchild2)))][@@from Three]],
  [%str
    let child = Three.One.child;
    let grandchild1 = Three.Two.Child.grandchild1;
    let grandchild2 = Three.Two.Child.grandchild2;
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

let run () => {
  let (total, failures) = List.fold_left (fun (total, failures) (input, expected) => {
    try {
    let result = Lib.mapper.structure Lib.mapper input;
    if (result != expected) {
      print_endline ">> Input:";
      print_endline (show_structure input);
      print_endline ">> Output:";
      print_endline (show_structure result);
      print_endline ">> Expected:";
      print_endline (show_structure expected);
      (total + 1, failures + 1)
    } else {
      (total + 1, failures)
    }
    } {
      | Location.Error error => {
        print_endline ">> Input:";
        print_endline (show_structure input);
        print_endline ">> Error:";
        print_endline error.Location.msg;
        (total + 1, failures + 1)
      }
    }
  }) (0, 0) fixtures;

  let (total, failures) = List.fold_left (fun (total, failures) bad => {
    try {
      Lib.mapper.structure Lib.mapper bad |> ignore;
      print_endline ">> Input was supposed to throw:";
      print_endline (show_structure bad);
      (total + 1, failures + 1) /* was supposed to raise */
    } {
    | _ => (total + 1, failures)
    } 
  }) (total, failures) invalid;

  if (failures !== 0) {
      Printf.printf "Total: %d, Failures: %d" total failures;
    exit 1;
  } else {
    Printf.printf "All %d succeeded!" total;
    exit 0;
  }
};

run ();
