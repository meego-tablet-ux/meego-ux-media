import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Media 0.1

Item {
    id: dinstance
    width: gridView.cellWidth
    height: gridView.cellHeight

    resources: [
        FuzzyDateTime {
            id: fuzzy
        }
    ]

    property int mindex: index
    property string mtitle
    property string muri: uri
    property string murn: urn
    property string mthumburi
    property string mitemid
    property int mitemtype
    property string maddedtime
    property int mphotocount

    property bool misvirtual: isvirtual

    mtitle:{
        try {
            return title
        }
        catch(err){
            return ""
        }
    }

    mthumburi:{
        try {
            if (thumburi == "" | thumburi == undefined)
                return defaultThumbnail
            else
                return thumburi
        }
        catch(err){
            return defaultThumbnail
        }
    }

    mitemid:{
        try {
            return itemid;
        }
        catch(err) {
            return ""
        }
    }

    mitemtype:{
        try {
            return itemtype
        }
        catch(err) {
            return -1
        }
    }

    maddedtime: {
        try {
            return addedtime;
        }
        catch(err) {
            return ""
        }
    }

    mphotocount: {
        try {
            return photocount;
        }
        catch(err) {
            return 0
        }
    }

    Image {
        id: content
        width: 104
        height: 108

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left

        asynchronous: true

        Item {
                id: wrapper
                anchors.fill: parent
                anchors.topMargin: 5
                anchors.bottomMargin: 9
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                transformOrigin: Item.Center
                rotation: extension.orientation * 90

                Image {
                    id: thumbnail
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    source: mthumburi
                    smooth: !gridView.moving
                    clip: true
                    asynchronous: true
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
            Item {
                id: frame
                anchors.fill: wrapper
                z: 2
                visible: false
                Rectangle {
                    anchors.fill: parent
                    color: "white"
                    opacity: 0.7
                }
                Image {
                    id: tick
                    asynchronous: true
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 10
                    source: "image://themedimage/widgets/apps/media/photo-selected-tick"
                }
            }
        }
        Image {
            z: 10
            anchors.fill: parent
            source: "image://themedimage/widgets/apps/media/tile-border-photos-album"
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
                    visible: false
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
                    visible: false
                }
            },
            State {
                name: "selectionSelected"
                when: selectionMode && gridView.model.isSelected(itemid)
                PropertyChanges {
                    target: frame
                    visible: true
                }
            }
        ]
    }

    Item {
        id: textBackground
        width: content.width * 2
        height: 108
        opacity: theme_mediaGridTitleBackgroundAlpha
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: parent.right
        z: 1
        Text {
            id: titleText
            text: mtitle
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 5
            width: parent.width - 20
            elide: Text.ElideRight
            font.pixelSize: theme_fontPixelSizeNormal
            font.bold: true
            color: theme_fontColorHighlight
        }
        Column {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 9
            anchors.left: parent.left
            anchors.leftMargin: 5
            Text {
                id: photocountText
                text: (mphotocount == 1)? qsTr("1 Photo") : qsTr("%1 Photos").arg(mphotocount)
                font.pixelSize: theme_fontPixelSizeNormal
                width: titleText.width
                elide: Text.ElideRight
                color: theme_fontColorMedium
            }
            Text {
                id: addedTimeText
                text: fuzzy.getFuzzy(maddedtime)
                font.pixelSize: theme_fontPixelSizeNormal
                width: titleText.width
                elide: Text.ElideRight
                color: theme_fontColorMedium
            }
        }
    }
}


