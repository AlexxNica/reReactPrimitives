type state = option LayerManager.layer;

let component = ReasonReact.statefulComponent "Tooltip";

module TooltipLayerManager = LayerManager.Make LayerManager.DefaultImpl;

let emptyStyle = ReactDOMRe.Style.make ();

let getArrowStyle (alignTo: LayerManager.align) =>
  switch alignTo {
  | TopLeft
  | Top =>
    ReactDOMRe.Style.make
      position::"absolute"
      width::"0"
      height::"0"
      border::"5px solid transparent"
      borderTopColor::"#000"
      borderBottomWidth::"0"
      left::"5px"
      top::"100%"
      ()
  | TopRight =>
    ReactDOMRe.Style.make
      position::"absolute"
      width::"0"
      height::"0"
      border::"5px solid transparent"
      borderTopColor::"#000"
      borderBottomWidth::"0"
      right::"5px"
      top::"100%"
      ()
  | Right =>
    ReactDOMRe.Style.make
      position::"absolute"
      width::"0"
      height::"0"
      border::"5px solid transparent"
      borderRightColor::"#000"
      borderLeftWidth::"0"
      right::"100%"
      top::"5px"
      ()
  | Bottom
  | BottomLeft =>
    ReactDOMRe.Style.make
      position::"absolute"
      width::"0"
      height::"0"
      border::"5px solid transparent"
      borderBottomColor::"#000"
      borderTopWidth::"0"
      left::"5px"
      bottom::"100%"
      ()
  | BottomRight =>
    ReactDOMRe.Style.make
      position::"absolute"
      width::"0"
      height::"0"
      border::"5px solid transparent"
      borderBottomColor::"#000"
      borderTopWidth::"0"
      right::"5px"
      bottom::"100%"
      ()
  | Left =>
    ReactDOMRe.Style.make
      position::"absolute"
      width::"0"
      height::"0"
      border::"5px solid transparent"
      borderLeftColor::"#000"
      borderRightWidth::"0"
      left::"100%"
      top::"5px"
      ()
  };

let make ::style=emptyStyle ::message ::alignTo children => {
  let showTooltip event _state _self => {
    let layer =
      TooltipLayerManager.make (Contextualized (ReactEventRe.Mouse.target event) alignTo);
    TooltipLayerManager.render
      layer
      <div
        style=(
          ReactDOMRe.Style.make
            borderRadius::"2px"
            backgroundColor::"#000"
            position::"relative"
            color::"#fff"
            padding::"5px"
            fontSize::"12px"
            margin::(
              switch alignTo {
              | Left
              | Right => "0 10px"
              | _ => "10px 0"
              }
            )
            ()
        )>
        <div style=(getArrowStyle alignTo) />
        (ReasonReact.stringToElement message)
      </div>;
    ReasonReact.SilentUpdate (Some layer)
  };
  let hideTooltip _event state _self =>
    switch state {
    | Some layer =>
      TooltipLayerManager.remove layer;
      ReasonReact.SilentUpdate None
    | None => ReasonReact.NoUpdate
    };
  {
    ...component,
    initialState: fun () => None,
    render: fun _state self =>
      <div style onMouseEnter=(self.update showTooltip) onMouseLeave=(self.update hideTooltip)>
        (ReasonReact.arrayToElement children)
      </div>
  }
};