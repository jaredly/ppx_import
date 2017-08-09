
open Migrate_parsetree.Ast_403;

let fail loc txt => raise (Location.Error (Location.error ::loc txt));

/*let fail msg => assert false;*/

type importItem =
  | Module string Location.t
  | Value string Location.t;

let getItem {Parsetree.pexp_desc, pexp_loc} => {
  open Parsetree;
  open Longident;
  switch pexp_desc {
  | Pexp_construct {txt: (Lident name)} None => Module name pexp_loc
  | Pexp_ident {txt: (Lident name)} => Value name pexp_loc
  | _ => fail pexp_loc "Invalid import name"
  }
};

let itemToImport source item => {
  open Ast_helper;
  switch item {
  | Module name loc => {
    let full = (Longident.Ldot source name);
    Str.module_ {
      pmb_name: Location.mkloc name loc,
      pmb_expr: {
        pmod_loc: loc,
        pmod_attributes: [],
        pmod_desc: Pmod_ident (Location.mkloc full loc),
      },
      pmb_attributes: [],
      pmb_loc: loc
    }
  }
  | Value name loc => {
    let full = (Longident.Ldot source name);
    switch [%str let [%p Pat.var (Location.mkloc name loc)] = [%e Exp.ident (Location.mkloc full loc)]] {
    | [item] => item
    | _ => assert false
    }
  }
  };
};

let mapper = Parsetree.{
  ...Ast_mapper.default_mapper,

  structure: fun mapper structure => {
    let rec loop items => {
      switch items {
      | [] => []
      | [{pstr_desc: Pstr_extension ({txt: "import"}, contents) attributes, pstr_loc}, ...rest] => {
          let items = switch contents {
          | PStr [{pstr_desc: Pstr_eval expr _}] => switch expr.pexp_desc {
            | Pexp_tuple items => List.map getItem items
            | Pexp_construct {txt: (Lident name), loc} None => [Module name loc]
            | Pexp_ident {txt: (Lident name), loc} => [Value name loc]
            | _ => fail pstr_loc "Invalid import contents"
            }
          | _ => fail pstr_loc "Invalid import contents"
          };
          let source = switch attributes {
            | [({txt: "from"}, contents)] => {
                switch contents {
                | PStr [{pstr_desc: Pstr_eval {pexp_desc: Pexp_construct {txt} None} _}] => txt
                | _ => fail pstr_loc "Invalid @from attribute contents"
                }
            }
            | _ => fail pstr_loc "Missing @from attribute"
          };
          List.append (List.map (itemToImport source) items) (loop rest);
        }
       | [item, ...rest] => [mapper.structure_item mapper item, ...loop rest] 
      }
    };
    loop structure;
  }
};
