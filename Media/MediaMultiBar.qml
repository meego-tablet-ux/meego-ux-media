import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Sharing 0.1
import MeeGo.Media 0.1

Item {
    id: container
    width: background.width
    height: background.height

    property bool landscape: false

    signal cancelPressed()
    signal deletePressed()
    signal sharePressed(real fingerX, real fingerY)
    signal favouritePressed()
    signal unfavouritePressed()
    signal rmFromQueuePressed()
    signal rmFromPlaylistPressed()
    signal addToQueuePressed()
    signal addToPlaylistPressed()

    property bool showfavourite: false
    property bool showunfavourite: false
    property bool showrmfromqueue: false
    property bool showrmfromplaylist: false
    property bool showaddtoqueue: false
    property bool showaddtoplaylist: false
    property bool showdelete: true
    property int itemsvisible: 2+(showdelete?1:0)+(showfavourite?1:0)+(showunfavourite?1:0)+(showrmfromqueue?1:0)+(showrmfromplaylist?1:0)+(showaddtoqueue?1:0)+(showaddtoplaylist?1:0)
    property int itemwidth: background.width/itemsvisible

    Item {
        anchors.fill: parent
        Theme { id: theme }

        Rectangle {
            id: background
            anchors.fill: parent
            color: theme_mediaGridTitleBackgroundColor
            opacity: theme_mediaGridTitleBackgroundAlpha
        }

        MediaToolbarButton {
            id: btFavourite
            anchors.left: parent.left
            anchors.top:parent.top
            height: parent.height
            width: showfavourite?itemwidth:0
            visible: showfavourite
            bgSourceUp: "image://themedimage/icons/actionbar/favorite-active"
            bgSourceDn: "image://themedimage/icons/actionbar/favorite-active"
            onClicked: container.favouritePressed()
        }
        MediaToolbarButton {
            id: btUnfavourite
            anchors.left: btFavourite.right
            anchors.top:parent.top
            height: parent.height
            width: showunfavourite?itemwidth:0
            visible: showunfavourite
            bgSourceUp: "image://themedimage/icons/actionbar/favorite"
            bgSourceDn: "image://themedimage/icons/actionbar/favorite"
            onClicked: container.unfavouritePressed()
        }
        MediaToolbarButton {
            id: btAddToQueue
            anchors.left: btShare.right
            anchors.top:parent.top
            height: parent.height
            width: showaddtoqueue?itemwidth:0
            visible: showaddtoqueue
            bgSourceUp: "image://themedimage/icons/actionbar/media-playqueue-add"
            bgSourceDn: "image://themedimage/icons/actionbar/media-playqueue-add"
            onClicked: container.addToQueuePressed()
        }
        MediaToolbarButton {
            id: btAddToPlaylist
            anchors.left: btAddToQueue.right
            anchors.top:parent.top
            height: parent.height
            width: showaddtoplaylist?itemwidth:0
            visible: showaddtoplaylist
            bgSourceUp: "image://themedimage/icons/actionbar/media-playlist-add"
            bgSourceDn: "image://themedimage/icons/actionbar/media-playlist-add"
            onClicked: container.addToPlaylistPressed()
        }
        MediaToolbarButton {
            id: btShare
            anchors.left: btRmFromPlaylist.right
            anchors.top:parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://themedimage/icons/actionbar/share"
            bgSourceDn: "image://themedimage/icons/actionbar/share"
            onClicked: container.sharePressed(btShare.x + btShare.width/2, btShare.y)
        }
        MediaToolbarButton {
            id: btRmFromQueue
            anchors.left: btUnfavourite.right
            anchors.top:parent.top
            height: parent.height
            width: showrmfromqueue?itemwidth:0
            visible: showrmfromqueue
            bgSourceUp: "image://themedimage/icons/actionbar/media-playqueue-remove-active"
            bgSourceDn: "image://themedimage/icons/actionbar/media-playqueue-remove-active"
            onClicked: container.rmFromQueuePressed()
        }
        MediaToolbarButton {
            id: btRmFromPlaylist
            anchors.left: btRmFromQueue.right
            anchors.top:parent.top
            height: parent.height
            width: showrmfromplaylist?itemwidth:0
            visible: showrmfromplaylist
            bgSourceUp: "image://themedimage/icons/actionbar/media-playlist-remove"
            bgSourceDn: "image://themedimage/icons/actionbar/media-playlist-remove"
            onClicked: container.rmFromPlaylistPressed()
        }
        MediaToolbarButton {
            id: btDelete
            anchors.left: btAddToPlaylist.right
            anchors.top: parent.top
            height: parent.height
            width: showdelete?itemwidth:0
            visible: showdelete
            bgSourceUp: "image://themedimage/icons/actionbar/edit-delete"
            bgSourceDn: "image://themedimage/icons/actionbar/edit-delete"
            onClicked: container.deletePressed()
        }
        MediaToolbarButton {
            id: cancelButton
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://themedimage/icons/actionbar/edit-cancel"
            bgSourceDn: "image://themedimage/icons/actionbar/edit-cancel"
            onClicked: container.cancelPressed()
        }
    }
    TopItem { id: topItem }
}

