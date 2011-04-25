/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1

Window {
    id: window
    width: 1024
    height: 600
    fullscreen: true
    showtoolbar: false

    MusicListModel {
        id: musicModel
        type: MusicListModel.ListofArtists
        limit: 0
        sort: MusicListModel.SortByTitle
//        mixtypes: MusicListModel.Songs|MusicListModel.Albums|MusicListModel.Artists|MusicListModel.Playlists
    }

    Rectangle {
        id: sortbuttons
        width: 40
        height: window.height
        color: "black"
        Rectangle {
            id: filterall1
            width: parent.width
            height: 50
            color: "blue"
            border.color: "black"
            border.width: 5
            radius: 10
            Text {
                anchors.top: filterbuttons1.top
                anchors.left: filterbuttons1.left
                text: "Title"
                color: "white"
                font.pixelSize: theme_fontPixelSizeSmaller
                font.bold: false
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicModel.sort = MusicListModel.SortByTitle
                    }
                }
            }
        }
        Rectangle {
            id: filterrv1
            anchors.top: filterall1.bottom
            width: parent.width
            height: 50
            color: "blue"
            border.color: "black"
            border.width: 5
            radius: 10
            Text {
                anchors.top: filterbuttons1.top
                anchors.left: filterbuttons1.left
                text: "Viewed"
                color: "white"
                font.pixelSize: theme_fontPixelSizeSmaller
                font.bold: false
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicModel.sort = MusicListModel.SortByAccessTime
                    }
                }
            }
        }
        Rectangle {
            id: filterra1
            anchors.top: filterrv1.bottom
            width: parent.width
            height: 50
            color: "blue"
            border.color: "black"
            border.width: 5
            radius: 10
            Text {
                anchors.top: filterbuttons1.top
                anchors.left: filterbuttons1.left
                text: "Added"
                color: "white"
                font.pixelSize: theme_fontPixelSizeSmaller
                font.bold: false
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicModel.sort = MusicListModel.SortByAddedTime
                    }
                }
            }
        }
        Rectangle {
            id: filterfav1
            anchors.top: filterra1.bottom
            width: parent.width
            height: 50
            color: "blue"
            border.color: "black"
            border.width: 5
            radius: 10
            Text {
                anchors.top: filterbuttons1.top
                anchors.left: filterbuttons1.left
                text: "Favorite"
                color: "white"
                font.pixelSize: theme_fontPixelSizeSmaller
                font.bold: false
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicModel.sort = MusicListModel.SortByFavorite
                    }
                }
            }
        }
    }

    GridView {
        id: view
        anchors.top: sortbuttons.top
        anchors.left: sortbuttons.right
        width: window.width - sortbuttons.width
        height: sortbuttons.height
        cellWidth: 100
        cellHeight: 30
        clip: true
        model: musicModel

        delegate: Item {
            id: dinstance
            width: 100
            height: 30

            Image {
                id: colthumb
                width: 30
                height: 30
                source: thumburi
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicModel.setViewed(itemid);
                    }
                }
            }

            Text {
                id: coltitle
                text: title
                color: "white"
                width: 70
                height: 30
                anchors.top: colthumb.top
                anchors.left: colthumb.right
                font.pixelSize: theme_fontPixelSizeSmallest
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            GridView.onAdd: SequentialAnimation {
                PropertyAction { target: dinstance; property: "scale"; value: 0 }
                NumberAnimation { target: dinstance; property: "scale"; to: 1; duration: 250; easing.type: Easing.InOutQuad }
            }
//            GridView.onRemove: SequentialAnimation {
//                PropertyAction { target: dinstance; property: "GridView.delayRemove"; value: true }
//                NumberAnimation { target: dinstance; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
//                PropertyAction { target: dinstance; property: "GridView.delayRemove"; value: false }
//            }
        }
    }
}
