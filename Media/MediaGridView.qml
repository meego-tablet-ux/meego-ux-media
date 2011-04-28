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

Item {
    id: container

    // apptype: 0=music, 1=video, 2=photo
    property int musictype: 0
    property int videotype: 1
    property int phototype: 2
    property int photoalbumtype: 0
    property int type: 0
    property int spacing: 2
    property bool showHeader: false
    property int headerHeight: theme_fontPixelSizeLarge + 10
    property int footerHeight: 0
    property bool selectionMode: false
    property int frameBorderWidth: 0
    property string defaultThumbnail: ""

    property alias clip: gridView.clip
    property alias cellWidth: gridView.cellWidth
    property alias cellHeight: gridView.cellHeight
    property alias model: gridView.model
    property alias header: gridView.header
    property alias footer: gridView.footer
    property alias currentItem: gridView.currentItem
    property alias currentIndex: gridView.currentIndex
    property alias theGridView: gridView

    signal clicked(real mouseX, real mouseY, variant payload)
    signal longPressAndHold(real mouseX, real mouseY, variant payload)
    signal doubleClicked(real mouseX, real mouseY, variant payload)
    signal released(real mouseX, real mouseY, variant payload)
    signal positionChanged(real mouseX, real mouseY, variant payload)

    Component {
        id: headerComponent
        Item {
            width: gridView.width
            height: (visible)?container.headerHeight:0
            visible: (container.showHeader && gridView.model.filter != MusicListModel.FilterAll)
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: theme_fontPixelSizeLarge
                color: theme_fontColorNormal
                text: ((gridView.model.filter == MusicListModel.FilterFavorite)?qsTr("Favorites"):
                       (gridView.model.filter == MusicListModel.FilterViewed)?qsTr("Recently Viewed"):
                       (gridView.model.filter == MusicListModel.FilterAdded)?qsTr("Recently Added"):
                       (gridView.model.filter == MusicListModel.FilterUnwatched)?qsTr("Unwatched"):
                       (gridView.model.filter == MusicListModel.FilterSearch)?qsTr("Search Results"):
                       (""))
            }
        }
    }

    Component {
        id: footerComponent
        Item {
            width: gridView.width
            height: container.footerHeight
        }
    }

    GridView {
        id: gridView
        clip:true
        cellWidth: 80
        cellHeight: 80
        header: headerComponent
        footer: footerComponent
        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds
        delegate: Image {
            id: dinstance
            width: gridView.cellWidth-spacing
            height: gridView.cellHeight-spacing
            source: "image://meegotheme/images/media/photos_thumb_med"
            clip: true

            function formatMinutes(time)
            {
                var min = parseInt(time/60);
                return min
            }
            property int mindex: index

            property string mtitle
            mtitle:{
                try
                {
                    return title;
                }
                catch(err)
                {
                    return "";
                }
            }
            property string mthumburi
            mthumburi:{
                try
                {
                    if (thumburi == "" | thumburi == undefined)
                        return defaultThumbnail;
                    else
                        return thumburi;
                }
                catch(err)
                {
                    return defaultThumbnail
                }
            }
            property string mitemid
            mitemid:{
                try
                {
                    return itemid;
                }
                catch(err)
                {
                    return "";
                }
            }
            property int mitemtype
            mitemtype:{
                try
                {
                    return itemtype;
                }
                catch(err)
                {
                    return -1;
                }
            }
            property bool mfavorite
            mfavorite: {
                try
                {
                    return favorite;
                }
                catch(err)
                {
                    return false;
                }
            }
            property int mcount
            mcount: {
                try
                {
                    return tracknum;
                }
                catch(err)
                {
                    return 0;
                }
            }
            property int mlength
            mlength: {
                try
                {
                    return length;
                }
                catch(err)
                {
                    return 0;
                }
            }
            property string martist
            martist: {
                var a;
                try
                {
                    a = artist ;
                }
                catch(err)
                {
                    a = "";
                }
                a[0]== undefined ? "":a[0];
            }
            property string malbum: (type == 0)?album:""
            property string muri: uri
            property string murn: urn

            property bool misvirtual: (type != 1)?isvirtual:false

            Item {
                id: thumbnailClipper
                anchors.fill:parent
                z: -10

                Item {
                    id: wrapper
                    anchors.fill: parent
                    transformOrigin: Item.Center
                    rotation: extension.orientation * 90

                    Image {
                        id: thumbnail
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop
                        source: mthumburi
                        smooth: true
                        clip: true
                        z: 0

                        Rectangle {
                            id: fog
                            anchors.fill: parent
                            color: "white"
                            opacity: 0.25
                            visible: false
                        }
                    }

                    ImageExtension {
                        id: extension
                        source: muri
                    }
                }

                Image {
                    id: textBackground
                    source: "image://meegotheme/images/media/music_text_bg_med"
                    width: parent.width
                    height: 63
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    z: 1
                    visible: (type != 2)
                    Text {
                        id: titleText
                        text: mtitle
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        width: parent.width - 20
                        elide: Text.ElideRight
                        font.pixelSize: theme_fontPixelSizeMedium
                        font.bold: true
                        color:theme_fontColorMediaHighlight
                    }
                    Text {
                        id: artistText
                        text: (type == 1)?((formatMinutes(length)==1)?qsTr("%1 Minute").arg(formatMinutes(length)):qsTr("%1 Minutes").arg(formatMinutes(length))):martist
                        font.pixelSize: theme_fontPixelSizeMedium
                        anchors.top: titleText.bottom
                        anchors.topMargin: 4
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        width: parent.width - 20
                        elide: Text.ElideRight
                        color:theme_fontColorMediaHighlight
                        visible: text
                    }
                }
                Image {
                    id: frame
                    anchors.centerIn: parent
                    width: parent.width - 2 * frameBorderWidth
                    height: parent.height - 2 * frameBorderWidth
                    smooth: true
                    z: 2
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill:parent
                onClicked:{
                    container.clicked(mouseX,mouseY, dinstance);
                }
                onPressAndHold: {
                    container.longPressAndHold(mouseX,mouseY,dinstance);
                }
                onDoubleClicked: {
                    container.doubleClicked(mouseX,mouseY,dinstance);
                }
                onReleased: {
                    container.released(mouseX,mouseY,dinstance);
                }
                onPositionChanged: {
                    container.positionChanged(mouseX,mouseY,dinstance);
                }
            }
            states: [
                State {
                    name: "normal"
                    when: !selectionMode && !mouseArea.pressed
                    PropertyChanges {
                        target: frame
                        source: "image://meegotheme/images/media/photos_thumb_med"
                    }
                },
                State {
                    name: "feedback"
                    when: !selectionMode && mouseArea.pressed
                    PropertyChanges {
                        target: fog
                        visible: true
                    }
                },
                State {
                    name: "selectionNotSelected"
                    when: selectionMode && !gridView.model.isSelected(itemid)
                    PropertyChanges {
                        target: frame
                        source: "image://meegotheme/images/media/photos_thumb_med"
                    }
                },
                State {
                    name: "selectionSelected"
                    when: selectionMode && gridView.model.isSelected(itemid)
                    PropertyChanges {
                        target: frame
                        source: "image://meegotheme/images/media/photos_selected_tick"
                    }
                }
            ]
        }
    }
}
