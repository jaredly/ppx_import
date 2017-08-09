
open Migrate_parsetree.Ast_403;

let fail loc txt => raise (Location.Error (Location.error ::loc txt));

/*let fail msg => assert false;*/

type importItem =
  | Module Longident.t Location.t
  | Value Longident.t Location.t;

let itemToImport item => {
  open Ast_helper;
  switch item {
  | Module full loc => {
    Str.module_ {
      pmb_name: Location.mkloc (Longident.last full) loc,
      pmb_expr: {
        pmod_loc: loc,
        pmod_attributes: [],
        pmod_desc: Pmod_ident (Location.mkloc full loc),
      },
      pmb_attributes: [],
      pmb_loc: loc
    }
  }
  | Value full loc => {
    switch [%str let [%p Pat.var (Location.mkloc (Longident.last full) loc)] = [%e Exp.ident (Location.mkloc full loc)]] {
    | [item] => item
    | _ => assert false
    }
  }
  };
};

let itemToInlineImport item next => {
  open Ast_helper;
  switch item {
  | Module full loc => {
    Exp.letmodule ::loc (Location.mkloc (Longident.last full) loc) {
      pmod_loc: loc,
      pmod_attributes: [],
      pmod_desc: Pmod_ident (Location.mkloc full loc),
    } next
  }
  | Value full loc => {
    Exp.let_ ::loc Nonrecursive [(Vb.mk
      (Pat.var (Location.mkloc (Longident.last full) loc))
      (Exp.ident (Location.mkloc full loc))
    )] next
    /* switch [%str let [%p Pat.var (Location.mkloc (Longident.last full) loc)] = [%e Exp.ident (Location.mkloc full loc)]] {
    | [item] => item
    | _ => assert false
    } */
  }
  };
};

let rec join one two => {
  open Longident;
  switch two {
  | Lident "()" => one
  | Lident name => Ldot one name
  | Ldot parent name => Ldot (join one parent) name
  | Lapply one two => assert false
  }
};

let rec process namespace expr => {
  open Parsetree;
  switch expr.Parsetree.pexp_desc {
  | Pexp_tuple items => List.map (process namespace) items |> List.concat
  | Pexp_construct {txt, loc} None => [Module (join namespace txt) loc]
  | Pexp_construct {txt, loc} (Some child) => process (join namespace txt) child
  | Pexp_ident {txt, loc} => [Value (join namespace txt) loc]
  | _ => fail expr.Parsetree.pexp_loc "Invalid import name"
  }
};

let getSourceFromAttributes attributes pstr_loc => Location.(Parsetree.(switch attributes {
  | [({txt: "from"}, contents)] => {
      switch contents {
      | PStr [{pstr_desc: Pstr_eval {pexp_desc: Pexp_construct {txt} None} _}] => txt
      | _ => fail pstr_loc "Invalid @from attribute contents"
      }
  }
  | _ => fail pstr_loc "Missing @from attribute"
}));

let mapper = Parsetree.{
  ...Ast_mapper.default_mapper,

  structure: fun mapper structure => {
    let rec loop items => {
      switch items {
      | [] => []
      | [{pstr_desc: Pstr_extension ({txt: "import"}, contents) attributes, pstr_loc}, ...rest] => {
          let source = getSourceFromAttributes attributes pstr_loc;
          let items = switch contents {
          | PStr [{pstr_desc: Pstr_eval expr _}] => process source expr
          | _ => fail pstr_loc "Invalid import contents"
          };
          List.append (List.map itemToImport  items) (loop rest);
        }
        | [item, ...rest] => [mapper.structure_item mapper item, ...loop rest]
      }
    };
    loop structure;
  },

  expr: fun mapper expr => {
    switch expr.pexp_desc {
    | Pexp_sequence {pexp_desc: Pexp_extension ({txt: "import"}, contents) , pexp_attributes, pexp_loc} second => {
      let source = getSourceFromAttributes pexp_attributes pexp_loc;
      let items = switch contents {
      | PStr [{pstr_desc: Pstr_eval expr _}] => process source expr
      | _ => fail pexp_loc "Invalid import contents"
      };
      List.fold_right
      (fun item rest => itemToInlineImport item rest)
      items
      second
      /* List.append (List.map itemToImport items) (loop rest); */
    }
    | _ => expr
    }
  },
};
