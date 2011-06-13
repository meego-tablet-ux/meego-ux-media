/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7

Item {
    id: noContents
    property int noContentSpacing: 10
    property alias notification: notificationContent.children
    property alias help: helpContent.children
    property bool isLandscape: (window.inLandscape || window.inInvertedLandscape)
    property int bottomMargin: 0
    anchors.fill: parent
    //TODO check margins
    anchors.margins: 20
    anchors.bottomMargin: anchors.margins + bottomMargin
    Column {
        id: col
        width: parent.width
        Item {
            width: parent.width - noContentSpacing
            height: noContentSpacing
        }
        Item {
            id: notificationContent
            //TODO check margins
            width: parent.width - 2*noContentSpacing
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Item {
            width: parent.width - noContentSpacing
            height: noContentSpacing
        }
        Loader {
            width: parent.width - 2*noContentSpacing
            anchors.horizontalCenter: parent.horizontalCenter
            sourceComponent: separator
        }
        Item {
            width: parent.width - 2*noContentSpacing
            height: noContentSpacing
        }
        Item {
            id: helpContent
            //TODO check margins
            width: parent.width
            height: noContents.height - 2*noContentSpacing - notificationContent.height
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
    Component {
        id: separator
        Item {
            height: 2
            width: parent.width
            Rectangle {
                id: spaceLineDark
                // TODO: Use defined values from theme
                color: "#cbc9c9"//separatorDarkColor
                opacity: 1.0 //separatorDarkAlpha
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
            }
            Rectangle {
                id: spaceLineLight
                // TODO: Use defined values from theme
                color: "#FFFFFF"//separatorLightColor
                opacity: 1.0 //separatorLightAlpha
                height: 1
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }
    }
}
