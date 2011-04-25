/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Media 0.1

Window {
    id: window
    title: qsTr("Photo")
    showsearch: false
    filterModel: []
    orientation: 1

    MusicListModel {
        id: recentlyplayed
        type: MusicListModel.ListofRecentlyPlayed
        limit: 0
    }

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
                                    onClicked: { musicByArtist.setViewed(itemid); }
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
                                font.pixelSize: 14
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
                                    onClicked: { musicByAlbum.setViewed(itemid) }
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
                                font.pixelSize: 14
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
                                    onClicked: { musicBySong.setViewed(itemid) }
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
                                font.pixelSize: 14
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
            width: 100
            height: 500
            anchors.top: list3.top
            anchors.left: list3.right
            z: 1
            Rectangle {
                id: filterrv1
                width: parent.width
                height: 50
                color: "blue"
                border.color: "black"
                border.width: 5
                radius: 10
                Text {
                    anchors.fill: parent
                    text: "CLEAR"
                    color: "white"
                    font.pixelSize: 20
                    font.bold: false
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            recentlyplayed.clear();
                        }
                    }
                }
            }
        }
        Item {
            id: recents
            width: 250
            height: 500
            anchors.top: list4.top
            anchors.left: list4.right
            z: 1

            Rectangle {
                id: listareap
                width: recents.width
                height: recents.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: recentlyplayed
                    delegate: Rectangle {
                        width: recents.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: recents.width
                            height: recents.height
                            Rectangle {
                                id: thumbspace
                                height: 100
                                width: 100
                                color: "gray"
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
                            text: title + " [" + lastplayedtime + "]"
                                color: "white"
                                width: 100
                                height: thumbspace.height
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.right
                                font.pixelSize: 14
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
