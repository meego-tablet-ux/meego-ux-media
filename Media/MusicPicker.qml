/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1
import MeeGo.Media 0.1

/* Music Picker
 * Displays a list of albums, playlist and and prompts to select photo from
 * one of the albums.  If photo selected OK is enabled which triggers
 * the songSelected signal.
 *
 * Properties:
 *   selectAlbumOrPlaylistOnly - if true, selects albums and Playlists instead of fotos.
 *
 * Signal:
 *   songSelected - this is not triggered on cancel
 *      string - name of that song
 *      string - albumOrPlaylist
 *      int - type
 *   albumOrPlaylistSelected - triggered if selectAlbumOrPlaylistOnly is true
 *      albumOrPlaylist - string of the selected photo album
 *      int - type
 *   closed // from Fog
 *   opened // from Fog
 *   cancel
 */

Fog {
    id: musicPicker

    property int imageWidthHint: inner.imageWidthHint //ckra not needed
    property alias promptText:  inner.promptText
    property alias okLabel: inner.okLabel
    property alias cancelLabel: inner.cancelLabel
    property alias selectAlbumOrPlaylistOnly: inner.selectAlbumOrPlaylistOnly
    property alias showAlbums: inner.showAlbums
    property alias showPlaylists: inner.showPlaylists

    signal songSelected(string title, string uri)
    signal albumOrPlaylistSelected (string albumOrPlaylist, int type)
    signal cancel
    signal newPlaylist()

    closeOnFogClick: false
    autoCenter: true
    z: 50000; //FIXME in music app

    modalSurface: MusicPickerRect { //ckra TODO MusicPickerRect could go right here

        id: inner
        visible: true

        onSongSelected: {
            if (title == undefined)
                musicPicker.cancel()
            else
                musicPicker.songSelected(title, uri)
             musicPicker.forceClose()
        }
        onAlbumOrPlaylistSelected: {
            if (albumOrPlaylist == undefined)
                musicPicker.cancel()
            else
                musicPicker.albumOrPlaylistSelected (albumOrPlaylist, type)
            musicPicker.forceClose()
        }
        onCancelSongSelection:  {
            musicPicker.cancel()
            musicPicker.forceClose()
        }
        onNewPlaylist:  {
            musicPicker.newPlaylist();
        }
    }
}
