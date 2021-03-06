let component = ReasonReact.statelessComponent "SegmentedControl";

type segment 'a = (string, 'a);

let make ::segments ::activeSegment ::color ::onSegmentPress _children => {
  ...component,
  render: fun _self =>
    <div
      style=(
        ReactDOMRe.Style.make
          overflow::"hidden"
          borderRadius::"3px"
          border::("1px solid " ^ color)
          display::"flex"
          flexDirection::"row"
          ()
      )>
      (
        segments
        |> List.mapi (
             fun index (title, value) =>
               <TouchableOpacity
                 key=(string_of_int index)
                 style=(
                   ReactDOMRe.Style.combine
                     (
                       ReactDOMRe.Style.make
                         flexGrow::"1"
                         textAlign::"center"
                         padding::"5px"
                         fontSize::"14px"
                         boxShadow::("1px 0 " ^ color)
                         ()
                     )
                     (
                       value == activeSegment ?
                         ReactDOMRe.Style.make backgroundColor::color color::"#fff" () :
                         ReactDOMRe.Style.make ::color ()
                     )
                 )
                 onPress=(fun _ => onSegmentPress value)>
                 (ReasonReact.stringToElement title)
               </TouchableOpacity>
           )
        |> Array.of_list
        |> ReasonReact.arrayToElement
      )
    </div>
};
