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
    id: scene
    width: 1024
    height: 600
    showsearch: false
    onStatusBarTriggered: orientation = !orientation
    backgroundColor: view.cellBackgroundColor

    Crumb {
        id: landingScreenCrumb
        label: "PhotosView Test"
        payload: 0
    }
    crumbTrail: [landingScreenCrumb]

    PhotoListModel {
        id: allPhotosListModel
        type: PhotoListModel.ListofPhotos
        limit: 0
        sort: PhotoListModel.SortByTitle
    }

    Item {
        parent: scene.container
        anchors.top: parent.top
        anchors.topMargin: scene.toolbarheight
        anchors.left: parent.left
        width: scene.topicsWidth
        height: parent.height - scene.toolbarheight

        Column {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.topMargin: 20
            spacing: 10
            Rectangle {
                width: parent.width - 20
                height: 50
                radius: 10
                color: "lightgray"
                Text {
                    anchors.centerIn: parent
                    font.pixelSize: theme_fontPixelSizeLargest
                    text: view.selectionMode ? "Select" : "Interact"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: view.selectionMode = !view.selectionMode
                }
            }
            Rectangle {
                width: parent.width - 20
                height: 50
                radius: 10
                color: "lightgray"
                Text {
                    anchors.centerIn: parent
                    font.pixelSize: theme_fontPixelSizeLargest
                    text: "Dump Stats"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log(view.selected)
                    }
                }
            }
        }
    }

    PhotosView {
        id: view
        parent: scene.content
        anchors.fill: parent
        anchors.topMargin: scene.toolbarheight
        model: allPhotosListModel
    }
}
