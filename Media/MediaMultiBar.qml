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
    property bool showfavourite: false
    property bool showunfavourite: false
    property bool showrmfromqueue: false
    property bool showrmfromplaylist: false
    property bool showaddtoqueue: false
    property bool showaddtoplaylist: false
    property bool showdelete: false
    signal addPressed()
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
            bgSourceUp: "image://themedimage/images/media/icn_favourite_up"
            bgSourceDn: "image://themedimage/images/media/icn_favourite_up"
            onClicked: container.sharePressed(btShare.x + btShare.width/2, btShare.y)
        }
        MediaToolbarButton {
            id: btUnfavourite
            anchors.left: btFavourite.right
            anchors.top:parent.top
            height: parent.height
            width: showunfavourite?itemwidth:0
            visible: showunfavourite
            bgSourceUp: "image://themedimage/images/media/icn_favourite_active"
            bgSourceDn: "image://themedimage/images/media/icn_favourite_active"
            onClicked: container.sharePressed(btShare.x + btShare.width/2, btShare.y)
        }
        MediaToolbarButton {
            id: btRmFromQueue
            anchors.left: btUnfavourite.right
            anchors.top:parent.top
            height: parent.height
            width: showrmfromqueue?itemwidth:0
            visible: showrmfromqueue
            bgSourceUp: "image://themedimage/images/media/icn_music_down"
            bgSourceDn: "image://themedimage/images/media/icn_music_down"
            onClicked: container.addPressed()
        }
        MediaToolbarButton {
            id: btRmFromPlaylist
            anchors.left: btRmFromQueue.right
            anchors.top:parent.top
            height: parent.height
            width: showrmfromplaylist?itemwidth:0
            visible: showrmfromplaylist
            bgSourceUp: "image://themedimage/images/media/icn_addtoalbum_up"
            bgSourceDn: "image://themedimage/images/media/icn_addtoalbum_dn"
            onClicked: container.addPressed()
        }
        MediaToolbarButton {
            id: btShare
            anchors.left: btRmFromPlaylist.right
            anchors.top:parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://themedimage/images/media/icn_share_up"
            bgSourceDn: "image://themedimage/images/media/icn_share_up"
            onClicked: container.sharePressed(btShare.x + btShare.width/2, btShare.y)
        }
        MediaToolbarButton {
            id: btAddToQueue
            anchors.left: btShare.right
            anchors.top:parent.top
            height: parent.height
            width: showaddtoqueue?itemwidth:0
            visible: showaddtoqueue
            bgSourceUp: "image://themedimage/images/media/icn_music_down"
            bgSourceDn: "image://themedimage/images/media/icn_music_down"
            onClicked: container.addPressed()
        }
        MediaToolbarButton {
            id: btAddToPlaylist
            anchors.left: btAddToQueue.right
            anchors.top:parent.top
            height: parent.height
            width: showaddtoplaylist?itemwidth:0
            visible: showaddtoplaylist
            bgSourceUp: "image://themedimage/images/media/icn_addtoalbum_up"
            bgSourceDn: "image://themedimage/images/media/icn_addtoalbum_dn"
            onClicked: container.addPressed()
        }
        MediaToolbarButton {
            id: btDelete
            anchors.left: btAddToPlaylist.right
            anchors.top: parent.top
            height: parent.height
            width: showdelete?itemwidth:0
            visible: showdelete
            bgSourceUp: "image://themedimage/images/media/icn_trash_up"
            bgSourceDn: "image://themedimage/images/media/icn_trash_dn"
            onClicked: container.deletePressed()
        }
        MediaToolbarButton {
            id: cancelButton
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://themedimage/images/media/icn_cancel_ms_up"
            bgSourceDn: "image://themedimage/images/media/icn_cancel_ms_dn"
            onClicked: {
                container.cancelPressed();
            }
        }
    }
    TopItem { id: topItem }
}

