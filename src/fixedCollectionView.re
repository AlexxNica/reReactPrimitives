module type FixedCollectionViewRowType = {let name: string; type t;};

type column 't = {
  headerLabel: string,
  style: option ReactDOMRe.Style.t,
  renderHeader: column 't => ReasonReact.reactElement,
  renderCell: int => 't => ReasonReact.reactElement
};

module Make (FixedCollectionViewRow: FixedCollectionViewRowType) => {
  type t = FixedCollectionViewRow.t;
  type state = {
    containerHeight: option int,
    scrollTop: int,
    containerRef: ref (option Dom.element)
  };
  type action =
    | MeasureContainerAtNextFrame
    | SetContainerHeight int
    | SetScrollTop (int, int);
  let component =
    ReasonReact.reducerComponent ("FixedCollectionView[" ^ FixedCollectionViewRow.name ^ "]");
  let setContainerRef containerRef {ReasonReact.state: state} =>
    state.containerRef := Js.Null.to_opt containerRef;
  let make
      data::(data: array t)
      ::onEndReached=?
      ::scrollOffset
      ::rowHeight
      ::headerHeight
      ::renderFooter=?
      ::footerHeight=0
      columns::(columns: list (column t))
      _children => {
    let measureContainerAtNextFrame {ReasonReact.state: state, ReasonReact.reduce: reduce} =>
      Bs_webapi.requestAnimationFrame (
        fun _ =>
          switch state.containerRef {
          | {contents: Some container} =>
            reduce
              (fun containerHeight => SetContainerHeight containerHeight)
              (DomRe.Element.clientHeight container - headerHeight)
          | _ => ()
          }
      );
    let setScrollTop (scrollTop, clientHeight) state => {
      switch onEndReached {
      | Some onEndReached =>
        if (Array.length data * rowHeight - (scrollTop + clientHeight) <= scrollOffset) {
          onEndReached ()
        }
      | None => ()
      };
      ReasonReact.Update {...state, scrollTop}
    };
    let renderRow startIndex rowIndex (rowData: t) =>
      <div
        key=(string_of_int (rowIndex + startIndex))
        style=(
          ReactDOMRe.Style.make
            height::(string_of_int rowHeight ^ "px")
            position::"absolute"
            left::"0px"
            right::"0px"
            top::(string_of_int ((rowIndex + startIndex) * rowHeight) ^ "px")
            boxShadow::"0 1px rgba(0, 0, 0, 0.1)"
            display::"flex"
            flexDirection::"row"
            alignItems::"center"
            ()
        )>
        (
          columns
          |> List.mapi (
               fun index column =>
                 <div key=(string_of_int index) style=?column.style>
                   (column.renderCell (startIndex + rowIndex) rowData)
                 </div>
             )
          |> Array.of_list
          |> ReasonReact.arrayToElement
        )
      </div>;
    {
      ...component,
      initialState: fun () => {containerHeight: None, scrollTop: 0, containerRef: ref None},
      reducer: fun action state =>
        switch action {
        | MeasureContainerAtNextFrame => ReasonReact.SideEffects measureContainerAtNextFrame
        | SetContainerHeight containerHeight =>
          ReasonReact.Update {...state, containerHeight: Some containerHeight}
        | SetScrollTop (scrollTop, clientHeight) => setScrollTop (scrollTop, clientHeight) state
        },
      didMount: fun {reduce} => {
        reduce (fun () => MeasureContainerAtNextFrame) ();
        ReasonReact.NoUpdate
      },
      render: fun {state, handle, reduce} =>
        <div
          style=(ReactDOMRe.Style.make flexGrow::"1" width::"100%" ())
          ref=(handle setContainerRef)>
          <div
            style=(
              ReactDOMRe.Style.make
                display::"flex"
                flexDirection::"row"
                alignItems::"center"
                height::(string_of_int headerHeight ^ "px")
                ()
            )>
            (
              columns
              |> List.mapi (
                   fun index column =>
                     <div key=(string_of_int index) style=?column.style>
                       (column.renderHeader column)
                     </div>
                 )
              |> Array.of_list
              |> ReasonReact.arrayToElement
            )
          </div>
          <div
            className="rrp-FixedCollectionViewScrollView"
            style=(
              ReactDOMRe.Style.make
                flexGrow::"1"
                overflow::"auto"
                width::"100%"
                height::(
                  switch state.containerHeight {
                  | None => "auto"
                  | Some containerHeight => string_of_int containerHeight ^ "px"
                  }
                )
                ()
            )
            onScroll=(
              reduce (
                fun event =>
                  SetScrollTop (
                    DomRe.Element.scrollTop (ReactEventRe.UI.target event),
                    DomRe.Element.clientHeight (ReactEventRe.UI.target event)
                  )
              )
            )>
            (
              switch state.containerHeight {
              | None => ReasonReact.nullElement
              | Some containerHeight =>
                <div
                  style=(
                    ReactDOMRe.Style.make
                      height::(string_of_int (Array.length data * rowHeight + footerHeight) ^ "px")
                      position::"relative"
                      ()
                  )>
                  {
                    let startIndex = max ((state.scrollTop - scrollOffset) / rowHeight) 0;
                    let renderableCount =
                      min ((containerHeight + scrollOffset * 2) / rowHeight) (Array.length data);
                    Array.sub
                      data startIndex (min (renderableCount + 1) (Array.length data - startIndex))
                    |> Array.mapi (renderRow startIndex)
                    |> ReasonReact.arrayToElement
                  }
                  (
                    switch renderFooter {
                    | Some renderFooter =>
                      <div
                        style=(
                          ReactDOMRe.Style.make
                            height::(string_of_int footerHeight ^ "px")
                            position::"absolute"
                            left::"0"
                            right::"0"
                            bottom::"0"
                            display::"flex"
                            flexDirection::"row"
                            alignItems::"center"
                            justifyContent::"center"
                            ()
                        )>
                        (renderFooter ())
                      </div>
                    | None => ReasonReact.nullElement
                    }
                  )
                </div>
              }
            )
          </div>
        </div>
    }
  };
};
