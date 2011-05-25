import Qt 4.7
import MeeGo.Components 0.1

/* MeeGo-Ux-Components tutorial - step 6 */
Window {
    id: window

    toolBarTitle: "My Window"

    bookMenuModel:[ qsTr("Book1") ]
    bookMenuPayload: [ page1Component ]

    Component { id: page1Component; Page1{} }

    Component.onCompleted: {
        console.log("Window loaded")
        switchBook( page1Component )
    }
}
