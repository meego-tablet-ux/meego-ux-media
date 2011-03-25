/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1

Item {

    property alias model: listView.model
    property alias index: listView.currentIndex
    property variant selectedSong: undefined

    property string labelUnknownArtist: qsTr("unknown artist")
    property string labelUnknownAlbum: qsTr("unknown album")

    SystemPalette { id: activePalette }

    signal listItemSelected (variant itemid, string title)

    // Scrollable list
    Rectangle {
        id: grid
        clip: true

        anchors.fill: parent
        color: "black"

        property Item highlightedItem // keep track of what item is currently highlighted
        property Item selectedItem


        // Delegate
        // The new item isn't selected until a full click is detected.
        // This allows scrolling without selection.
        Component {
            id: songDelegate

            Rectangle {
                width: parent.width
                height: trackTitle.font.pixelSize + 20
                border.width: 1
                clip: true
                color: activePalette.mid

                gradient:  Gradient {
                    GradientStop { position: 0.0; color: activePalette.light }
                    GradientStop { position: 1.0; color: activePalette.dark }
                }

                property string mtitle: title
                property bool mfavorite: favorite
                property string mitemid: itemid
                property int mitemtype: itemtype

                Rectangle {
                    id: rect
                    width: 20 ; height: width;
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin:10
                    color:"transparent"
                    border.color:"gray"
                }
                Text {
                    id: trackTitle
                    text: title
                    height: parent.height
                    width: parent.width/3
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment:Text.AlignLeft
                    anchors.left: rect.right
                    anchors.leftMargin:10
                    elide: Text.ElideRight
                    font.bold: true
                }

                Text {
                    id:trackArtist
                    text: {
                        artist[0] == undefined? labelUnknownArtist:artist[0];
                    }
                    height: parent.height
                    width: parent.width/6
                    anchors.left: trackTitle.right
                    anchors.top:parent.top
                    anchors.leftMargin:15
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment:Text.AlignLeft
                    elide: Text.ElideRight
                }

                Text {
                    id:trackLength
                    text: formatTime(length)
                    height: parent.height
                    width: parent.width/10
                    anchors.left: trackArtist.right
                    anchors.top:parent.top
                    anchors.leftMargin: 15
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment:Text.AlignLeft
                    elide: Text.ElideRight
                }

                Text {
                    id:trackAlbum
                    text: {
                        album == ""?labelUnknownAlbum: album;
                    }
                    height: parent.height
                    width: parent.width/6
                    anchors.left: trackLength.right
                    anchors.right: parent.right
                    anchors.top:parent.top
                    anchors.leftMargin: 10
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment:Text.AlignLeft
                    elide: Text.ElideRight
                }

                MouseArea {
                    anchors.fill:parent
                    onClicked:{
                        listView.currentIndex = index;

                        listItemSelected(itemid, title)
                        selectedSong = title;
                    }
                }
            }
        }

        // Highlighter
        Component {
            id: songHighlight
            Rectangle {
                opacity: .5
                color: "yellow"
                y: listView.currentItem.y
                z:100
                Behavior on y { SmoothedAnimation { velocity: 500 } }
            }
        }
        ListView {
            id: listView
            focus: true
            spacing: 1

            anchors.fill: parent;
            highlightMoveDuration: 300

            highlight: songHighlight

            delegate: songDelegate
        }
    }

    function formatTime(time)
    {
        var min = parseInt(time/60);
        var sec = parseInt(time%60);
        return min+ (sec<10 ? ":0":":") + sec
    }

    function show()
    {
        visible = true
        listView.currentIndex = -1
        selectedSong = undefined
    }

    function hide()
    {
        visible = false
        listView.currentIndex = -1
    }

}
