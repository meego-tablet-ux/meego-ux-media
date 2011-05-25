import QtQuick 1.0
import MeeGo.Media 0.1 as Models
import MeeGo.Components 0.1
import Qt.labs.folderlistmodel 1.0

ModalDialog {
    id: simpleDocumentPicker
    /*ListModel {
        id: documentListModel
        ListElement {
            filename: "msword.doc"
            thumbURI: "image://themedimage/images/media/icn_ms_word"
        }
        ListElement {
            filename: "adobepdf.pdf"
            thumbURI: "image://themedimage/images/media/icn_adobe_pdf"
        }
        ListElement {
            filename: "plaintext.txt"
            thumbURI: "image://themedimage/images/media/icn_plain_txt";
        }
    }*/

    FolderListModel {
        id: documentListModel
        showDirs: false
        nameFilters: ["*.doc", "*.pdf", "*.txt"]
    }

    Component {
        id: documentDelegate

        Item {
            Row {
                id: row
                Image {
                    id: image;
                    source: {
                        var ext = fileName.substr(fileName.lastIndexOf('.') + 1);
                        var thumbURI = "";

                        switch (ext) {
                        case "doc":
                            thumbURI = "image://themedimage/images/media/icn_ms_word"
                            break;
                        case "pdf":
                            thumbURI = "image://themedimage/images/media/icn_adobe_pdf"
                            break;
                        case "txt":
                            thumbURI = "image://themedimage/images/media/icn_plain_txt"
                            break;
                        }

                        return thumbURI;
                    }
                    width: 50;
                    height: 50;
                }
                Text  {
                    id: filename;
                    text: model.fileName;
                    anchors.verticalCenter: image.verticalCenter;
                    font.pixelSize: theme.fontPixelSizeLargest;
                }
                states: [
                    State {
                        name: "selected"
                        when: (index==selectedFileIndex)
                        PropertyChanges {target: filename; color: "red"}
                    }
                ]
            }
            MouseArea {
                anchors.fill: row
                onClicked: {
                    selectedFileIndex = index;
                    selectedFileName = fileName;
                }
            }
        }
    }

    property string selectedFileName: ""
    property int selectedFileIndex: -1

    content: GridView {
        width: 200
        height: cellHeight*count
        cellWidth: width
        cellHeight: 50
        flow: GridView.TopToBottom

        model: documentListModel
        delegate: documentDelegate
    }

    Item { id: topItem }
    Theme { id: theme }
}
