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
    title: qsTr("Music")
    showsearch: false
    filterModel: []
    applicationPage: landingScreenContent

    orientation: 1
    Component {
        id: landingScreenContent
        ApplicationPage {
            id: landingPage
            anchors.fill: parent
            title: ""
            Rectangle {
                id: main
                anchors.fill: parent
                color: "black"

                Item {
                    id:scene1
                    width: 340
                    height: window.content.height

                    MusicListModel {
                        id: musicByArtist
                        type: MusicListModel.ListofArtists
                        limit: 0
                        sort: MusicListModel.SortByTitle
                    }

                    Rectangle {
                        id: filterbuttons1
                        width: 50
                        height: scene1.height
                        color: "black"
                        Rectangle {
                            id: filterall1
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons1.top
                                anchors.left: filterbuttons1.left
                                text: "All"
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
                                        musicByArtist.filter = 0
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterrv1
                            anchors.top: filterall1.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons1.top
                                anchors.left: filterbuttons1.left
                                text: "Rec View"
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
                                        musicByArtist.filter = 2
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterra1
                            anchors.top: filterrv1.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons1.top
                                anchors.left: filterbuttons1.left
                                text: "Rec Add"
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
                                        musicByArtist.filter = 3
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterfav1
                            anchors.top: filterra1.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons1.top
                                anchors.left: filterbuttons1.left
                                text: "Fav"
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
                                        musicByArtist.filter = 1
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: listarea1
                        anchors.left: filterbuttons1.right
                        width: scene1.width - filterbuttons1.width
                        height: scene1.height
                        color: "black"
                        ListView {
                            anchors.fill: parent
                            spacing: 10
                            model: musicByArtist
                            delegate: Rectangle {
                                id: musicByArtistDelegate
                                width: scene1.width
                                height: colthumb.height
                                color: "transparent"

                                Column {
                                    id: column1
                                    Image {
                                        id: colthumb
                                        source: thumburi
                                        width: 200
                                        height: 200
                                    }
                                    Text {
                                        id: coltitle
                                        text: title
                                        color: "white"
                                        width: colthumb.width
                                        height: colthumb.height
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        font.pixelSize: theme_fontPixelSizeSmall
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                albumsByArtist.artist = title
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 2 - 2
                                        width: 30
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.right
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
                                                musicByArtist.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colthumb.height / 2 - 2
                                        width: 30
                                        anchors.top: colfav1.bottom
                                        anchors.left: colthumb.right
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
                                                musicByArtist.setViewed(itemid)
                                            }
                                        }
                                    }
                                 }
                            }
                        }
                    }
                }

                Item {
                    id: scene2
                    width: 340
                    height: 600
                    anchors.top: scene1.top
                    anchors.left: scene1.right

                    MusicListModel {
                        id: albumsByArtist
                        type: MusicListModel.ListofAlbumsForArtist
                        limit: 0
                        sort: MusicListModel.SortByTitle
                    }

                    Rectangle {
                        id: filterbuttons2
                        width: 50
                        height: scene2.height
                        color: "black"
                        Rectangle {
                            id: filterall2
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "All"
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
                                        albumsByArtist.filter = 0
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterrv2
                            anchors.top: filterall2.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Rec View"
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
                                        albumsByArtist.filter = 2
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterra2
                            anchors.top: filterrv2.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Rec Add"
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
                                        albumsByArtist.filter = 3
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterfav2
                            anchors.top: filterra2.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Fav"
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
                                        albumsByArtist.filter = 1
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: listarea2
                        anchors.left: filterbuttons2.right
                        width: scene2.width - filterbuttons2.width
                        height: scene2.height
                        color: "black"
                        ListView {
                            anchors.fill: parent
                            spacing: 10
                            model: albumsByArtist
                            delegate: Rectangle {
                                width: scene2.width
                                height: colthumb.height
                                color: "transparent"

                                Column {
                                    id: column1
                                    Image {
                                        id: colthumb
                                        source: thumburi
                                        width: 200
                                        height: 200
                                    }
                                    Text {
                                        id: coltitle
                                        text: title
                                        color: "white"
                                        width: colthumb.width
                                        height: colthumb.height
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        font.pixelSize: theme_fontPixelSizeSmall
                                        verticalAlignment: Text.AlignBottom
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if(isvirtual)
                                                {
                                                    musicForArtistOrAlbum.type = MusicListModel.ListofOrphanSongsForArtist;
                                                    musicForArtistOrAlbum.artist = artist[0];
                                                }
                                                else
                                                {
                                                    musicForArtistOrAlbum.type = MusicListModel.ListofSongsForAlbum;
                                                    musicForArtistOrAlbum.album = title;
                                                }
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 2 - 2
                                        width: 30
                                        anchors.top: coltitle.top
                                        anchors.left: coltitle.right
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
                                                albumsByArtist.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colthumb.height / 2 - 2
                                        width: 30
                                        anchors.top: colfav1.bottom
                                        anchors.left: coltitle.right
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
                                                albumsByArtist.setViewed(itemid)
                                            }
                                        }
                                    }
                                 }
                            }
                        }
                    }
                }

                Item {
                    id: scene3
                    width: 340
                    height: 600
                    anchors.top: scene2.top
                    anchors.left: scene2.right

                    MusicListModel {
                        id: musicForArtistOrAlbum
                        limit: 0
                        sort: MusicListModel.SortByDefault
                    }

                    Rectangle {
                        id: filterbuttons3
                        width: 50
                        height: scene2.height
                        color: "black"
                        Rectangle {
                            id: filterall3
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "All"
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
                                        musicForArtistOrAlbum.filter = 0
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterrv3
                            anchors.top: filterall3.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Rec View"
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
                                        musicForArtistOrAlbum.filter = 2
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterra3
                            anchors.top: filterrv3.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Rec Add"
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
                                        musicForArtistOrAlbum.filter = 3
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterfav3
                            anchors.top: filterra3.bottom
                            width: parent.width
                            height: 100
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Fav"
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
                                        musicForArtistOrAlbum.filter = 1
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: listarea3
                        anchors.left: filterbuttons3.right
                        width: scene2.width - filterbuttons3.width
                        height: scene2.height
                        color: "black"
                        ListView {
                            anchors.fill: parent
                            spacing: 10
                            model: musicForArtistOrAlbum
                            delegate: Rectangle {
                                width: scene2.width
                                height: coltitle.height
                                color: "transparent"

                                Column {
                                    id: column1
                                    Image {
                                        id: colthumb
                                        source: thumburi
                                        width: 100
                                        height: 100
                                    }
                                    Text {
                                        id: coltitle
                                        text: title
                                        color: "white"
                                        width: scene3.width - 100
                                        height: colthumb.height
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        font.pixelSize: theme_fontPixelSizeSmall
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 3 - 2
                                        width: 30
                                        anchors.top: colthumb.top
                                        anchors.left: coltitle.right
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
                                                musicForArtistOrAlbum.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colthumb.height / 3 - 2
                                        width: 30
                                        anchors.top: colfav1.bottom
                                        anchors.left: coltitle.right
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
                                                musicForArtistOrAlbum.setViewed(itemid)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colps1
                                        height: colthumb.height / 3 - 2
                                        width: 30
                                        anchors.top: colrv1.bottom
                                        anchors.left: coltitle.right
                                        anchors.leftMargin: 4
                                        anchors.topMargin: 4
                                        color: (playstatus)?"red":"purple"
                                        Text {
                                            text: "PS"
                                            color: (playstatus)?"white":"black"
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
                                                musicForArtistOrAlbum.setPlayStatus(itemid, 2)
                                            }
                                        }
                                    }
                                 }
                            }
                        }
                    }
                }
            }
        }
    }
}
