/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Item{
    id: button
    property string bgSourceUp:""
    property string bgSourceDn:""
    property bool show: true
    width: (show)?(image.width):0
    height: (show)?(image.height):0

    signal clicked()
    clip:true
    Image{
        id: image
        visible: show
        source: bgSourceUp
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
    }

    states: [
        State {
            name:"pressed"
            when:mouseArea.pressed && mouseArea.containsMouse
            PropertyChanges {
                target: image
                source: bgSourceDn
            }
        }
    ]

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            button.clicked()
        }
    }
}
