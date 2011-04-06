/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1
import "../"

Rectangle {
    width: 700
    height: 700

    SystemPalette { id: activePalette }
    color: activePalette.dark

    MouseArea {
        anchors.fill: parent
        onClicked: modelVideoPicker.show()
    }


    VideoPicker {
        id: modelVideoPicker

//        imageWidthHint: 90
//        popupWidth: 300
//        popupHeight: 440
//        promptText: "Pick something"
//        showLabels:  true

        onVideoSelected: {
            console.debug("Video selected:"
                          +"\n  itemid = "+itemid
                          +"\n  itemtype = "+itemtype
                          +"\n  uri = "+uri
                          +"\n  thumburi = "+thumburi
                          +"\n  title = "+title )
        }
        onCancel: console.debug("Canceled")
        onClosed: console.debug("Closed")
    }
}
