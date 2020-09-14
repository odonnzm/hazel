module Vdom = Virtual_dom.Vdom;
module Dom = Js_of_ocaml.Dom;
module Dom_html = Js_of_ocaml.Dom_html;
module Js = Js_of_ocaml.Js;
module Sexp = Sexplib.Sexp;

open ViewUtil;
open Sexplib.Std;

/** maps key combos to actions contextually, depending on cursor info */
let kc_actions: Hashtbl.t(HazelKeyCombos.t, CursorInfo_common.t => Action.t) =
  [
    (HazelKeyCombos.Backspace, _ => Action.Backspace),
    (Delete, _ => Action.Delete),
    (ShiftTab, _ => Action.MoveToPrevHole),
    (Tab, _ => Action.MoveToNextHole),
    (
      HazelKeyCombos.GT,
      fun
      | {CursorInfo_common.typed: OnType, _} =>
        Action.Construct(SOp(SArrow))
      | _ => Action.Construct(SOp(SGreaterThan)),
    ),
    (Ampersand, _ => Action.Construct(SOp(SAnd))),
    (VBar, _ => Action.Construct(SOp(SOr))),
    (LeftParen, _ => Action.Construct(SParenthesized)),
    (Colon, _ => Action.Construct(SAsc)),
    (Equals, _ => Action.Construct(SOp(SEquals))),
    (Enter, _ => Action.Construct(SLine)),
    (Backslash, _ => Action.Construct(SLam)),
    (Plus, _ => Action.Construct(SOp(SPlus))),
    (Minus, _ => Action.Construct(SOp(SMinus))),
    (Asterisk, _ => Action.Construct(SOp(STimes))),
    (Slash, _ => Action.Construct(SOp(SDivide))),
    (LT, _ => Action.Construct(SOp(SLessThan))),
    (
      Space,
      fun
      | {CursorInfo_common.cursor_term: Line(_, CommentLine(_)), _} =>
        Action.Construct(SChar(" "))
      | _ => Action.Construct(SOp(SSpace)),
    ),
    (Comma, _ => Action.Construct(SOp(SComma))),
    (
      LeftBracket,
      fun
      | {CursorInfo_common.typed: OnType, _} => Action.Construct(SList)
      | _ => Action.Construct(SListNil),
    ),
    (Semicolon, _ => Action.Construct(SOp(SCons))),
    (Alt_L, _ => Action.Construct(SInj(L))),
    (Alt_R, _ => Action.Construct(SInj(R))),
    (Alt_C, _ => Action.Construct(SCase)),
    (Pound, _ => Action.Construct(SCommentLine)),
    (Shift_Enter, _ => Action.Construct(SCommentLine)),
    (Ctrl_Alt_I, _ => Action.SwapUp),
    (Ctrl_Alt_K, _ => Action.SwapDown),
    (Ctrl_Alt_J, _ => Action.SwapLeft),
    (Ctrl_Alt_L, _ => Action.SwapRight),
  ]
  |> List.to_seq
  |> Hashtbl.of_seq;

let focus = () => {
  JSUtil.force_get_elem_by_id("cell")##focus;
};

let view = (~inject, model: Model.t) => {
  TimeUtil.measure_time(
    "Cell.view",
    model.measurements.measurements && model.measurements.cell_view,
    () => {
      open Vdom;
      let program = model |> Model.get_program;
      let measure_program_get_doc =
        model.measurements.measurements && model.measurements.program_get_doc;
      let measure_layoutOfDoc_layout_of_doc =
        model.measurements.measurements
        && model.measurements.layoutOfDoc_layout_of_doc;
      let memoize_doc = model.memoize_doc;
      let code_view = {
        let caret_pos =
          if (Model.is_cell_focused(model)) {
            let (_, ((row, col), _)) =
              Program.get_cursor_map_z(
                ~measure_program_get_doc,
                ~measure_layoutOfDoc_layout_of_doc,
                ~memoize_doc,
                program,
              );
            Some((row, col));
          } else {
            None;
          };

        UHCode.view(
          ~measure=
            model.measurements.measurements && model.measurements.uhcode_view,
          ~inject,
          ~font_metrics=model.font_metrics,
          ~caret_pos,
        );
      };

      /* browser API to prevent event propagation up the DOM */
      let prevent_stop_inject = a =>
        Event.Many([
          Event.Prevent_default,
          Event.Stop_propagation,
          inject(a),
        ]);
      let (key_handlers, code_view) =
        if (Model.is_cell_focused(model)) {
          let key_handlers = [
            Attr.on_keypress(_ => Event.Prevent_default),
            Attr.on_keydown(evt => {
              switch (MoveKey.of_key(Key.get_key(evt))) {
              | Some(move_key) =>
                prevent_stop_inject(ModelAction.MoveAction(Key(move_key)))
              | None =>
                switch (HazelKeyCombos.of_evt(evt)) {
                | Some(Ctrl_Z) =>
                  if (model.is_mac) {
                    Event.Ignore;
                  } else {
                    prevent_stop_inject(ModelAction.Undo);
                  }
                | Some(Meta_Z) =>
                  if (model.is_mac) {
                    prevent_stop_inject(ModelAction.Undo);
                  } else {
                    Event.Ignore;
                  }
                | Some(Ctrl_Shift_Z) =>
                  if (model.is_mac) {
                    Event.Ignore;
                  } else {
                    prevent_stop_inject(ModelAction.Redo);
                  }
                | Some(Meta_Shift_Z) =>
                  if (model.is_mac) {
                    prevent_stop_inject(ModelAction.Redo);
                  } else {
                    Event.Ignore;
                  }
                | Some(kc) =>
                  prevent_stop_inject(
                    ModelAction.EditAction(
                      Hashtbl.find(
                        kc_actions,
                        kc,
                        program |> Program.get_cursor_info,
                      ),
                    ),
                  )
                | None =>
                  switch (HazelKeyCombos.of_evt(evt)) {
                  | Some(Ctrl_Z) => prevent_stop_inject(ModelAction.Undo)
                  | Some(Ctrl_Shift_Z) =>
                    prevent_stop_inject(ModelAction.Redo)
                  | Some(kc) =>
                    prevent_stop_inject(
                      ModelAction.EditAction(
                        Hashtbl.find(
                          kc_actions,
                          kc,
                          program |> Program.get_cursor_info,
                        ),
                      ),
                    )
                  | None =>
                    switch (JSUtil.is_single_key(evt)) {
                    | None => Event.Ignore
                    | Some(single_key) =>
                      prevent_stop_inject(
                        ModelAction.EditAction(
                          Construct(
                            SChar(JSUtil.single_key_string(single_key)),
                          ),
                        ),
                      )
                    }
                  }
                }
              }
            }),
          ];
          let view =
            program
            |> Program.get_decorated_layout(
                 ~measure_program_get_doc,
                 ~measure_layoutOfDoc_layout_of_doc,
                 ~memoize_doc,
               )
            |> code_view;
          (key_handlers, view);
        } else {
          (
            [],
            program
            |> Program.get_layout(
                 ~measure_program_get_doc,
                 ~measure_layoutOfDoc_layout_of_doc,
                 ~memoize_doc,
               )
            |> code_view,
          );
        };
      Node.div(
        [
          Attr.id(cell_id),
          // necessary to make cell focusable
          Attr.create("tabindex", "0"),
          Attr.on_focus(_ => inject(ModelAction.FocusCell)),
          Attr.on_blur(_ => inject(ModelAction.BlurCell)),
          ...key_handlers,
        ],
        [
          /* font-specimen used to gather font metrics for caret positioning and other things */
          Node.div([Attr.id("font-specimen")], [Node.text("X")]),
          Node.div([Attr.id("code-container")], [code_view]),
        ],
      );
    },
  );
};
