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

    Component {
        id: highlighter
        Rectangle {
            color: "#281832"
        }
    }

    Component {
        id: highlighteroff
        Rectangle {
            color: "transparent"
        }
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
                                    onClicked: { musicPlaylist.addItems(itemid) }
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
                                    onClicked: { musicPlaylist.addItems(itemid) }
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
                                    onClicked: { musicPlaylist.addItems(itemid) }
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
            width: 180
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
                    id: playList
                    anchors.fill: parent
                    spacing: 10
                    model: musicByPlaylist
                    highlight: highlighteroff
                    highlightMoveDuration: 1
                    delegate: Rectangle {
                        id: playlistDelegate
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
                                color: "transparent"
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
                                    onClicked: {
                                        musicPlaylist.playlist = title;
                                        playList.currentIndex = index;
                                        playList.highlight = highlighter;
                                    }
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
                            Rectangle {
                                id: colfav1
                                height: thumbspace.height / 2 - 2
                                width: 30
                                anchors.top: thumbspace.top
                                anchors.left: thumbspace.right
                                anchors.leftMargin: 4
                                color: (favorite)?"red":"purple"
                                Text {
                                    text: "F"
                                    color: (favorite)?"white":"black"
                                    width: parent.width
                                    height: parent.height
                                    font.pixelSize: theme_fontPixelSizeMedium
                                    font.bold: true
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    wrapMode: Text.WordWrap
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        musicByAlbum.setFavorite(itemid, !favorite)
                                    }
                                }
                            }
                            Rectangle {
                                id: colrv1
                                height: thumbspace.height / 2 - 2
                                width: 30
                                anchors.top: colfav1.bottom
                                anchors.left: thumbspace.right
                                anchors.leftMargin: 4
                                anchors.topMargin: 4
                                color: (recentlyviewed)?"red":"purple"
                                Text {
                                    text: "RV"
                                    color: (recentlyviewed)?"white":"black"
                                    width: parent.width
                                    height: parent.height
                                    font.pixelSize: theme_fontPixelSizeMedium
                                    font.bold: true
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    wrapMode: Text.WordWrap
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        musicByAlbum.setViewed(itemid)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: playlistbuttons
            width: 50
            height: 500
            anchors.top: list4.top
            anchors.left: list4.right
            color: "black"
            Rectangle {
                id: save
                width: parent.width
                height: 100
                color: "blue"
                border.color: "black"
                border.width: 5
                radius: 10
                Text {
                    text: "Save"
                    color: "white"
                    font.pixelSize: theme_fontPixelSizeMedium
                    font.bold: true
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            musicPlaylist.savePlaylist(musicPlaylist.playlist)
                        }
                    }
                }
            }
            Rectangle {
                id: clear
                width: parent.width
                height: 100
                anchors.top: save.bottom
                anchors.left: playlistbuttons.left
                color: "blue"
                border.color: "black"
                border.width: 5
                radius: 10
                Text {
                    text: "Clear"
                    color: "white"
                    font.pixelSize: theme_fontPixelSizeMedium
                    font.bold: true
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            musicPlaylist.clear();
                        }
                    }
                }
            }
            Rectangle {
                id: deletelist
                width: parent.width
                height: 100
                anchors.top: clear.bottom
                anchors.left: playlistbuttons.left
                color: "blue"
                border.color: "black"
                border.width: 5
                radius: 10
                Text {
                    text: "Delete"
                    color: "white"
                    font.pixelSize: theme_fontPixelSizeMedium
                    font.bold: true
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            musicPlaylist.type = MusicListModel.MusicPlaylist;
                            if(playList.highlight == highlighter)
                            {
                                playList.highlight = highlighteroff;
                                musicByPlaylist.destroyItem(playList.currentIndex);
                                musicPlaylist.playlist = "";
                            }
                        }
                    }
                }
            }
            Rectangle {
                id: newlist
                width: parent.width
                height: 100
                anchors.top: deletelist.bottom
                anchors.left: playlistbuttons.left
                color: "blue"
                border.color: "black"
                border.width: 5
                radius: 10
                Text {
                    text: "New"
                    color: "white"
                    font.pixelSize: theme_fontPixelSizeMedium
                    font.bold: true
                    width: parent.width
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            musicPlaylist.playlist = "";
                            playList.highlight = highlighteroff;
                        }
                    }
                }
            }
        }
        Item {
            id: playlistgen
            width: 250
            height: 500
            anchors.top: playlistbuttons.top
            anchors.left: playlistbuttons.right
            z: 1

            MusicListModel {
                id: musicPlaylist
                type: MusicListModel.MusicPlaylist
                limit: 0
            }

            Rectangle {
                id: listareap
                width: playlistgen.width
                height: playlistgen.height
                color: "black"
                ListView {
                    anchors.fill: parent
                    spacing: 10
                    model: musicPlaylist
                    delegate: Rectangle {
                        width: playlistgen.width
                        height: thumbnail.height
                        color: "transparent"

                        Column {
                            id: column1
                            width: playlistgen.width
                            height: playlistgen.height
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
                                MouseArea {
                                    id: mouse
                                    anchors.fill: parent
                                    onClicked: { musicPlaylist.removeIndex(index) }
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
