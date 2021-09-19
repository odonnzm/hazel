open Virtual_dom.Vdom;
open Node;
let logo_panel =
  a(
    [Attr.classes(["logo-text"]), Attr.href("https://hazel.org")],
    [text("Hazel")],
  );

let top_bar = (~inject: ModelAction.t => Ui_event.t, ~model: Model.t) => {
  div(
    [Attr.classes(["top-bar"])],
    [
      logo_panel,
      CardsPanel.view(~inject, ~model),
      ActionMenu.view(~inject),
    ],
  );
};

let cell_status_panel = (~settings: Settings.t, ~model: Model.t, ~inject) => {
  let program = Model.get_program(model);
  let selected_instance = Model.get_selected_hole_instance(model);
  let (_, ty, _) = program.edit_state;
  let result =
    settings.evaluation.show_unevaluated_expansion
      ? program |> Program.get_expansion
      : program |> Program.get_result |> Result.get_dhexp;
  div(
    [],
    [
      div(
        [Attr.classes(["cell-status"])],
        [
          div(
            [Attr.classes(["type-indicator"])],
            [
              div(
                [Attr.classes(["type-label"])],
                [text("Result of type: ")],
              ),
              div([Attr.classes(["htype-view"])], [HTypCode.view(ty)]),
            ],
          ),
        ],
      ),
      div(
        [Attr.classes(["result-view"])],
        [
          DHCode.view(
            ~inject,
            ~selected_instance,
            ~settings=settings.evaluation,
            ~width=80,
            ~font_metrics=model.font_metrics,
            result,
          ),
        ],
      ),
    ],
  );
};

let left_sidebar = (~inject: ModelAction.t => Event.t, ~model: Model.t) =>
  Sidebar.left(~inject, ~is_open=model.left_sidebar_open, () =>
    [ActionPanel.view(~inject, model)]
  );

let right_sidebar = (~inject: ModelAction.t => Event.t, ~model: Model.t) => {
  let settings = model.settings;
  let program = Model.get_program(model);
  let selected_instance = Model.get_selected_hole_instance(model);
  Sidebar.right(~inject, ~is_open=model.right_sidebar_open, () =>
    [
      CursorInspector.view(~inject, model),
      ContextInspector.view(
        ~inject,
        ~selected_instance,
        ~settings=settings.evaluation,
        ~font_metrics=model.font_metrics,
        program,
      ),
      UndoHistoryPanel.view(~inject, model),
      SettingsPanel.view(~inject, settings),
    ]
  );
};

let prev_card_button = (~inject, model: Model.t) => {
  let cardstack = model |> Model.get_cardstack;
  let show_prev = Cardstack.has_prev(cardstack) ? [] : [Attr.disabled];
  Node.button(
    [
      Attr.id("cardstack-prev-button"),
      Attr.on_click(_ => inject(ModelAction.PrevCard)),
      ...show_prev,
    ],
    [Node.text("Previous")],
  );
};

let next_card_button = (~inject, model: Model.t) => {
  let cardstack = model |> Model.get_cardstack;
  let show_next = Cardstack.has_next(cardstack) ? [] : [Attr.disabled];
  Node.button(
    [
      Attr.id("cardstack-next-button"),
      Attr.on_click(_ => inject(ModelAction.NextCard)),
      ...show_next,
    ],
    [Node.text("Next")],
  );
};

let cardstack_controls = (~inject, model: Model.t) =>
  Node.div(
    [Attr.id("cardstack-controls")],
    [
      Node.div(
        [Attr.id("button-centering-container")],
        [
          prev_card_button(~inject, model),
          next_card_button(~inject, model),
        ],
      ),
    ],
  );

let cell_controls = (~inject: ModelAction.t => Event.t) =>
  Node.div(
    [Attr.id("cardstack-controls")],
    [
      Node.div(
        [Attr.id("button-centering-container")],
        [
          Node.button(
            [
              Attr.id("add-button"),
              Attr.on_click(_ => inject(ModelAction.EditAction(AddCell))),
            ],
            [Node.text("Add")],
          ),
          Node.button(
            [
              Attr.id("remove-button"),
              Attr.on_click(_ => inject(ModelAction.EditAction(RemoveCell))),
            ],
            [Node.text("Remove")],
          ),
        ],
      ),
    ],
  );

let view = (~inject: ModelAction.t => Event.t, model: Model.t) => {
  let settings = model.settings;
  TimeUtil.measure_time(
    "Page.view",
    settings.performance.measure && settings.performance.page_view,
    () => {
      let card_caption = Model.get_card(model).info.caption;
      let cell_status =
        !settings.evaluation.evaluate
          ? div([], []) : cell_status_panel(~settings, ~model, ~inject);
      div(
        [Attr.id("root")],
        [
          top_bar(~inject, ~model),
          div(
            [Attr.classes(["main-area"])],
            [
              left_sidebar(~inject, ~model),
              div(
                [Attr.classes(["flex-wrapper"])],
                [
                  div(
                    [Attr.id("page-area")],
                    [
                      div(
                        [Attr.classes(["page"])],
                        [
                          div(
                            [Attr.classes(["card-caption"])],
                            [card_caption],
                          ),
                          Cell.view(~inject, model),
                          cell_status,
                          cell_controls(~inject),
                          cardstack_controls(~inject, model),
                        ],
                      ),
                      // Node.button([
                      //   Attr.on_click(_ => {
                      //     let e = program |> Program.get_uhexp;
                      //     JSUtil.log(
                      //       Js.string(Serialization.string_of_exp(e)),
                      //     );
                      //     Event.Ignore;
                      //   }),
                      // ]),
                      div(
                        [
                          Attr.style(
                            Css_gen.(
                              white_space(`Pre) @> font_family(["monospace"])
                            ),
                          ),
                        ],
                        [],
                      ),
                    ],
                  ),
                ],
              ),
              right_sidebar(~inject, ~model),
            ],
          ),
        ],
      );
    },
  );
};
