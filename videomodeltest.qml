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
    title: qsTr("Video")
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

                Item {
                    id: scene1
                    width: 250
                    height: 600
                    anchors.top: scene1.top
                    anchors.left: scene1.right

                    VideoListModel {
                        id: videoListModel
                        type: VideoListModel.ListofVideos
                        limit: 0
                        sort: VideoListModel.SortByTitle
                    }

                    Rectangle {
                        id: filterbuttons2
                        width: 40
                        height: scene1.height
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
                                        videoListModel.filter = 0
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
                                        videoListModel.filter = 2
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
                                        videoListModel.filter = 3
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
                                        videoListModel.filter = 1
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: listarea2
                        anchors.left: filterbuttons2.right
                        width: scene1.width - filterbuttons2.width
                        height: scene1.height
                        color: "black"
                        ListView {
                            anchors.fill: parent
                            spacing: 10
                            model: videoListModel
                            delegate: Rectangle {
                                width: scene1.width
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
                                        font.pixelSize: theme_fontPixelSizeSmaller
                                        font.bold: false
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.WordWrap
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                //singleAlbumListModel.album = title
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
                                            font.pixelSize: theme_fontPixelSizeSmaller
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                videoListModel.setFavorite(itemid, !favorite)
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
                                            font.pixelSize: theme_fontPixelSizeSmaller
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                videoListModel.setViewed(itemid)
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
                                        color: (videoListModel.limit == index + 1)?"red":"purple"
                                        Text {
                                            text: "Limit to Here"
                                            color: (videoListModel.limit == index + 1)?"white":"black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: theme_fontPixelSizeSmaller
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if(videoListModel.limit == 0)
                                                    videoListModel.limit = index + 1;
                                                else
                                                    videoListModel.limit = 0;
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
                                        color: ((playstatus == 0)?"red":((playstatus == 1)?"yellow":"green"))
                                        Text {
                                            text: ((playstatus == 0)?"stopped":((playstatus == 1)?"paused":"playing"))
                                            color: "black"
                                            width: parent.width
                                            height: parent.height
                                            font.pixelSize: theme_fontPixelSizeSmaller
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                videoListModel.setPlayStatus(itemid, VideoListModel.Playing);
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
                    height: 600
                    anchors.top: scene1.top
                    anchors.left: scene1.right

                    VideoListModel {
                        id: filteredVideoModel
                        type:VideoListModel.ListofAll
                        limit: 0
                        sort: VideoListModel.SortByTitle
                    }

                    Rectangle {
                        id: filterbuttons4
                        width: 40
                        height: scene1.height
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
                                        filteredVideoModel.filter = VideoListModel.FilterAll;
                                        filteredVideoModel.sort = VideoListModel.SortByTitle;
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
                                        filteredVideoModel.filter = VideoListModel.FilterViewed;
                                        filteredVideoModel.sort = VideoListModel.SortByAccessTime;
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
                                        filteredVideoModel.filter = VideoListModel.FilterAdded;
                                        filteredVideoModel.sort = VideoListModel.SortByAddedTime;
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
                                        filteredVideoModel.filter = VideoListModel.FilterFavorite;
                                        filteredVideoModel.sort = VideoListModel.SortByTitle;
                                    }
                                }
                            }
                        }
                        Rectangle {
                            id: filterunw4
                            anchors.top: filterfav4.bottom
                            width: parent.width
                            height: 50
                            color: "blue"
                            border.color: "black"
                            border.width: 5
                            radius: 10
                            Text {
                                anchors.top: filterbuttons4.top
                                anchors.left: filterbuttons4.left
                                text: "Unwatched"
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
                                        filteredVideoModel.filter = VideoListModel.FilterUnwatched;
                                        filteredVideoModel.sort = VideoListModel.SortByTitle;
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        id: listarea4
                        anchors.left: filterbuttons4.right
                        width: scene1.width - filterbuttons4.width
                        height: scene1.height
                        color: "black"
                        ListView {
                            anchors.fill: parent
                            spacing: 10
                            model: filteredVideoModel
                            delegate: Rectangle {
                                width: scene1.width
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
                                        font.pixelSize: theme_fontPixelSizeSmaller
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
                                            font.pixelSize: theme_fontPixelSizeSmaller
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                filteredVideoModel.setFavorite(itemid, !favorite)
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
                                            font.pixelSize: theme_fontPixelSizeSmaller
                                            font.bold: false
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignHCenter
                                            wrapMode: Text.WordWrap
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                filteredVideoModel.setViewed(itemid)
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
