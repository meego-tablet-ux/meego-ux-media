/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1

Item {
    id: helpItem
    property alias heading: headingText.text
    property alias text: helpText.text
    property alias image: screenshot.source
    property alias buttonText: button.text
    property bool landscape: true

    signal buttonClicked()

    Grid {
        width: parent.width
        height: parent.height
        columns: landscape ? 1 : 2
        Item {
            id: imageArea
            width: landscape ? parent.width : screenshot.width + 2*noContentSpacing
            height: landscape ? screenshot.height + 2*noContentSpacing : parent.height
            Image {
                id: screenshot
                visible: headingText.text != ""
                anchors.top: parent.top
                anchors.topMargin: 2*noContentSpacing
                anchors.horizontalCenter: parent.horizontalCenter
                width: source != "" ? sourceSize.width : 384
                height: source != "" ? sourceSize.height : 232
                BorderImage {
                    width: parent.width + border.left + border.right
                    height: parent.height + border.top + border.bottom
                    x: -border.left
                    y: -border.top
                    source: "image://themedimage/widgets/apps/media/tile-border-videos"
                    border.top: 6
                    border.bottom: 10
                    border.left: 5
                    border.right: 5
                }
                Button {
                    id: button
                    visible: text != ""
                    active: true
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 2*noContentSpacing
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        helpItem.buttonClicked();
                    }
                }
            }
        }
        Column {
            width: landscape ? parent.width : parent.width - imageArea.width
            Item {
                width: parent.width
                height: 4*noContentSpacing
            }
            Text {
                id: headingText
                width: parent.width - 8*noContentSpacing
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: theme_fontPixelSizeLarge
                wrapMode: Text.WordWrap
                height: paintedHeight + 2*noContentSpacing
                color: theme_fontColorNormal
            }
            Text {
                id: helpText
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 8*noContentSpacing
                font.pixelSize: theme_fontPixelSizeNormal
                wrapMode: Text.WordWrap
                color: theme_fontColorMedium
            }
        }
    }
}
