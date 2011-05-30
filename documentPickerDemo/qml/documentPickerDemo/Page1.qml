import Qt 4.7
import MeeGo.Components 0.1
import MeeGo.Labs.Components 0.1

/* MeeGo-Ux-Components tutorial - step 6 */
AppPage {
    id: page1

    pageTitle: "ModalDialog Test"
    anchors.fill: parent

    Column {
	anchors.centerIn: parent
        Button {
            id: simpleButton

            width:  300
            height:  80
            text: "simple document picker"

            onClicked: {
                simpleDocumentPicker.show()
            }
        }

        Button {
            id: mediaDocumentButton

            width:  300
            height:  80
            text: "media document picker"

            onClicked: {
                mediaDocumentPicker.show()
            }
        }

        Button {
            id: mediaVideoButton

            width:  300
            height:  80
            text: "media video picker"

            onClicked: {
                mediaVideoPicker.show()
            }
        }

        Text {
            id: selectedFile

            font.pixelSize: theme.fontPixelSizeLargest
            text: "nothing selected"
        }
    }

    Theme { id: theme }

    SimpleDocumentPicker {
        id: simpleDocumentPicker

        title: "This is a SimpleDocumentPicker"

        showAcceptButton: true
        showCancelButton: true

        onSelectedFileNameChanged: {
            selectedFile.text = selectedFileName;
        }

        onAccepted:  {
            //selectedFile.text = "accepted"
        }

        onRejected: {
            //selectedFile.text = "rejected"
        }
    }

    DocumentPicker {
        id: mediaDocumentPicker

        title: "This is a media DocumentPicker"

        //showAcceptButton: true
        //showCancelButton: true

        fogClickable: false //false means clicking outside the dialog won't close it

        onDocumentSelected: {
            console.log("MAXMAXMAX/onDocumentSelected");
            // string itemid, string itemtitle, string uri, string thumbUri
            selectedFile.text = itemtitle;
        }

        onAccepted: { } // do something
        onRejected: { } // do something
    }

    VideoPicker {
        id: mediaVideoPicker

        title: "This is a media VideoPicker"

        //showAcceptButton: true
        //showCancelButton: true

        fogClickable: false //false means clicking outside the dialog won't close it

        onVideoSelected: {
            console.log("MAXMAXMAX/onVideoSelected");
            // string itemid, string itemtitle, string uri, string thumbUri
            selectedFile.text = itemtitle;
        }

        onAccepted: { } // do something
        onRejected: { } // do something
    }

}
