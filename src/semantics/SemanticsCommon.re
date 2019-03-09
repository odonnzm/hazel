[@deriving sexp]
type in_hole_reason =
  | TypeInconsistent
  | WrongLength;

[@deriving sexp]
type err_status =
  | NotInHole
  | InHole(in_hole_reason, MetaVar.t);

let err_status_to_string =
  fun
  | NotInHole => "NotInHole"
  | InHole(reason, u) => "InHole";

[@deriving sexp]
type var_err_status =
  | NotInVHole
  | InVHole(MetaVar.t);

[@deriving (show({with_path: false}), sexp)]
type inj_side =
  | L
  | R;

let pick_side = (side, l, r) =>
  switch (side) {
  | L => l
  | R => r
  };

[@deriving show({with_path: false})]
type cursor_side =
  | Before
  | After
  | In(int);

exception SkelInconsistentWithOpSeq;
