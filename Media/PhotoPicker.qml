/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1
import MeeGo.Media 0.1


/* Photo Picker
 * Displays a list of photo albums and prompts to select photo from
 * one of the albums.  If photo selected OK is enabled which triggers
 * the photoSelected signal.
 *
 * Properties:
 *   albumSelectionMode - if true, selects albums instead of photos.
 *
 * Signal:
 *   photoSelected - this is not triggered on cancel
 *      photoid - ID of the selected photo
 *      title - title of the selected photo
 *      uri - path to the photo
 *   albumSelected - triggered if albumSelectionMode is true
 *      albumid - ID of the selected photo album
 *      title - title of the selected photo album
 *   closed
 *   opened
 *   cancel
 */

ModalSurface {
    id: photoPicker

    property bool albumSelectionMode: false
    property string promptText: albumSelectionMode ? qsTr("Pick an album") : qsTr("Pick a photo")
    property string albumText: qsTr("Albums")

    signal photoSelected(variant photoid, string title, string uri, string thumburi)
    signal albumSelected(variant albumid, string title)

    signal cancel

    // probably will remove these signals at some point
    signal closed
    signal opened

    autoCenter: true

    function show() {
        console.log("DEPRECATED! Call visible = true on the PhotoPicker yourself!")
        visible = true
    }


    content: BorderImage {
        id: inner

        property alias showImageLabels: imageGrid.showLabels

        property variant selectedPhotoID: undefined
        property variant selectedPhotoTitle: ""
        property string selectedPhotoURI: ""
        property string selectedPhotoThumbURI: ""
        property variant selectedAlbumID: undefined
        property variant selectedAlbumTitle: ""

        TopItem {
            id: topItem
        }

        border.top: 14
        border.left: 20
        border.right: 20
        border.bottom: 20

        source: "image://theme/notificationBox_bg"

        property int internalBorder: 10

        function updateWidth() {
            var w = topItem.topItem.width
            var h = topItem.topItem.height
            var columns = w > h ? 5 : 2
            width = (columns * (gridView.cellWidth + 2)) - 2 + 2 * internalBorder
        }

        height: topItem.topItem.height * 0.8

        Connections {
            target: gridView
            onCellWidthChanged: {
                inner.updateWidth()
            }
        }

        Text {
            id: header

            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: photoPicker.promptText
            font.weight: Font.Bold
            font.pixelSize: theme_dialogTitleFontPixelSize
            color: theme_dialogTitleFontColor
        }

        // image grid
        Item {

            id: imageGrid

            property bool showLabels: false

            anchors { left: parent.left; top: header.bottom; right: parent.right; bottom: footer.top }
            anchors.margins: parent.internalBorder

            property Item highlightedItem // keep track of what item is currently highlighted


                MediaGridView {
                    id: gridView

                    model: theModel
                    type: albumSelectionMode ? photoalbumtype : phototype
                    selectionMode: true
                    anchors.fill: parent;
                    spacing: 2
                    clip: true
                    defaultThumbnail: "image://theme/media/photo_thumb_default"

                    cellWidth: {
                        var w = topItem.topItem.width
                        var h = topItem.topItem.height
                        return Math.floor((w > h ? h : w) / 3) - 2
                    }
                    cellHeight: cellWidth

                    property string selectedItem

                    onClicked: {
                        model.setSelected(selectedItem, false)
                        model.setSelected(payload.mitemid, true)
                        selectedItem = payload.mitemid

                        if (photoPicker.albumSelectionMode) {
                            // album mode
                            inner.selectedAlbumID = payload.mitemid
                            inner.selectedAlbumTitle = payload.mtitle
                        }
                        else {
                            // photo mode
                            inner.selectedPhotoID = payload.mitemid
                            inner.selectedPhotoTitle = payload.mtitle
                            inner.selectedPhotoURI = payload.muri
                            inner.selectedPhotoThumbURI = payload.mthumburi
                        }
                    }
                }
            }

        Row {
            id: footer
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            height: childrenRect.height
            spacing: 18
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: button1
                width: 210
                height: 60
                active: ( photoPicker.albumSelectionMode ? ( inner.selectedAlbumID != undefined ) :
                           (  inner.selectedPhotoID != undefined ) )
                enabled: active
                title: qsTr("OK")
                onClicked: {
                    if (photoPicker.albumSelectionMode) {
                        albumSelected(inner.selectedAlbumID, inner.selectedAlbumTitle)
                    } else {
                        photoSelected(inner.selectedPhotoID, inner.selectedPhotoTitle, inner.selectedPhotoURI, inner.selectedPhotoThumbURI)
                    }
                    closed()
                    close()
                }
            }

            Button {
                id: button2
                width: button1.width
                height: button1.height
                title: qsTr("Cancel")
                onClicked: {
                    cancel()
                    close()
                }
            }
        }

        PhotoListModel {
            id: theModel

            limit: 0
            sort: PhotoListModel.SortByDefault

            type: photoPicker.albumSelectionMode ? PhotoListModel.ListofUserAlbums : PhotoListModel.ListofPhotos
        }
    }  // end of inner
}
