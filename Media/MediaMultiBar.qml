import Qt 4.7
import MeeGo.Labs.Components 0.1
import MeeGo.Sharing 0.1
import MeeGo.Media 0.1

Item {
    id: container
    width: background.width
    height: background.height

    property bool landscape: false
    property alias sharing: shareIcon

    signal cancelPressed()
    signal deletePressed()
    property bool showadd: false
    signal addPressed()
    property int itemwidth: background.width/((showadd)?4:3)

    Image {
        id: background
        anchors.fill: parent
        source: "image://meegotheme/widgets/common/action-bar/action-bar-background"
        Item {
            id: btShare
            anchors.left: parent.left
            anchors.top:parent.top
            height: parent.height
            width: itemwidth
            ShareIcon {
                id: shareIcon
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Image{
                id: divider
                anchors.right: parent.right
                source: "image://meegotheme/widgets/common/action-bar/action-bar-separator"
                height: parent.height
            }
        }
        MediaToolbarButton {
            id: btAdd
            anchors.left: btShare.right
            anchors.top:parent.top
            height: parent.height
            width: (showadd)?itemwidth:0
            visible: showadd
            bgSourceUp: "image://theme/media/icn_addtoalbum_up"
            bgSourceDn: "image://theme/media/icn_addtoalbum_dn"
            onClicked: container.addPressed()
        }
        MediaToolbarButton {
            id: btDelete
            anchors.left: btAdd.right
            anchors.top: parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://theme/media/icn_trash_up"
            bgSourceDn: "image://theme/media/icn_trash_dn"
            onClicked: container.deletePressed()
        }
        MediaToolbarButton {
            id: cancelButton
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://theme/media/icn_cancel_ms_up"
            bgSourceDn: "image://theme/media/icn_cancel_ms_dn"
            onClicked: container.cancelPressed()
        }
    }
}

