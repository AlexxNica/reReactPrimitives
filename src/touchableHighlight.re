type pressed =
  | Depressed
  | Idle;

type focus =
  | NotFocused
  | FocusedFromMouse
  | FocusedFromKeyboard;

type state = {
  pressed,
  focus
};

type action =
  | Focus
  | Blur
  | MouseDown
  | MouseUp
  | KeyDown int
  | KeyUp int
  | KeyPress (int, int)
  | Click;

let component = ReasonReact.reducerComponent "TouchableHighlight";

module Styles = {
  let container = ReactDOMRe.Style.make position::"relative" cursor::"pointer" ();
  let contents = ReactDOMRe.Style.make position::"relative" ();
};

let make
    ::onPress
    ::underlayColor="rgba(0, 0, 0, 0.2)"
    ::onKeyPress=?
    ::onKeyDown=?
    ::onKeyUp=?
    ::onFocus=?
    ::onBlur=?
    ::style=?
    ::focusedFromKeyboardStyle=?
    ::focusedFromMouseStyle=?
    ::disabled=false
    ::disabledStyle=?
    children => {
  ...component,
  initialState: fun _ => {pressed: Idle, focus: NotFocused},
  reducer: fun action state =>
    switch action {
    | Focus =>
      disabled ?
        ReasonReact.NoUpdate :
        (
          switch state.focus {
          | FocusedFromMouse => ReasonReact.NoUpdate
          | _ => ReasonReact.Update {...state, focus: FocusedFromKeyboard}
          }
        )
    | Blur => disabled ? ReasonReact.NoUpdate : ReasonReact.Update {...state, focus: NotFocused}
    | MouseDown =>
      disabled ?
        ReasonReact.NoUpdate : ReasonReact.Update {focus: FocusedFromMouse, pressed: Depressed}
    | MouseUp => disabled ? ReasonReact.NoUpdate : ReasonReact.Update {...state, pressed: Idle}
    | KeyDown key =>
      disabled ?
        ReasonReact.NoUpdate :
        (
          switch key {
          | 13
          | 32 => ReasonReact.Update {...state, pressed: Depressed}
          | _ => ReasonReact.NoUpdate
          }
        )
    | KeyUp key =>
      disabled ?
        ReasonReact.NoUpdate :
        (
          switch key {
          | 13
          | 32 => ReasonReact.Update {...state, pressed: Idle}
          | _ => ReasonReact.NoUpdate
          }
        )
    | KeyPress keys =>
      disabled ?
        ReasonReact.NoUpdate :
        (
          switch keys {
          | (13, _)
          | (_, 13)
          | (32, _)
          | (_, 32) =>
            onPress ();
            ReasonReact.NoUpdate
          | _ => ReasonReact.NoUpdate
          }
        )
    | Click =>
      disabled ? ReasonReact.NoUpdate : ReasonReact.SideEffects (fun _ => ignore (onPress ()))
    },
  render: fun {state, reduce} =>
    ReasonReact.cloneElement
      <div
        role="button"
        tabIndex=0
        style=(
          ReactDOMRe.Style.combine
            (
              ReactDOMRe.Style.combine
                (
                  ReactDOMRe.Style.unsafeAddProp
                    (
                      ReactDOMRe.Style.combine
                        (
                          ReactDOMRe.Style.make
                            outline::(
                              switch state.focus {
                              | FocusedFromMouse => "none"
                              | _ => ""
                              }
                            )
                            ()
                        )
                        (
                          switch style {
                          | None => Styles.container
                          | Some style => ReactDOMRe.Style.combine Styles.container style
                          }
                        )
                    )
                    "WebkitTapHighlightColor"
                    "rgba(0, 0, 0, 0)"
                )
                (
                  switch (state.focus, focusedFromKeyboardStyle, focusedFromMouseStyle) {
                  | (FocusedFromKeyboard, Some style, _) => style
                  | (FocusedFromMouse, _, Some style) => style
                  | _ => ReactDOMRe.Style.make ()
                  }
                )
            )
            (
              switch (disabled, disabledStyle) {
              | (true, Some style) => style
              | _ => ReactDOMRe.Style.make ()
              }
            )
        )
        onFocus=(
          fun event => {
            switch onFocus {
            | Some onFocus => onFocus event
            | None => ()
            };
            reduce (fun () => Focus) ()
          }
        )
        onBlur=(
          fun event => {
            switch onBlur {
            | Some onBlur => onBlur event
            | None => ()
            };
            reduce (fun _ => Blur) ()
          }
        )
        onMouseDown=(reduce (fun _ => MouseDown))
        onMouseUp=(reduce (fun _ => MouseUp))
        onTouchStart=(reduce (fun _ => MouseDown))
        onTouchEnd=(reduce (fun _ => MouseUp))
        onKeyDown=(
          fun event => {
            switch onKeyDown {
            | Some onKeyDown => onKeyDown event
            | None => ()
            };
            reduce (fun event => KeyDown (ReactEventRe.Keyboard.keyCode event)) event
          }
        )
        onKeyUp=(
          fun event => {
            switch onKeyUp {
            | Some onKeyUp => onKeyUp event
            | None => ()
            };
            reduce (fun event => KeyUp (ReactEventRe.Keyboard.keyCode event)) event
          }
        )
        onKeyPress=(
          fun event => {
            switch onKeyPress {
            | Some onKeyPress => onKeyPress event
            | None => ()
            };
            let keys = (ReactEventRe.Keyboard.keyCode event, ReactEventRe.Keyboard.charCode event);
            switch keys {
            | (13, _)
            | (_, 13)
            | (32, _)
            | (_, 32) => ReactEventRe.Keyboard.preventDefault event
            | _ => ()
            };
            reduce (fun keys => KeyPress keys) keys
          }
        )
        onClick=(reduce (fun _ => Click))>
        (
          switch state.pressed {
          | Depressed =>
            <div
              style=(
                ReactDOMRe.Style.make
                  position::"absolute"
                  top::"0"
                  left::"0"
                  right::"0"
                  bottom::"0"
                  backgroundColor::underlayColor
                  ()
              )
            />
          | Idle => ReasonReact.nullElement
          }
        )
        <div style=Styles.contents> children.(0) </div>
      </div>
      props::{"aria-disabled": Js.Boolean.to_js_boolean disabled}
      [||]
};
