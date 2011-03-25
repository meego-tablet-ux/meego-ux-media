/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1
import MeeGo.Media 0.1

/*
 * Resizeable grid layout
 * The cells in the grid will scale so that the correct number of
 * cells will fit in region.  Columns are added/removed based on
 * keeping the cell size as close to the original (suggested) as possible.
 *
 * Parameters:
 *    widthHint - suggested width/height of each image (note cells are square)
 *    showLabels - if true, displays the video title over the thumbnail
 *
 * Returns:
 *    When "ok" button selected:
 *    videoSelected ( itemid, itemtype, uri, thumburi, title )
 *
 *    When "cancel" button selected:
 *    cancel() - does not invoke videoSelected signal.
 *
 */

Item {
    id: videoPicker

    property alias imageWidthHint: inner.widthHint    // preferred width of images
    property bool showLabels: false
    property alias popupWidth: inner.width
    property alias popupHeight: inner.height
    property string promptText: qsTr("Pick a video")
    property string okLabel: qsTr("OK")
    property string cancelLabel: qsTr("Cancel")
    property string unknownAlbumImage: ""
    property string labelColor: "cyan"

    signal closed
    signal opened
    signal cancel
    signal videoSelected( string itemid, int itemtype, string uri, string thumburi, string title )

    function show() {
        inner.opacity = 1
        gridView.currentIndex = -1
        videoPicker.opened()
    }
    function forceClose() {
        if (inner.opacity != 0) {
            inner.opacity = 0;
            videoPicker.closed();
        }
    }

    anchors.fill:  parent; anchors.centerIn:  parent;

    MouseArea { anchors.fill: parent; onClicked: /* swallow mouse clicks */; }

    Item {
        id: selectedData
        property string itemid
        property int itemtype
        property string uri
        property string thumburi
        property string title
    }

    Rectangle {
        id: inner

        property int widthHint: 160 //120         // preferred width of object
        property int spacing: 5 //3             // spacing between cells
        property real scaleSelected: 0.9 //0.85
        property real scaleUnselected: 1.0 //0.95

        opacity: 0
        visible: opacity > 0
        anchors.fill: parent
        color: "#707070"//"#080808"

        anchors {
            topMargin: parent.height * 0.05
            bottomMargin: parent.height * 0.05
            leftMargin:  parent.width * 0.05
            rightMargin: parent.width * 0.05
        }
        // header
        Item {
            id: header

            anchors { left: parent.left; top: parent.top; right: parent.right; }
            height: 30

            BorderImage {
                id: background

                anchors.fill: parent
                source: "image://theme/titlebar_l"

                Text {
                    id: crumbLabel

                    anchors.fill: parent
                    anchors.leftMargin: 10
                    verticalAlignment: Text.AlignVCenter
                    text: videoPicker.promptText
                    font.bold: true
                    color: "black" //"white"
                    elide: Text.ElideRight
                    font.pixelSize: theme_fontPixelSizeMedium
                }
            }
        } // header

        // Scrollable grid
        Rectangle {
            id: grid
            color: "#707070"

            // find the true cell width based on the parent and the suggested size
            function getCellWidth ( widthHint, parentWidth, spacing ) {
                var g_w = parentWidth;
                var sp = spacing
                var r_w = widthHint

                var c_n = (g_w - sp) / (r_w + sp)
                var r_wt = ( (g_w - sp) / Math.floor(c_n)  ) - sp // upper bounds for the rect size
                var r_wr = ( (g_w - sp) / Math.round(c_n) ) - sp  // lower bounds

                return ( (r_wt - r_w) < (r_w - r_wr) ) ? r_wt : r_wr;
            }

            property Item highlightedItem // keep track of what item is currently highlighted

            property int r_width: getCellWidth(inner.widthHint, parent.width, inner.spacing)

            anchors { left: parent.left; right: parent.right; top: header.bottom; bottom: footerRect.top }

            // Delegate
            // The new item isn't selected until a full click is detected.
            // This allows scrolling without selection.
            Component {
                id: imageDelegate

                Item {
                    id: boundingBox

                    //clip: true
                    width: gridView.cellWidth;
                    height:gridView.cellHeight


                    Image {
                        id: icon

                        property string mthumburi

                        mthumburi:{
                            try {
                                if (thumburi == "" | thumburi == undefined)
                                    return videoPicker.unknownAlbumImage;
                                else
                                    return thumburi;
                            } catch(err) { return videoPicker.unknownAlbumImage }
                        }

                        anchors.fill: parent
                        anchors.margins: inner.spacing

                        source: mthumburi

                        asynchronous: true

                        width: gridView.cellWidth
                        height: gridView.cellWidth
                        scale: inner.scaleUnselected
                        fillMode: Image.PreserveAspectCrop
                        clip: true
                    }

                    Image {
                        id:  nameRect

                        z: 2
                        visible: videoPicker.showLabels
                        opacity: 0.5
                        source: "image://theme/titlebar_l"
                        anchors { left: icon.left; right: icon.right; bottom: icon.bottom; /*rightMargin: 5; leftMargin: 5*/ }
                        // There is a bug in qml when showing several images in a grid or listview. The images can exceed their defined size.
                        anchors.leftMargin: -1; anchors.rightMargin: 1   // This is a brute force work around
                        //anchors.margins: grid.sp
                        height: nameField.font.pixelSize + 4

//                        Rectangle {
//                            anchors.fill: parent
//                            opacity: .4
//                            color: videoPicker.labelColor
//                        }
                    }

                    Text {
                        id: nameField

                        z: 3
                        visible: videoPicker.showLabels
                        anchors.fill: nameRect;
                        anchors.leftMargin: 4
                        anchors.rightMargin: 4
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        text: title
                        color: "black"
                        font.pixelSize: theme_fontPixelSizeMedium
                    }

                    MouseArea {
                        id: iconArea

                        z: 4
                        anchors.fill: parent

                        onPressed: {
                            //parent.opacity = 0.8
                            icon.scale = inner.scaleSelected
                            grid.highlightedItem = parent
                        }
                        onReleased: {
                            //parent.opacity = 1.0
                            icon.scale = inner.scaleUnselected
                        }
                        onClicked: {
                            gridView.currentIndex = index
                            selectedData.itemid = itemid
                            selectedData.itemtype = itemtype
                            selectedData.uri = uri
                            selectedData.thumburi = thumburi
                            selectedData.title = title
                        }
                        onDoubleClicked: {
                            gridView.currentIndex = index
                            selectedData.itemid = itemid
                            selectedData.itemtype = itemtype
                            selectedData.uri = uri
                            selectedData.thumburi = thumburi
                            selectedData.title = title
                            videoSelected(selectedData.itemid, selectedData.itemtype, selectedData.uri,
                                          selectedData.thumburi, selectedData.title )
                            videoPicker.forceClose()
                        }

                        onExited: {
                            icon.scale = inner.scaleUnselected
                        }
                    }
                }
            }

            // Highlighter
            Component {
                id: iconHighlight

                Rectangle {
                    id: highlighterRect

                    border.width: 5
                    border.color:  "yellow"
                    radius:  5;
                    clip: false
                    color: "transparent"
                    visible: (grid.state == "selected")
                }
            }

            GridView {
                id: gridView

                anchors.fill: parent;
                anchors.leftMargin: (inner.spacing - 2); anchors.rightMargin: -2
                cellWidth: ( grid.r_width + inner.spacing ); cellHeight: ( grid.r_width + ( 2 * inner.spacing ) )
                clip: true
                focus: true;
                highlight: iconHighlight

                cacheBuffer: 2500
                pressDelay: 100

                interactive: contentHeight > inner.height

                model: allAlbumsListModel
                delegate: imageDelegate

                // if scrolling is started undo the selection in progress
                onMovementStarted: {
                    //grid.highlightedItem.opacity = 1.0
                    grid.highlightedItem.scale = inner.scaleUnselected
                }
                // Workaround: sometimes selection is not removed on movement start. This resets it for sure
                onMovementEnded: {
                    grid.highlightedItem.scale = inner.scaleUnselected
                }
                onModelChanged: {
                    currentIndex = -1
                }
            }


            state:  "unselected"
            states: [
                State {
                    name: "selected"
                    when: (gridView.currentIndex != -1)
                },
                State {
                    name:  "unselected"
                    when: (gridView.currentIndex == -1)
                }
            ]
        }


        // Footer Buttons:
        Rectangle {
        //BorderImage { //replaced by rectangle right now, since the titlebar image looks terrible with the buttons
            id: footerRect

            color: "#707070"
            //source: "image://theme/titlebar_l"

            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
                top: parent.bottom;
                topMargin: -header.height;
            }

            Button {
                id: okButton;

                title: videoPicker.okLabel;
                enabled: gridView.currentIndex != -1
                active: enabled
                width: (header.width/3); height: header.height;
                anchors { left: parent.left; leftMargin: -1 }
                onClicked: {
                    videoSelected(selectedData.itemid, selectedData.itemtype, selectedData.uri,
                                  selectedData.thumburi, selectedData.title )
                    videoPicker.forceClose()
                }
            }
            Button {
                id: cancelButton

                title: videoPicker.cancelLabel
                enabled: true;
                width: (header.width/3); height: header.height;
                anchors { right: parent.right; rightMargin: -2 }
                onClicked: {
                    videoPicker.cancel()
                    videoPicker.forceClose()
                }
            }
        }

        Behavior on opacity { NumberAnimation { duration: 320 } }
    }//end of inner

    VideoListModel {
        id: allAlbumsListModel
        type: VideoListModel.ListofVideos
        limit: 0
        sort: VideoListModel.SortByTitle
    }

    states: [
        State {
            name: "inactive"
            when: inner.opacity == 0
            PropertyChanges { target: videoPicker; visible: false }
        },
        State {
            name: "active"
            when:  inner.opacity > 0
            PropertyChanges { target: videoPicker; visible: true }
        }
    ]
}
