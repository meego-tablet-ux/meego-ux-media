/*
* Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0


/*!
  \qmlclass MucMediaGridView
  \title MucMediaGridView
  \section1 MucMediaGridView
  Displays a given set of images representing videos, folders or image files in a grid view.

  \section2  API properties

  \qmlproperty int spacing
  \qmlcm the border margin of the items.

  \qmlproperty string defaultThumbnail
  \qmlcm the default image for the items.

  \qmlproperty int frameBorderWidth
  \qmlcm margin of the selection overlay.

  \qmlproperty bool selectionMode
  \qmlcm on true the selected item is highlighted, on false only a pressed state is provided.

  \qmlproperty int type
  \qmlcm the internal type. See the private properties for details.

  \section2 Signals
  \qmlsignal clicked
  \qmlcm emitted when an item was clicked.
       \param int mouseX
       \qmlpcm position of the click event. \endparam
       \param int mouseY
       \qmlpcm position of the click event. \endparam
       \param int payload
       \qmlpcm the clicked item. \endparam

  \qmlsignal longPressAndHold
  \qmlcm emitted when an item was long pressed.
       \param int mouseX
       \qmlpcm position of the click event. \endparam
       \param int mouseY
       position of the click event. \endparam
       \param variant payload
       \qmlpcm variant, the clicked item. \endparam

  \qmlsignal doubleClicked
  \qmlcm emitted when an item was double clicked. Note that a 'clicked' for the first click is emitted as well.
       \param int mouseX
       \qmlpcm position of the click event. \endparam
       \param int mouseY
       \qmlpcm position of the click event. \endparam
       \param variant payload
       \qmlpcm the clicked item. \endparam

  \qmlsignal released
  \qmlcm emitted when the left mouse button was released.
       \param int mouseX
       \qmlpcm position of the click event. \endparam
       \param int mouseY
       \qmlpcm position of the click event. \endparam
       \param variant payload
       \qmlpcm the clicked item. \endparam

  \qmlsignal positionChanged
  \qmlcm emitted when the mouse moves above the item.
       \param int mouseX
       \qmlpcm position of the click event. \endparam
       \param int mouseY
       \qmlpcm position of the click event. \endparam
       \param int payload
       \qmlpcm the hovered item. \endparam

  \section2 Functions
  \qmlfn formatMinutes
  \qmlcm provides functionality to calculate hours and minutes. Only for qml internal use.
       \param object time
       \qmlpcm for internal use only  \endparam
  \section2 Example
   \code
       MucMediaGridView {
           id: gridView

           model: someModel

           anchors.top: parent.top
           anchors.bottom: parent.bottom

           type: 2

           width: 400

           onClicked: {
               // selected item and click position
           }
       }
   \endcode
*/

import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Media 0.1

Item {
    id: container

    // apptype: 0=music, 1=video, 2=photo, 3=photoalbum
    property int musictype: 0
    property int videotype: 1
    property int phototype: 2
    property int photoalbumtype: 3
    property int type: 0
    property int spacing: 2
    property bool showHeader: false
    property int headerHeight: theme_fontPixelSizeLarge + 10
    property int footerHeight: 0
    property bool selectionMode: false
    property int frameBorderWidth: 0
    property string defaultThumbnail: ""

    property string delegateFooterSource: ""
    property bool delegateFooterVisible: false
    property string delegateHeaderSource: ""
    property bool delegateHeaderVisible: false

    property string borderImageSource: ""
    property int borderImageTop: 0
    property int borderImageBottom: 0
    property int borderImageLeft: 0
    property int borderImageRight: 0
    property int borderImageInnerMargin: 0

    property alias cellWidth: gridView.cellWidth
    property alias cellHeight: gridView.cellHeight
    property alias model: gridView.model
    property alias header: gridView.header
    property alias footer: gridView.footer
    property alias currentItem: gridView.currentItem
    property alias currentIndex: gridView.currentIndex
    property alias count: gridView.count
    property alias snapMode: gridView.snapMode
    property alias theGridView: gridView

    signal clicked(real mouseX, real mouseY, variant payload)
    signal longPressAndHold(real mouseX, real mouseY, variant payload)
    signal doubleClicked(real mouseX, real mouseY, variant payload)
    signal released(real mouseX, real mouseY, variant payload)
    signal positionChanged(real mouseX, real mouseY, variant payload)

    function setMargins() {
        var columns = Math.floor(parent.width / cellWidth)
        var gridWidth = columns * cellWidth
        var remain = parent.width - gridWidth
        anchors.leftMargin = Math.floor(remain / 2);
    }

    Component.onCompleted: setMargins()
    onWidthChanged: setMargins()

    function formatMinutes(time){
        var min = parseInt( time / 60 );
        return min
    }

    Component {
        id: headerComponent
        Item {
            anchors.leftMargin: spacing/2
            width: gridView.width
            height: (visible)?container.headerHeight:0
            visible: (container.showHeader && gridView.model.filter != MusicListModel.FilterAll)
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: theme_fontPixelSizeLarge
                color: theme_fontColorNormal
                text: ((gridView.model.filter == MusicListModel.FilterFavorite)?qsTr("Favorites"):
                       (gridView.model.filter == MusicListModel.FilterViewed)?qsTr("Recently viewed"):
                       (gridView.model.filter == MusicListModel.FilterAdded)?qsTr("Newest"):
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

        anchors.fill: parent
        clip: true
        cellWidth: theme.thumbSize
        cellHeight: cellWidth

        header: headerComponent
        footer: footerComponent

        interactive: contentHeight > height
        boundsBehavior: Flickable.StopAtBounds

        cacheBuffer: 20000
        flickDeceleration: 250

        delegate: Item {
            id: dinstance
            width: gridView.cellWidth
            height: gridView.cellHeight

            property int mindex: index
            property string mtitle
            property string malbum
            property string muri: uri
            property string murn: urn
            property string mthumburi
            property string mitemid
            property int mitemtype
            property bool mfavorite
            property int mcount
            property int mlength
            property string martist
            property string maddedtime
            property int mphotocount

            property bool misvirtual: (type != 1)?isvirtual:false

            mtitle:{
                try {
                    return title
                }
                catch(err){
                    return ""
                }
            }

            mlength:{
                try {
                    return length
                }
                catch(err){
                    return 0
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

            malbum:{
                try {
                    return album;
                }
                catch(err) {
                    return ""
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

            mfavorite: {
                try {
                    return favorite;
                }
                catch(err) {
                    return false
                }
            }

            mcount: {
                try {
                    return tracknum;
                }
                catch(err) {
                    return 0
                }
            }

            martist: {
                var a;
                try {
                    a = artist
                }
                catch(err) {
                    a = ""
                }
                a[0]== undefined ? "" : a[0]
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
                width: gridView.cellWidth - spacing
                height: gridView.cellHeight - spacing
                anchors.centerIn: parent

                asynchronous: true

                Image {
                    id: delegateHeader
                    source: delegateHeaderSource
                    anchors.bottom: thumbnailClipper.top
                    anchors.left:  thumbnailClipper.left
                    width: thumbnailClipper.width
                    visible: delegateHeaderVisible
                    asynchronous: true
                }

                BorderImage {
                    id: thumbnailClipper
                    anchors.fill:parent
                    z: -10
                    asynchronous: true

                    source: borderImageSource
                    border.top: borderImageTop
                    border.bottom: borderImageBottom
                    border.left: borderImageLeft
                    border.right: borderImageRight

                    Item {
                        id: wrapper
                        anchors.fill: parent
                        anchors.topMargin: thumbnailClipper.border.top - borderImageInnerMargin
                        anchors.bottomMargin: thumbnailClipper.border.bottom - borderImageInnerMargin
                        anchors.leftMargin: thumbnailClipper.border.left - borderImageInnerMargin
                        anchors.rightMargin: thumbnailClipper.border.right - borderImageInnerMargin
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
                    }


                    Rectangle {
                        id: metaDataBackground
                        width: (metaDataText.width + 12 < (wrapper.width / 6))? wrapper.width / 6 : metaDataText.width + 12
                        height: 24
                        color: theme_mediaGridTitleBackgroundColor
                        opacity: theme_mediaGridTitleBackgroundAlpha
                        anchors.top: wrapper.top
                        anchors.right: wrapper.right
                        z: 1
                        visible: (type == photoalbumtype)
                        Text {
                            id: metaDataText
                            text: mphotocount
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            // TODO: Fix the padding
                            anchors.topMargin: 4
                            anchors.rightMargin: 6
                            font.pixelSize: theme_fontPixelSizeMedium
                            font.bold: true
                            color:theme_fontColorMediaHighlight
                        }
                    }

                    Rectangle {
                        id: textBackground
                        width: wrapper.width
                        height: (type == photoalbumtype)? 34 : 63
                        color: theme_mediaGridTitleBackgroundColor
                        opacity: theme_mediaGridTitleBackgroundAlpha
                        anchors.bottom: wrapper.bottom
                        anchors.left: wrapper.left
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
                    id: delegateFooter
                    source: delegateFooterSource
                    anchors.top: thumbnailClipper.bottom
                    anchors.left:  thumbnailClipper.left
                    width: thumbnailClipper.width
                    asynchronous: true
                    visible: delegateFooterVisible
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
        }

        Theme { id: theme }
    }
}
