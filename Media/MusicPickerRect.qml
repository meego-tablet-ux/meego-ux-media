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

/* MusicPickerRect
* Displays a list of music albums or songs and prompts to select album or song from
 * one of the albums.  If song selected OK is enabled which triggers
 * the songSelected signal.
 *
 * Properties:
 *   selectAlbumOrPlaylistOnly - if true, selects albums instead of songs.
 *
 * Signal:
 *   songSelected - this is not triggered on cancel
 *      song - name of the selected song
 *      albumOrPlaylist - name of the album or the playlist
 *      int - type of the item
 *   albumOrPlaylistSelected - triggered if albumSelectionMode is true
 *      albumOrPlaylist - name of the selected album or playlist
 *      int - type of the item
 *   closed
 *   opened
 *   cancel
 */

BorderImage {

    id: musicPickerRect

    property int imageWidthHint: musicGridView.cellWidth //ckra not needed
    property bool selectAlbumOrPlaylistOnly: false
    property bool showAlbums: true
    property bool showPlaylists: true
    property string promptText: ( ( showPlaylists && showAlbums ) ? qsTr("Pick a playlist/album") :
                                  ( showPlaylists ? qsTr("Pick a playlist") :
                                    ( showAlbums ? qsTr("Pick an album") :  qsTr("Pick a song") ) ) )
    property string okLabel: qsTr("OK")
    property string cancelLabel: qsTr("Cancel")
    property string newLabel: qsTr("New Playlist")
    property variant model: ( showAlbums ? (showPlaylists ? musicAlbumsAndPlaylistsMixed : musicAlbumsOnly) :
                              (showPlaylists ? musicPlaylistsOnly : allSongs) )
    property variant currentAlbumOrPlaylist: undefined
    property int currentType: 0
    property variant currentSong: undefined
    property variant currentSongUri: undefined

    signal songSelected (string title, string uri)
    signal albumOrPlaylistSelected (string albumOrPlaylist, int type)
    signal cancelSongSelection()
    signal newPlaylist()

    function updateWidth() {
        var w = topItem.topItem.width
        var h = topItem.topItem.height
        var columns = w > h ? 5 : 2
        width = (columns * (musicGridView.cellWidth + 2)) + 15
    }

    function showAlbumsAndOrPlaylists() {
        if (currentAlbumOrPlaylist != undefined) {
            model = showAlbums ? (showPlaylists ? musicAlbumsAndPlaylistsMixed : musicAlbumsOnly)
            : (showPlaylists ? musicPlaylistsOnly : allSongs)
            currentAlbumOrPlaylist = undefined
            musicGridView.show();
            musicListView.hide();
        }
    }

    border.top: 14
    border.left: 20
    border.right: 20
    border.bottom: 20

    source: "image://theme/notificationBox_bg"
    height: topItem.topItem.height * 0.8

    TopItem {
        id: topItem
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }

    Text { //ckra to be shiftet to modalmessagebox
        id: header

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        text: musicPickerRect.promptText
        font.weight: Font.Bold
        font.pixelSize: theme_dialogTitleFontPixelSize
        color: theme_dialogTitleFontColor
    }

    MediaGridView {
        id: musicGridView

        visible: true
        selectionMode: true
        defaultThumbnail: "image://theme/media/music_thumb_med"

        anchors {
            margins: 10;
            rightMargin: 8; //ckra tbd in messagebox
            top: header.bottom;
            bottom: footer.top;
            left: parent.left;
            right: parent.right;
        }

        interactive: gridView.contentHeight > musicGridView.height
        cacheBuffer: 5000

        spacing: 2
        clip: true

        cellWidth: {
            var w = topItem.topItem.width
            var h = topItem.topItem.height
            return Math.floor((w > h ? h : w) / 3) - 2
        }
        cellHeight: cellWidth

        Component.onCompleted: { musicGridView.model = musicPickerRect.model }

        property string selectedItem

        onClicked: {
            model.setSelected(selectedItem, false)
            model.setSelected(payload.mitemid, true)
            currentType = payload.mitemtype

            if (musicPickerRect.selectAlbumOrPlaylistOnly) {
                currentAlbumOrPlaylist = payload.mtitle
            } else {
                // for now, just show all songs
                currentSong = payload.mtitle
                currentSongUri = payload.muri
            }
            selectedItem = payload.mitemid
        }
        onDoubleClicked: {
            model.setSelected(selectedItem, false)
            model.setSelected(payload.mitemid, true)
            currentType = payload.mitemtype

            if (musicPickerRect.selectAlbumOrPlaylistOnly) {
                currentAlbumOrPlaylist = payload.mtitle
                selectedItem = payload.mitemid
                albumOrPlaylistSelected(currentAlbumOrPlaylist, currentType)
            } else {
                if(currentAlbumOrPlaylist = undefined) {
                    currentAlbumOrPlaylist = payload.mtitle
                    //showSongList()
                }
            }
            selectedItem = payload.mitemid
        }
    }

    // song selection list
    ScrollableMusicList {
        id: musicListView
        visible:  false

        anchors.left: parent.left; anchors.top: header.bottom; anchors.right: parent.right
        anchors.bottom: footer.top

        model: musicPickerRect.model
    }

    Connections {
        target: musicGridView
        onCellWidthChanged: {
            musicPickerRect.updateWidth()
        }
    }

    Row { //ckra to be shiftet to modalmessagebox
        id: footer
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        height: childrenRect.height
        spacing: 18
        anchors.horizontalCenter: parent.horizontalCenter

        Button {
            id: okButton;
            title: okLabel
            active: (!selectAlbumOrPlaylistOnly && currentSong != undefined) ||
                    (selectAlbumOrPlaylistOnly && currentAlbumOrPlaylist != undefined)
            enabled: active
            width: Math.min( (musicPickerRect.width / 3) , 210 )
            height: 60
            bgSourceUp: "image://theme/btn_grey_up"
            bgSourceDn: "image://theme/btn_grey_dn"
            onClicked: {
                if (musicPickerRect.selectAlbumOrPlaylistOnly) {
                    musicPickerRect.model.setSelected( musicGridView.selectedItem, false )
                    albumOrPlaylistSelected(currentAlbumOrPlaylist, currentType)
                } else {
                    musicPickerRect.model.setSelected( musicGridView.selectedItem, false )
                    songSelected(currentSong, currentSongUri)
                }
            }
        }
        Button {
            id: cancelButton
            title: cancelLabel
            enabled: true;
            height: 60
            width: Math.min( ( musicPickerRect.width / 3) , 210 ) //ckra should be done in dialog
            bgSourceUp: "image://theme/btn_grey_up"
            bgSourceDn: "image://theme/btn_grey_dn"
            onClicked: {
                musicPickerRect.model.setSelected( musicGridView.selectedItem, false )
                cancelSongSelection()
            }
        }
        Button {
            id: newButton
            title: newLabel
            visible: (showPlaylists && !showAlbums)
            enabled: true;
            height: 60
            width: Math.min( ( musicPickerRect.width / 3) , 210 ) //ckra should be done in dialog
            bgSourceUp: "image://theme/btn_grey_up"
            bgSourceDn: "image://theme/btn_grey_dn"
            onClicked: {
                newPlaylist();
            }
        }
    }
    states: [
        State {
            name: "closed"
            when: parent.opacity == 0
            StateChangeScript { name: "resetMusicPicker"; script: showAlbumsAndOrPlaylists() }
        },
        State {
            name: "albumAndPlaylistView"
            when: musicPickerRect.currentAlbumOrPlaylist == undefined
        },
        State {
            name: "songView"
            when: musicPickerRect.currentAlbumOrPlaylist != undefined && pickerRect.currentSong == undefined
        }
    ]

    //ckra todo: remove some models if possible
    MusicListModel {
        id: allSongs
        type: MusicListModel.ListofSongs
        limit: 0
        sort: MusicListModel.SortByTitle
        mixtypes: MusicListModel.Songs
    }
    MusicListModel {
        id: musicAlbumsOnly
        type: MusicListModel.ListofAlbums
        limit: 0
        sort: MusicListModel.SortByTitle
        mixtypes: MusicListModel.Albums
    }
    MusicListModel {
        id: musicPlaylistsOnly
        type: MusicListModel.ListofPlaylists
        limit: 0
        sort: MusicListModel.SortByTitle
        mixtypes: MusicListModel.Playlists
    }
    MusicListModel {
        id: musicAlbumsAndPlaylistsMixed
        type: MusicListModel.MixedList
        limit: 0
        sort: MusicListModel.SortByTitle
        mixtypes: MusicListModel.Albums|MusicListModel.Playlists
    }
    MusicListModel {
        id: songsFromAlbum
        type: MusicListModel.ListofSongsForAlbum
        album: currentAlbumOrPlaylist == undefined ? "" : currentAlbumOrPlaylist
        limit: 0
        sort: MusicListModel.SortByDefault
    }
    MusicListModel {
        id: songsFromPlaylist
        type: MusicListModel.MusicPlaylist
        album: currentAlbumOrPlaylist == undefined ? "" : currentAlbumOrPlaylist
        playlist: currentAlbumOrPlaylist == undefined ? "" : currentAlbumOrPlaylist
        limit: 0
        sort: MusicListModel.SortByDefault
    }
}
