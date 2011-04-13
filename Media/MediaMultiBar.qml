import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Sharing 0.1
import MeeGo.Media 0.1

Item {
    id: container
    width: background.width
    height: background.height

    property bool landscape: false
    property variant sharing

    signal cancelPressed()
    signal deletePressed()
    property bool showadd: false
    signal addPressed()
    property int itemwidth: background.width/((showadd)?4:3)

    ModalContextMenu {
        id: contextMenu
        property alias model: contextActionMenu.model
        content: ActionMenu {
            id: contextActionMenu
            onTriggered: {
                // Share
                var svcTypes = sharing.serviceTypes;
                for (x in svcTypes) {
                    if (model[index] == svcTypes[x]) {
                        sharing.showContext(model[index], contextMenu.x, contextMenu.y);
                        break;
                    }
                }
                contextMenu.hide();
            }
        }
    }

    Image {
        id: background
        anchors.fill: parent
        source: "image://meegotheme/widgets/common/action-bar/action-bar-background"
        MediaToolbarButton {
            id: btShare
            anchors.left: parent.left
            anchors.top:parent.top
            height: parent.height
            width: itemwidth
            visible: true
            bgSourceUp: "image://meegotheme/images/media/icn_share_up"
            bgSourceDn: "image://meegotheme/images/media/icn_share_up"
            onClicked: {
                if((sharing != undefined)&&(sharing.shareCount > 0))
                {
                    var map = mapToItem(topItem.topItem, btShare.x + btShare.width/2, btShare.y);
                    contextMenu.model = sharing.serviceTypes;
                    topItem.calcTopParent()
                    contextMenu.setPosition( map.x, map.y );
                    contextMenu.show();
                }
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
            onClicked: {
                if(sharing != undefined)
                    sharing.clearItems();
                container.cancelPressed();
            }
        }
    }
    TopItem { id: topItem }
}

