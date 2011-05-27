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

    property alias model: gridView.model
    property alias header: gridView.header
    property alias footer: gridView.footer
    property alias currentItem: gridView.currentItem
    property alias currentIndex: gridView.currentIndex
    property alias count: gridView.count
    property alias snapMode: gridView.snapMode
    property alias theGridView: gridView
    property int contHeight: 0

    signal clicked(real mouseX, real mouseY, variant payload)
    signal longPressAndHold(real mouseX, real mouseY, variant payload)
    signal doubleClicked(real mouseX, real mouseY, variant payload)
    signal released(real mouseX, real mouseY, variant payload)
    signal positionChanged(real mouseX, real mouseY, variant payload)
    signal contentHeightChanged()

    function setMargins() {
        var columns = Math.floor(parent.width / gridView.cellWidth)
        var gridWidth = columns * gridView.cellWidth
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

    Component {
        id: photoDelegate
        MediaGridPhotoDelegate { }
    }

    Component {
        id: photoAlbumDelegate
        MediaGridPhotoAlbumDelegate { }
    }

    Component {
        id: musicAlbumDelegate
        MediaGridMusicAlbumDelegate { }
    }

    Component {
        id: videoDelegate
        MediaGridVideoDelegate { }
    }

    GridView {
        id: gridView

        anchors.fill: parent
        clip: true

        // TODO: Use defined values from theme
        cellWidth: {
            switch(type) {
            case 0:
                return theme.thumbSize
                break;
            case 1:
                return theme.thumbSize
                break;
            case 2:
                return 114
                break;
            case 3:
                return 326
                break;
            default:
                return theme.thumbSize
            }
        }

        // TODO: Use defined values from theme
        cellHeight: {
            switch(type) {
            case 0:
                return cellWidth
                break;
            case 1:
                return cellWidth
                break;
            case 2:
                return 114
                break;
            case 3:
                return 128
                break;
            default:
                return cellWidth
            }
        }

        onContentHeightChanged: {
            container.contHeight = contentHeight
            container.contentHeightChanged()
        }

        header: headerComponent
        footer: footerComponent

        interactive: contentHeight > height
        boundsBehavior: Flickable.StopAtBounds

        cacheBuffer: 20000
        flickDeceleration: 250

        delegate: {
            switch(type) {
            case 0:
                return musicAlbumDelegate
                break;
            case 1:
                return videoDelegate
                break;
            case 2:
                return photoDelegate
                break;
            case 3:
                return photoAlbumDelegate
                break;
            default:
                return musicAlbumDelegate
            }
        }
        Theme { id: theme }
    }
}
