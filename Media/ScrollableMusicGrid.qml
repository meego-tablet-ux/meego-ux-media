/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1

/*
 * Resizeable grid layout
 * The cells in the grid will scale so that the correct number of
 * cells will fit in region.  Columns are added/removed based on
 * keeping the cell size as close to the original (suggested) as possible.
 *
 * Required Params:
 *    videoModel - XmListModel providing video "title" and "image" path.
 *    widthHint  - suggested width/height of each image (note cells are square)
 *
 */
Item {

    property alias model: gridView.model
    property alias index: gridView.currentIndex

    property int widthHint: 130                      // preferred width of object
    property int spacing: 4                         // spacing between cells
    property real scaleSelected: 0.80
    property real scaleUnselected: 0.95
    property bool showLabels: true
    property string defaultThumbnail: ""

    SystemPalette { id: activePalette }

    signal gridItemSelected (variant itemid, string title, int itemType)

    // Scrollable grid
    Rectangle {
        id: grid

        anchors.fill: parent
        color: "black"

        property int g_w: parent.width
        property int sp: spacing
        property int r_w: widthHint

        property real c_n  // number of columns - calculated
        property real r_wt  // upper bounds of object width - calculated
        property real r_wr // lower bounds of object width - calculated

        property int r_width // the true width to use for the rectangle

        property Item highlightedItem // keep track of what item is currently highlighted
        property Item selectedItem

        c_n: (g_w - sp) / (r_w + sp)
        r_wt: ( (g_w - sp) / Math.floor(c_n)  ) - sp   // calculate the upper bounds for the rect size
        r_wr: ( (g_w - sp) / Math.round(c_n) ) - sp   // calculate the lower bounds

        r_width: ( (r_wt - r_w) < (r_w - r_wr) ) ? r_wt : r_wr;


        // Delegate
        // The new item isn't selected until a full click is detected.
        // This allows scrolling without selection.
        Component {
            id: imageDelegate

            Rectangle {
                id: boundingBox
                clip: true

                width: icon.width; height:icon.height
                color: "transparent"

                Image {
                    anchors.fill: parent
                    id: icon
                    source:  mthumburi
                    property string mthumburi
                    mthumburi:{
                        try
                        {
                            if (thumburi == "" | thumburi == undefined)
                                return defaultThumbnail;
                            else
                                return thumburi;
                        }
                        catch(err)
                        {
                            return defaultThumbnail
                        }
                    }

                    property int mitemtype
                    mitemtype:{
                        try
                        {
                            return itemtype;
                        }
                        catch(err)
                        {
                            return -1;
                        }
                    }

                    width: gridView.cellWidth - grid.sp
                    height: gridView.cellWidth
                    scale: scaleUnselected

                    property string name: title

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            parent.opacity = 0.8
                            parent.scale = scaleSelected
                            grid.highlightedItem = parent
                        }
                        onReleased: {
                            parent.opacity = 1.0
                            parent.scale = scaleUnselected
                        }
                        onClicked: {
                            gridView.currentIndex = index
                            gridItemSelected(itemid, title, icon.mitemtype)
                        }
                    }
                }
                Image {
                    id:  nameField
                    opacity: .5
                    source: "image://theme/titlebar_l"
                    anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
                    anchors.margins: grid.sp
                    height: 25

                    Rectangle {
                        anchors.fill: parent
                        opacity: .4
                        color: icon.mitemtype == 4 ? "cyan" : "green"
                    }
                }
                Text {
                    anchors.fill: nameField;
                    anchors.verticalCenter: parent.verticalCenter
                    visible: showLabels
                    elide: Text.ElideRight
                    opacity: 1.0

                    text: title
                    color: "black"
                    font.pixelSize: theme_fontPixelSizeMedium
                }
            }

        }

        // Highlighter
        Component {
            id: iconHighlight
            Rectangle {
                border.width: 3
                border.color: (gridView.currentIndex != -1) ? "yellow" : "transparent"
                radius:  2;
                clip: false
                color: "transparent"
            }
        }


        GridView {
            id: gridView

            anchors.fill: parent;
            anchors.leftMargin: (grid.sp - 2); anchors.rightMargin: -2
            cellWidth: ( grid.r_width + grid.sp ); cellHeight: ( grid.r_width + (2*grid.sp) )
            clip: true
            focus: true;

            Component.onCompleted: {  gridView.currentIndex = -1 }

            delegate: imageDelegate
            highlight: iconHighlight

            // if scrolling is started undo the selection in progress
            onMovementStarted: {
                if (grid.highlightedItem != null) {
                    grid.highlightedItem.opacity = 1.0
                    grid.highlightedItem.scale = scaleUnselected
                }
            }
        }

    }


    function show()
    {
        visible = true
        gridView.currentIndex = -1
    }

    function hide()
    {
        visible = false
    }
}
