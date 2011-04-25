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

    Rectangle {
        id: main
        anchors.fill: parent
        color: "black"

        Item {
            id: list1
            width: 100
            height: 500
            z: 3

            MusicListModel {
                id: musicByArtist
                type: MusicListModel.ListofArtists
                limit: 0
                sort: MusicListModel.SortByTitle
            }

            Rectangle {
                id: listarea1
                width: list1.width
                height: list1.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: musicByArtist
                    delegate: Rectangle {
                        width: list1.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: list1.width
                            height: list1.height
                            Rectangle {
                                id: thumbspace
                                height: 100
                                width: 100
                                color: "blue"
                                Image {
                                    id: thumbnail
                                    source: thumburi
                                    anchors.fill: parent
                                    width: 100
                                    height: 100
                                }
                                MouseArea {
                                    id: mouse
                                    anchors.fill: parent
                                    onClicked: { nowplaying.addItems(itemid) }
                                }
                            }
                            Text {
                                id: itemtitle
                                text: title
                                color: "white"
                                width: 100
                                height: thumbspace.height
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.left
                                font.pixelSize: theme_fontPixelSizeSmall
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: list2
            width: 100
            height: 500
            anchors.top: list1.top
            anchors.left: list1.right
            z: 2

            MusicListModel {
                id: musicByAlbum
                type: MusicListModel.ListofAlbums
                limit: 0
                sort: MusicListModel.SortByTitle
            }

            Rectangle {
                id: listarea2
                width: list2.width
                height: list2.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: musicByAlbum
                    delegate: Rectangle {
                        width: list2.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: list2.width
                            height: list2.height
                            Rectangle {
                                id: thumbspace
                                height: 100
                                width: 100
                                color: "blue"
                                Image {
                                    id: thumbnail
                                    source: thumburi
                                    anchors.fill: parent
                                    width: 100
                                    height: 100
                                }
                                MouseArea {
                                    id: mouse
                                    anchors.fill: parent
                                    onClicked: { nowplaying.addItems(itemid) }
                                }
                            }
                            Text {
                                id: itemtitle
                                text: title
                                color: "white"
                                width: 100
                                height: thumbspace.height
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.left
                                font.pixelSize: theme_fontPixelSizeSmall
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: list3
            width: 100
            height: 500
            anchors.top: list2.top
            anchors.left: list2.right
            z: 1

            MusicListModel {
                id: musicBySong
                type: MusicListModel.ListofSongs
                limit: 0
                sort: MusicListModel.SortByTitle
            }

            Rectangle {
                id: listarea3
                width: list3.width
                height: list3.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: musicBySong
                    delegate: Rectangle {
                        width: list3.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: list3.width
                            height: list3.height
                            Rectangle {
                                id: thumbspace
                                height: 100
                                width: 100
                                color: "blue"
                                Image {
                                    id: thumbnail
                                    source: thumburi
                                    anchors.fill: parent
                                    width: 100
                                    height: 100
                                }
                                MouseArea {
                                    id: mouse
                                    anchors.fill: parent
                                    onClicked: { nowplaying.addItems(itemid) }
                                }
                            }
                            Text {
                                id: itemtitle
                                text: title
                                color: "white"
                                width: 100
                                height: thumbspace.height
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.left
                                font.pixelSize: theme_fontPixelSizeSmall
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: list4
            width: 150
            height: 500
            anchors.top: list3.top
            anchors.left: list3.right
            z: 1

            MusicListModel {
                id: musicByPlaylist
                type: MusicListModel.ListofPlaylists
                limit: 0
                sort: MusicListModel.SortByTitle
            }

            Rectangle {
                id: listarea4
                width: list4.width
                height: list4.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: musicByPlaylist
                    delegate: Rectangle {
                        width: list4.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: list4.width
                            height: list4.height
                            Rectangle {
                                id: thumbspace
                                height: 100
                                width: 150
                                color: "blue"
                                Image {
                                    id: thumbnail
                                    source: thumburi
                                    anchors.fill: parent
                                    width: 150
                                    height: 100
                                }
                                MouseArea {
                                    id: mouse
                                    anchors.fill: parent
                                    onClicked: { nowplaying.addItems(itemid) }
                                }
                            }
                            Text {
                                id: itemtitle
                                text: title
                                color: "white"
                                width: 150
                                height: thumbspace.height
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.left
                                font.pixelSize: theme_fontPixelSizeSmall
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
        Item {
            id: nowplayinglist
            width: 250
            height: 500
            anchors.top: list4.top
            anchors.left: list4.right
            z: 1

            MusicListModel {
                id: nowplaying
                type: MusicListModel.NowPlaying
                limit: 0
                sort: MusicListModel.SortByTitle
            }

            Rectangle {
                id: listareap
                width: nowplayinglist.width
                height: nowplayinglist.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: nowplaying
                    delegate: Rectangle {
                        width: nowplayinglist.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: nowplayinglist.width
                            height: nowplayinglist.height
                            Rectangle {
                                id: thumbspace
                                height: 100
                                width: 100
                                color: "blue"
                                Image {
                                    id: thumbnail
                                    source: thumburi
                                    anchors.fill: parent
                                    width: 100
                                    height: 100
                                }
                            }
                            Text {
                                id: itemtitle
                                text: title
                                color: "white"
                                width: 100
                                height: thumbspace.height
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.right
                                font.pixelSize: theme_fontPixelSizeSmall
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }
    }
}
