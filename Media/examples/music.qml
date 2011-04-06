/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Components 0.1
import "../"

Rectangle {
    width: 700
    height: 700

    SystemPalette { id: activePalette }
    color: activePalette.dark

    MouseArea {
        anchors.fill: parent
        onClicked: musicPicker.show()
    }



    MusicPicker {
        id: musicPicker;

//      imageWidthHint: 100
        selectAlbumOrPlaylistOnly: false
        showPlaylists:  true
        showAlbums:  true

        onSongSelected: {
            console.debug("Song selected = "+song+"  from "+(type == 4 ? "album:":"playlist: ")+albumOrPlaylist)
        }
        onAlbumOrPlaylistSelected: {
            console.debug("Selected "+(type == 4 ? "album:":"playlist: ")+albumOrPlaylist)
        }

        onCancel: console.debug("Canceled")
        onClosed: console.debug("Closed")
    }
}

