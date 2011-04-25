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
    title: qsTr("Photo")
    showsearch: false
    filterModel: []
    applicationPage: landingScreenContent

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

                property variant activemodel: scene1.themodel
                Item {
                    id:scene1
                    width: 250
                    height: parent.height

                    property variant themodel: allPhotosListModel
                    PhotoListModel {
                        id: allPhotosListModel
                        type: PhotoListModel.ListofPhotos
                        limit: 0
                        sort: PhotoListModel.SortByCreationTime
                    }

                    Rectangle {
                        id: filterbuttons1
                        width: 40
                        height: scene1.height
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
                                text: "Show All"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allPhotosListModel.filter = 0
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
                                text: "Recently Viewed"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allPhotosListModel.filter = 2
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
                                text: "Recently Added"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allPhotosListModel.filter = 3
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
                                text: "Favorites"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allPhotosListModel.filter = 1
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
                            model: allPhotosListModel
                            delegate: Rectangle {
                                width: scene1.width
                                height: colthumb.height
                                color: "transparent"

                                Column {
                                    id: column1
                                    Image {
                                        id: colthumb
                                        source: thumburi
                                        opacity: (itemtype == 0)
                                        width: 100
                                        height: 100
                                    }
                                    Text {
                                        text: title + "\n" + ((creationtime == "")?addedtime:creationtime)
                                        color: "white"
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        width: colthumb.width
                                        height: colthumb.height
                                        font.pixelSize: 20
                                        font.bold: false
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 3 - 2
                                        width: 50
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.right
                                        //anchors.leftMargin: 4
                                        color: (favorite)?"red":"purple"
                                        Text {
                                            text: "Favorite"
                                            color: (favorite)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                allPhotosListModel.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colfav1.height
                                        width: colfav1.width
                                        anchors.top: colfav1.top
                                        anchors.left: colfav1.right
                                        anchors.leftMargin: 3
                                        //anchors.topMargin: 4
                                        color: (recentlyviewed)?"red":"purple"
                                        Text {
                                            text: "Set Title"
                                            color: (recentlyviewed)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                allPhotosListModel.changeTitle(uri, "Crazy Photo")
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: coladd1
                                        height: colthumb.height / 3 - 2
                                        width: 50
                                        anchors.top: colfav1.bottom
                                        anchors.left: colthumb.right
                                        anchors.topMargin: 3
                                        color: "purple"
                                        Text {
                                            text: "Add to Album"
                                            color: "black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                singleAlbumListModel.addItems(itemid);
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrem1
                                        height: coladd1.height
                                        width: coladd1.width
                                        anchors.top: coladd1.top
                                        anchors.left: coladd1.right
                                        anchors.leftMargin: 3
                                        //anchors.topMargin: 4
                                        color: "purple"
                                        Text {
                                            text: "Remove from Album"
                                            color: "black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                singleAlbumListModel.removeItems(itemid)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: collim1
                                        height: colthumb.height / 3 - 2
                                        width: 50
                                        anchors.top: coladd1.bottom
                                        anchors.left: colthumb.right
                                        anchors.topMargin: 3
                                        color: (allPhotosListModel.limit == index + 1)?"red":"purple"
                                        Text {
                                            text: "Limit to Here"
                                            color: (allPhotosListModel.limit == index + 1)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if(allPhotosListModel.limit == 0)
                                                    allPhotosListModel.limit = index + 1;
                                                else
                                                    allPhotosListModel.limit = 0;
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: coldel1
                                        height: collim1.height
                                        width: collim1.width
                                        anchors.top: collim1.top
                                        anchors.left: collim1.right
                                        anchors.leftMargin: 3
                                        //anchors.topMargin: 4
                                        color: "purple"
                                        Text {
                                            text: "Delete Completely"
                                            color: "black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                allPhotosListModel.destroyItemByID(itemid)
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
                    width: 250
                    height: parent.height
                    anchors.top: scene1.top
                    anchors.left: scene1.right

                    PhotoListModel {
                        id: allAlbumsListModel
                        type: PhotoListModel.ListofAlbums
                        limit: 0
                        sort: PhotoListModel.SortByCreationTime
                    }

                    Rectangle {
                        id: filterbuttons2
                        width: 40
                        height: scene2.height
                        color: "black"
                        Rectangle {
                            id: filterall2
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Show All"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allAlbumsListModel.filter = 0
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterrv2
                            anchors.top: filterall2.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Recently Viewed"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allAlbumsListModel.filter = 2
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterra2
                            anchors.top: filterrv2.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Recently Added"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allAlbumsListModel.filter = 3
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterfav2
                            anchors.top: filterra2.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons2.top
                                anchors.left: filterbuttons2.left
                                text: "Favorites"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        allAlbumsListModel.filter = 1
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
                            model: allAlbumsListModel
                            delegate: Rectangle {
                                width: scene2.width
                                height: colthumb.height
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
                                        text: title
                                        color: "white"
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        width: colthumb.width
                                        height: colthumb.height
                                        font.pixelSize: 20
                                        font.bold: false
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                singleAlbumListModel.album = title
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 2 - 2
                                        width: 50
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.right
                                        //anchors.leftMargin: 4
                                        color: (favorite)?"red":"purple"
                                        Text {
                                            text: "Favorite"
                                            color: (favorite)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                allAlbumsListModel.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colfav1.height
                                        width: colfav1.width
                                        anchors.top: colfav1.top
                                        anchors.left: colfav1.right
                                        anchors.leftMargin: 4
                                        //anchors.topMargin: 4
                                        color: (recentlyviewed)?"red":"purple"
                                        Text {
                                            text: "Set Viewed"
                                            color: (recentlyviewed)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                allAlbumsListModel.setViewed(itemid)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: collim1
                                        height: colthumb.height / 2 - 2
                                        width: 50
                                        anchors.top: colfav1.bottom
                                        anchors.left: colthumb.right
                                        anchors.topMargin: 4
                                        color: (allAlbumsListModel.limit == index + 1)?"red":"purple"
                                        Text {
                                            text: "Limit to Here"
                                            color: (allAlbumsListModel.limit == index + 1)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if(allAlbumsListModel.limit == 0)
                                                    allAlbumsListModel.limit = index + 1;
                                                else
                                                    allAlbumsListModel.limit = 0;
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: coldel1
                                        height: collim1.height
                                        width: collim1.width
                                        anchors.top: collim1.top
                                        anchors.left: collim1.right
                                        anchors.leftMargin: 4
                                        //anchors.topMargin: 4
                                        color: "purple"
                                        Text {
                                            text: "Delete Item"
                                            color: "black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                allAlbumsListModel.destroyItemByID(itemid)
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
                    width: 250
                    height: parent.height
                    anchors.top: scene2.top
                    anchors.left: scene2.right

                    PhotoListModel {
                        id: singleAlbumListModel
                        type: PhotoListModel.PhotoAlbum
                        limit: 0
                        sort: PhotoListModel.SortByCreationTime
                    }

                    Rectangle {
                        id: filterbuttons3
                        width: 40
                        height: scene2.height
                        color: "black"
                        Rectangle {
                            id: filterall3
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Show All"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        singleAlbumListModel.filter = 0
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterrv3
                            anchors.top: filterall3.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Recently Viewed"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        singleAlbumListModel.filter = 2
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterra3
                            anchors.top: filterrv3.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Recently Added"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        singleAlbumListModel.filter = 3
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterfav3
                            anchors.top: filterra3.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "Favorites"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        singleAlbumListModel.filter = 1
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: new3
                            anchors.top: filterfav3.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons3.top
                                anchors.left: filterbuttons3.left
                                text: "New Album"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        singleAlbumListModel.album = ""
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
                            model: singleAlbumListModel
                            delegate: Rectangle {
                                width: scene2.width
                                height: colthumb.height
                                color: "transparent"

                                Column {
                                    id: column1
                                    Image {
                                        id: colthumb
                                        source: thumburi
                                        opacity: (itemtype == 0)
                                        width: 100
                                        height: 100
                                    }
                                    Text {
                                        text: title
                                        color: "white"
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        width: colthumb.width
                                        height: colthumb.height
                                        font.pixelSize: 20
                                        font.bold: false
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                        opacity: (itemtype == 1)
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 2 - 2
                                        width: 50
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.right
                                        //anchors.leftMargin: 4
                                        color: (favorite)?"red":"purple"
                                        Text {
                                            text: "Favorite"
                                            color: (favorite)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                singleAlbumListModel.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colfav1.height
                                        width: colfav1.width
                                        anchors.top: colfav1.top
                                        anchors.left: colfav1.right
                                        anchors.leftMargin: 4
                                        //anchors.topMargin: 4
                                        color: (recentlyviewed)?"red":"purple"
                                        Text {
                                            text: "Set Viewed"
                                            color: (recentlyviewed)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                singleAlbumListModel.setViewed(itemid)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: collim1
                                        height: colthumb.height / 2 - 2
                                        width: 50
                                        anchors.top: colfav1.bottom
                                        anchors.left: colthumb.right
                                        anchors.topMargin: 4
                                        color: (singleAlbumListModel.limit == index + 1)?"red":"purple"
                                        Text {
                                            text: "Limit to Here"
                                            color: (singleAlbumListModel.limit == index + 1)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if(singleAlbumListModel.limit == 0)
                                                    singleAlbumListModel.limit = index + 1;
                                                else
                                                    singleAlbumListModel.limit = 0;
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colart1
                                        height: collim1.height
                                        width: collim1.width
                                        anchors.top: collim1.top
                                        anchors.left: collim1.right
                                        anchors.leftMargin: 4
                                        //anchors.topMargin: 4
                                        color: (singleAlbumListModel.coveruri == thumburi)?"red":"purple"
                                        Text {
                                            text: "Set as Album Art"
                                            color: (singleAlbumListModel.coveruri == thumburi)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                singleAlbumListModel.coveruri = thumburi;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                Item {
                    id: scene4
                    width: 250
                    height: parent.height
                    anchors.top: scene3.top
                    anchors.left: scene3.right

                    PhotoListModel {
                        id: recentlyViewed
                        type: PhotoListModel.ListofRecentlyViewed
                        limit: 0
                    }

                    Rectangle {
                        id: filterbuttons4
                        width: 40
                        height: scene2.height
                        color: "black"
                        Rectangle {
                            id: filterall4
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons4.top
                                anchors.left: filterbuttons4.left
                                text: "Show All"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        recentlyViewed.filter = 0
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterrv4
                            anchors.top: filterall4.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons4.top
                                anchors.left: filterbuttons4.left
                                text: "Recently Viewed"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        recentlyViewed.filter = 2
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterra4
                            anchors.top: filterrv4.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons4.top
                                anchors.left: filterbuttons4.left
                                text: "Recently Added"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        recentlyViewed.filter = 3
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterfav4
                            anchors.top: filterra4.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons4.top
                                anchors.left: filterbuttons4.left
                                text: "Favorites"
                                color: "white"
                                font.pixelSize: 20
                                font.bold: false
                                width: parent.width
                                height: parent.height
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        recentlyViewed.filter = 1
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: listarea4
                        anchors.left: filterbuttons4.right
                        width: scene3.width - filterbuttons4.width
                        height: scene3.height
                        color: "black"
                        ListView {
                            anchors.fill: parent
                            spacing: 10
                            model: recentlyViewed
                            delegate: Rectangle {
                                width: scene3.width
                                height: colthumb.height
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
                                        text: title
                                        color: "white"
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.left
                                        width: colthumb.width
                                        height: colthumb.height
                                        font.pixelSize: 20
                                        font.bold: false
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                    }
                                    Rectangle {
                                        id: colfav1
                                        height: colthumb.height / 2 - 2
                                        width: 100
                                        anchors.top: colthumb.top
                                        anchors.left: colthumb.right
                                        anchors.leftMargin: 4
                                        color: (favorite)?"red":"purple"
                                        Text {
                                            text: "Favorite"
                                            color: (favorite)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                recentlyViewed.setFavorite(itemid, !favorite)
                                            }
                                        }
                                    }
                                    Rectangle {
                                        id: colrv1
                                        height: colthumb.height / 2 - 2
                                        width: 100
                                        anchors.top: colfav1.bottom
                                        anchors.left: colthumb.right
                                        anchors.leftMargin: 4
                                        anchors.topMargin: 4
                                        color: (recentlyviewed)?"red":"purple"
                                        Text {
                                            text: "Recently Viewed"
                                            color: (recentlyviewed)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: 20
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                recentlyViewed.setViewed(itemid)
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
