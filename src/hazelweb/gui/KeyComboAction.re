module Js = Js_of_ocaml.Js;
module Dom_html = Js_of_ocaml.Dom_html;

let get_model_action_from_kc =
    (cursor_info: CursorInfo.t, key_combo: HazelKeyCombos.t)
    : option(ModelAction.t) => {
  let construct = (shape: Action.shape): option(ModelAction.t) =>
    Some(EditAction(Construct(shape)));

  let (cursor_on_type, cursor_on_comment) =
    switch (cursor_info) {
    | {typed: OnType, _} => (true, false)
    | {cursor_term: Line(_, CommentLine(_)), _} => (false, true)
    | _ => (false, false)
    };

  switch (key_combo) {
  | Escape => None
  | Backspace => Some(EditAction(Backspace))
  | Delete => Some(EditAction(Delete))
  | ShiftTab => Some(EditAction(MoveToPrevHole))
  | Tab => Some(EditAction(MoveToNextHole))
  | GT when cursor_on_type => construct(SOp(SArrow))
  | GT => construct(SOp(SGreaterThan))
  | Ampersand => construct(SOp(SAnd))
  | VBar when cursor_on_type => construct(SOp(SVBar))
  | VBar => construct(SOp(SOr))
  | LeftParen => construct(SParenthesized)
  | Colon => construct(SAnn)
  | Equals => construct(SOp(SEquals))
  | Enter => construct(SLine)
  | Shift_Enter => construct(SCommentLine)
  | Backslash => construct(SLam)
  | Plus => construct(SOp(SPlus))
  | Minus => construct(SOp(SMinus))
  | Asterisk => construct(SOp(STimes))
  | Slash => construct(SOp(SDivide))
  | LT => construct(SOp(SLessThan))
  | Space when cursor_on_comment => construct(SChar(" ")) //
  | Space => construct(SOp(SSpace))
  | Comma => construct(SOp(SComma))
  | LeftBracket when cursor_on_type => construct(SList)
  | LeftBracket => construct(SListNil)
  | Semicolon => construct(SOp(SCons))
  | Alt_L => construct(SInj(L))
  | Alt_R => construct(SInj(R))
  | Alt_C => construct(SCase)
  | Pound => construct(SCommentLine)
  | Ctrl_S => Some(SerializeToConsole)
  | Ctrl_Shift_S => Some(SerializeZexpToConsole)
  | CtrlOrCmd_Z => Some(Undo)
  | CtrlOrCmd_Shift_Z => Some(Redo)
  | Up => Some(MoveAction(Key(ArrowUp)))
  | Down => Some(MoveAction(Key(ArrowDown)))
  | Left => Some(MoveAction(Key(ArrowLeft)))
  | Right => Some(MoveAction(Key(ArrowRight)))
  | Home => Some(MoveAction(Key(Home)))
  | End => Some(MoveAction(Key(End)))
  | Alt_Up => Some(EditAction(SwapUp))
  | Alt_Down => Some(EditAction(SwapDown))
  | Alt_Left => Some(EditAction(SwapLeft))
  | Alt_Right => Some(EditAction(SwapRight))
  };
};

let get_model_action =
    // use cursor_info's cursor_term to match with commentline
    // make sure only single character keys/keycombos are "printed"
    (cursor_info: CursorInfo.t, evt: Js.t(Dom_html.keyboardEvent))
    : option(ModelAction.t) => {
  let construct = (shape: Action.shape): option(ModelAction.t) =>
    Some(EditAction(Construct(shape)));

  let key_combo = HazelKeyCombos.of_evt(evt);
  let single_key = JSUtil.is_single_key(evt);

  switch (key_combo, single_key) {
  | (Some(key_combo), _) => get_model_action_from_kc(cursor_info, key_combo)
  | (_, Some(single_key)) =>
    construct(SChar(JSUtil.single_key_string(single_key)))
  | (None, None) => None
  };
};
