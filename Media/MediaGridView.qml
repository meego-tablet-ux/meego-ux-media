/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1

GridView {
    id: gridView
    clip:true
    cellWidth: 80
    cellHeight: 80

    // apptype: 0=music, 1=video, 2=photo
    property int musictype: 0
    property int videotype: 1
    property int phototype: 2
    property int photoalbumtype: 0
    property int type: 0
    property int spacing: 2
    property int footerHeight: 50
    property bool selectionMode: false
    property int frameBorderWidth: 0
    property string defaultThumbnail: ""

    signal clicked(real mouseX, real mouseY, variant payload)
    signal longPressAndHold(real mouseX, real mouseY, variant payload)
    signal doubleClicked(real mouseX, real mouseY, variant payload)
    signal released(real mouseX, real mouseY, variant payload)
    signal positionChanged(real mouseX, real mouseY, variant payload)

    delegate: BorderImage {
        id: dinstance
        width: gridView.cellWidth-spacing
        height: gridView.cellHeight-spacing
        source: "image://theme/media/photos_thumb_med"
        clip: true

        Component.onCompleted: gridView.currentIndex = 0

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
            BorderImage {
                id: textBackground
                source: "image://theme/media/music_text_bg_med"
                border.left: 3; border.top: 3
                border.right: 3; border.bottom: 3
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
            BorderImage {
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
                gridView.clicked(mouseX,mouseY, dinstance);
            }
            onPressAndHold: {
                gridView.longPressAndHold(mouseX,mouseY,dinstance);
            }
            onDoubleClicked: {
                gridView.doubleClicked(mouseX,mouseY,dinstance);
            }
            onReleased: {
                gridView.released(mouseX,mouseY,dinstance);
            }
            onPositionChanged: {
                gridView.positionChanged(mouseX,mouseY,dinstance);
            }
        }
        states: [
            State {
                name: "normal"
                when: !selectionMode && !mouseArea.pressed
                PropertyChanges {
                    target: frame
                    source: "image://theme/media/photos_thumb_med"
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
                    source: "image://theme/media/photos_thumb_med"
                }
            },
            State {
                name: "selectionSelected"
                when: selectionMode && gridView.model.isSelected(itemid)
                PropertyChanges {
                    target: frame
                    source: "image://theme/media/photos_selected_tick"
                }
            }

        ]
    }
}
