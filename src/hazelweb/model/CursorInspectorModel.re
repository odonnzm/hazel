type t = {
  visible: bool,
  show_expanded: bool,
  novice_mode: bool,
  strategy_guide: bool,
  strategy_guide_lit: bool,
  strategy_guide_var: bool,
  strategy_guide_fun: bool,
  strategy_guide_branch: bool,
  strategy_guide_new_var: bool,
  strategy_guide_other: bool,
  strategy_guide_val: bool,
  strategy_guide_structure: bool,
  strategy_guide_any: bool,
  strategy_guide_ignore: bool,
};

let init = {
  visible: true,
  show_expanded: false,
  novice_mode: true,
  strategy_guide: false,
  strategy_guide_lit: false,
  strategy_guide_var: false,
  strategy_guide_fun: false,
  strategy_guide_branch: false,
  strategy_guide_new_var: false,
  strategy_guide_other: false,
  strategy_guide_val: false,
  strategy_guide_structure: false,
  strategy_guide_any: false,
  strategy_guide_ignore: false,
};

[@deriving sexp]
type update =
  | Toggle_visible
  | Toggle_show_expanded
  | Toggle_novice_mode
  | Toggle_strategy_guide
  | Toggle_strategy_guide_lit
  | Toggle_strategy_guide_var
  | Toggle_strategy_guide_fun
  | Toggle_strategy_guide_branch
  | Toggle_strategy_guide_new_var
  | Toggle_strategy_guide_other
  | Toggle_strategy_guide_val
  | Toggle_strategy_guide_structure
  | Toggle_strategy_guide_any
  | Toggle_strategy_guide_ignore;

let apply_update = (u: update, settings: t) =>
  switch (u) {
  | Toggle_visible => {...settings, visible: !settings.visible}
  | Toggle_show_expanded => {
      ...settings,
      show_expanded: !settings.show_expanded,
    }
  | Toggle_novice_mode => {...settings, novice_mode: !settings.novice_mode}
  | Toggle_strategy_guide => {
      ...settings,
      strategy_guide: !settings.strategy_guide,
    }
  | Toggle_strategy_guide_lit => {
      ...settings,
      strategy_guide_lit: !settings.strategy_guide_lit,
    }
  | Toggle_strategy_guide_var => {
      ...settings,
      strategy_guide_var: !settings.strategy_guide_var,
    }
  | Toggle_strategy_guide_fun => {
      ...settings,
      strategy_guide_fun: !settings.strategy_guide_fun,
    }
  | Toggle_strategy_guide_branch => {
      ...settings,
      strategy_guide_branch: !settings.strategy_guide_branch,
    }
  | Toggle_strategy_guide_new_var => {
      ...settings,
      strategy_guide_new_var: !settings.strategy_guide_new_var,
    }
  | Toggle_strategy_guide_other => {
      ...settings,
      strategy_guide_other: !settings.strategy_guide_other,
    }
  | Toggle_strategy_guide_val => {
      ...settings,
      strategy_guide_val: !settings.strategy_guide_val,
    }
  | Toggle_strategy_guide_structure => {
      ...settings,
      strategy_guide_structure: !settings.strategy_guide_structure,
    }
  | Toggle_strategy_guide_any => {
      ...settings,
      strategy_guide_any: !settings.strategy_guide_any,
    }
  | Toggle_strategy_guide_ignore => {
      ...settings,
      strategy_guide_ignore: !settings.strategy_guide_ignore,
    }
  };
