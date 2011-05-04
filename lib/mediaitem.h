/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MEDIAITEM_H
#define MEDIAITEM_H

#include <QtCore/QtCore>
#include <QtCore/QObject>

/* indexes/commands for all the properties we get from tracker for each of */
/* the seven types of tracker element we retrieve */

/* GLOBAL */

#define IDX_URN          0
#define IDX_SUBJECTID    1
#define IDX_ADDTIME      2
#define IDX_VIEWED       3
#define IDX_FAVORITE     4
#define IDX_CUSTOM_BEGIN 5

/* PHOTO */

#define TRACKER_ALLPHOTOS  "SELECT ?photo tracker:id(?photo) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) " \
        "nie:url(nie:isStoredAs(?photo)) nie:contentCreated(?photo) nie:mimeType(?photo) nie:title(?photo) " \
        "nfo:horizontalResolution(?photo) nfo:verticalResolution(?photo) " \
        "nie:comment(?photo) WHERE{?photo a nmm:Photo;tracker:added ?_added " \
        ". OPTIONAL {?photo nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?photo nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} " \
        "ORDER BY desc(?_added) LIMIT %1 OFFSET %2"
#define TRACKER_PHOTO  "SELECT ?photo tracker:id(?photo) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) " \
        "nie:url(nie:isStoredAs(?photo)) nie:contentCreated(?photo) nie:mimeType(?photo) nie:title(?photo)" \
        "nfo:horizontalResolution(?photo) nfo:verticalResolution(?photo) " \
        "nie:comment(?photo) WHERE{?photo a nmm:Photo;tracker:added ?_added " \
        ". FILTER (str(?photo) = '%1') " \
        ". OPTIONAL {?photo nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?photo nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_PHOTO_URL  "SELECT ?photo tracker:id(?photo) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) " \
        "nie:url(nie:isStoredAs(?photo)) nie:contentCreated(?photo) nie:mimeType(?photo) nie:title(?photo)" \
        "nfo:horizontalResolution(?photo) nfo:verticalResolution(?photo) " \
        "nie:comment(?photo) WHERE{?photo a nmm:Photo;tracker:added ?_added " \
        ". FILTER (nie:url(?photo) = '%1') " \
        ". OPTIONAL {?photo nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?photo nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_PHOTO_SID  "SELECT ?photo tracker:id(?photo) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) " \
        "nie:url(nie:isStoredAs(?photo)) nie:contentCreated(?photo) nie:mimeType(?photo) nie:title(?photo)" \
        "nfo:horizontalResolution(?photo) nfo:verticalResolution(?photo) " \
        "nie:comment(?photo) WHERE{?photo a nmm:Photo;tracker:added ?_added " \
        ". FILTER (tracker:id(?photo) = '%1') " \
        ". OPTIONAL {?photo nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?photo nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"

#define IDX_PHO_URI   IDX_CUSTOM_BEGIN
#define IDX_PHO_CTIME IDX_CUSTOM_BEGIN + 1
#define IDX_PHO_MIME  IDX_CUSTOM_BEGIN + 2
#define IDX_PHO_TITLE IDX_CUSTOM_BEGIN + 3
#define IDX_PHO_WIDTH IDX_CUSTOM_BEGIN + 4
#define IDX_PHO_HEIGHT IDX_CUSTOM_BEGIN + 5
#define IDX_PHO_USER  IDX_CUSTOM_BEGIN + 6
#define PHO_ARGS      IDX_PHO_USER + 1

/* PHOTO ALBUM */

#define TRACKER_ALLPHOTOALBUMS "SELECT ?album tracker:id(?album) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag)  " \
        "nao:prefLabel(?tag3) nie:title(?album) WHERE{?album a nmm:ImageList;tracker:added ?_added " \
        ". OPTIONAL {?album nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag3 . ?tag3 nao:identifier 'coverart' . }} " \
        "ORDER BY desc(?_added) LIMIT %1 OFFSET %2"
#define TRACKER_PHOTOALBUM  "SELECT ?album tracker:id(?album) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nao:prefLabel(?tag3) " \
        "nie:title(?album) WHERE{?album a nmm:ImageList;tracker:added ?_added " \
        ". FILTER (str(?album) = '%1') " \
        ". OPTIONAL {?album nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag3 . ?tag3 nao:identifier 'coverart' . }} "
#define TRACKER_PHOTOALBUM_TITLE  "SELECT ?album tracker:id(?album) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nao:prefLabel(?tag3) " \
        "nie:title(?album) WHERE{?album a nmm:ImageList;tracker:added ?_added " \
        ". FILTER (nie:title(?album) = '%1') " \
        ". OPTIONAL {?album nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag3 . ?tag3 nao:identifier 'coverart' . }} "
#define TRACKER_PHOTOALBUM_SID  "SELECT ?album tracker:id(?album) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nao:prefLabel(?tag3) " \
        "nie:title(?album) WHERE{?album a nmm:ImageList;tracker:added ?_added " \
        ". FILTER (tracker:id(?album) = '%1') " \
        ". OPTIONAL {?album nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag3 . ?tag3 nao:identifier 'coverart' . }} "

#define IDX_PAL_COVER IDX_CUSTOM_BEGIN
#define IDX_PAL_TITLE IDX_CUSTOM_BEGIN + 1
#define PAL_ARGS      IDX_PAL_TITLE + 1

/* SONG */

#define TRACKER_ALLSONGS  "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(nmm:performer(?song)) nmm:performer(?song) nmm:albumTitle(nmm:musicAlbum(?song)) " \
        "nmm:musicAlbum(?song) nie:mimeType(?song) nfo:encodedBy(?song) WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} " \
        "ORDER BY nie:title(?song) LIMIT %1 OFFSET %2"
#define TRACKER_SONG  "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(nmm:performer(?song)) nmm:performer(?song) nmm:albumTitle(nmm:musicAlbum(?song)) " \
        "nmm:musicAlbum(?song) nie:mimeType(?song) nfo:encodedBy(?song) WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". FILTER (str(?song) = '%1') " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} "
#define TRACKER_SONG_URL  "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(nmm:performer(?song)) nmm:performer(?song) nmm:albumTitle(nmm:musicAlbum(?song)) " \
        "nmm:musicAlbum(?song) nie:mimeType(?song) nfo:encodedBy(?song) WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". FILTER (nie:url(?song) = '%1') " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} "
#define TRACKER_SONG_SID  "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(nmm:performer(?song)) nmm:performer(?song) nmm:albumTitle(nmm:musicAlbum(?song)) " \
        "nmm:musicAlbum(?song) nie:mimeType(?song) nfo:encodedBy(?song) WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". FILTER (tracker:id(?song) = '%1') " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} "
#define TRACKER_SONGS_BYALBUM  "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(?artist) ?artist nmm:albumTitle(?album) ?album nie:mimeType(?song) nfo:encodedBy(?song) " \
        "WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". ?song nmm:musicAlbum ?album . ?song nmm:performer ?artist " \
        ". FILTER (str(?album) = '%1') " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} " \
        "ORDER BY nmm:trackNumber(?song)"
#define TRACKER_SONGS_BYARTIST  "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(?artist) ?artist nmm:albumTitle(?album) ?album nie:mimeType(?song) nfo:encodedBy(?song) " \
        "WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". ?song nmm:musicAlbum ?album . ?song nmm:performer ?artist " \
        ". FILTER (str(?artist) = '%1') " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} " \
        "ORDER BY nmm:albumTitle(?album) nmm:trackNumber(?song)"
#define TRACKER_SONGS_BYPLAYLIST "SELECT ?song tracker:id(?song) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?song) " \
        "nie:url(nie:isStoredAs(?song)) nie:contentCreated(?song) nmm:trackNumber(?song) nfo:duration(?song) " \
        "nmm:artistName(?artist) ?artist nmm:albumTitle(?album) ?album nie:mimeType(?song) nfo:encodedBy(?song) " \
        "WHERE{?song a nmm:MusicPiece;tracker:added ?_added " \
        ". ?song nmm:musicAlbum ?album . ?song nmm:performer ?artist " \
        ". FILTER (str(?song) = ?item) " \
        ". OPTIONAL {?song nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?song nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . } " \
        ". { SELECT ?item WHERE { ?playlist nfo:hasMediaFileListEntry ?entry . ?entry nfo:entryUrl ?item " \
        ". ?entry nfo:listPosition ?index { SELECT ?playlist WHERE {?playlist a nmm:Playlist . " \
        "FILTER (str(?playlist) = '%1')} } } " \
        "ORDER BY ?index }}"

#define IDX_SNG_TITLE      IDX_CUSTOM_BEGIN
#define IDX_SNG_URI        IDX_CUSTOM_BEGIN + 1
#define IDX_SNG_CTIME      IDX_CUSTOM_BEGIN + 2
#define IDX_SNG_TRACK      IDX_CUSTOM_BEGIN + 3
#define IDX_SNG_DURATION   IDX_CUSTOM_BEGIN + 4
#define IDX_SNG_ARTISTNAME IDX_CUSTOM_BEGIN + 5
#define IDX_SNG_ARTISTURN  IDX_CUSTOM_BEGIN + 6
#define IDX_SNG_ALBUMNAME  IDX_CUSTOM_BEGIN + 7
#define IDX_SNG_ALBUMURN   IDX_CUSTOM_BEGIN + 8
#define IDX_SNG_MIME       IDX_CUSTOM_BEGIN + 9
#define IDX_SNG_USER       IDX_CUSTOM_BEGIN + 10
#define SNG_ARGS           IDX_SNG_USER + 1

/* MUSIC PLAYLIST */

#define TRACKER_ALLPLAYLISTS "SELECT ?playlist tracker:id(?playlist) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?playlist) " \
        "nfo:entryCounter(?playlist) WHERE{?playlist a nmm:Playlist;tracker:added ?_added " \
        ". OPTIONAL {?playlist nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?playlist nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }} " \
        "ORDER BY nie:title(?playlist) LIMIT %1 OFFSET %2"
#define TRACKER_PLAYLIST "SELECT ?playlist tracker:id(?playlist) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?playlist) " \
        "nfo:entryCounter(?playlist) WHERE{?playlist a nmm:Playlist;tracker:added ?_added " \
        ". FILTER (str(?playlist) = '%1') " \
        ". OPTIONAL {?playlist nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?playlist nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_PLAYLIST_SID "SELECT ?playlist tracker:id(?playlist) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?playlist) " \
        "nfo:entryCounter(?playlist) WHERE{?playlist a nmm:Playlist;tracker:added ?_added " \
        ". FILTER (tracker:id(?playlist) = '%1') " \
        ". OPTIONAL {?playlist nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?playlist nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_PLAYLIST_TITLE "SELECT ?playlist tracker:id(?playlist) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nie:title(?playlist) " \
        "nfo:entryCounter(?playlist) WHERE{?playlist a nmm:Playlist;tracker:added ?_added " \
        ". FILTER (nie:title(?playlist) = '%1') " \
        ". OPTIONAL {?playlist nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?playlist nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"

#define IDX_MPL_TITLE IDX_CUSTOM_BEGIN
#define IDX_MPL_COUNT IDX_CUSTOM_BEGIN + 1
#define MPL_ARGS      IDX_MPL_COUNT + 1

/* MUSIC ALBUM */

#define TRACKER_MUSICALBUM "SELECT ?album tracker:id(?album) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nmm:albumTitle(?album) " \
        "nmm:albumTrackCount(?album) nmm:artistName(?artist) ?artist " \
        "WHERE{?album a nmm:MusicAlbum;tracker:added ?_added . ?album nmm:albumArtist ?artist " \
        ". FILTER (str(?album) = '%1') " \
        ". OPTIONAL {?album nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_MUSICALBUM_SID "SELECT ?album tracker:id(?album) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nmm:albumTitle(?album) " \
        "nmm:albumTrackCount(?album) nmm:artistName(nmm:albumArtist(?album)) nmm:albumArtist(?album) " \
        "WHERE{?album a nmm:MusicAlbum;tracker:added ?_added " \
        ". FILTER (tracker:id(?album) = '%1') " \
        ". OPTIONAL {?album nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?album nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"

#define IDX_MAL_TITLE      IDX_CUSTOM_BEGIN
#define IDX_MAL_COUNT      IDX_CUSTOM_BEGIN + 1
#define IDX_MAL_ARTISTNAME IDX_CUSTOM_BEGIN + 2
#define IDX_MAL_ARTISTURN  IDX_CUSTOM_BEGIN + 3
#define MAL_ARGS           IDX_MAL_ARTISTURN + 1

/* MUSIC ARTIST */

#define TRACKER_ARTIST "SELECT ?artist tracker:id(?artist) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nmm:artistName(?artist) " \
        "WHERE{?artist a nmm:Artist;tracker:added ?_added " \
        ". FILTER (str(?artist) = '%1') " \
        ". OPTIONAL {?artist nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?artist nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_ARTIST_SID "SELECT ?artist tracker:id(?artist) ?_added nao:prefLabel(?tag2) nao:prefLabel(?tag) nmm:artistName(?artist) " \
        "WHERE{?artist a nmm:Artist;tracker:added ?_added " \
        ". FILTER (tracker:id(?artist) = '%1') " \
        ". OPTIONAL {?artist nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?artist nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"

#define IDX_ART_NAME IDX_CUSTOM_BEGIN
#define ART_ARGS     IDX_ART_NAME + 1

/* VIDEO */

#define TRACKER_ALLVIDEOS "SELECT ?video tracker:id(?video) ?_added nao:prefLabel(?tagv) nao:prefLabel(?tagf) " \
        "nie:url(nie:isStoredAs(?video)) nie:title(?video) nfo:duration(?video) nie:mimeType(?video) " \
        "nie:comment(?video) WHERE{?video a nmm:Video;tracker:added ?_added " \
        ". OPTIONAL {?video nao:hasTag ?tagf . ?tagf nao:identifier 'favorite' } " \
        ". OPTIONAL {?video nao:hasTag ?tagv . ?tagv nao:identifier 'viewed' . }} " \
        "ORDER BY nie:url(nie:isStoredAs(?video)) LIMIT %1 OFFSET %2";
#define TRACKER_VIDEO  "SELECT ?video tracker:id(?video) ?_added nao:prefLabel(?tagv) nao:prefLabel(?tagf) " \
        "nie:url(nie:isStoredAs(?video)) nie:title(?video) nfo:duration(?video) nie:mimeType(?video) " \
        "nie:comment(?video) WHERE{?video a nmm:Video;tracker:added ?_added " \
        ". FILTER (str(?video) = '%1') " \
        ". OPTIONAL {?video nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?video nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_VIDEO_URL  "SELECT ?video tracker:id(?video) ?_added nao:prefLabel(?tagv) nao:prefLabel(?tagf) " \
        "nie:url(nie:isStoredAs(?video)) nie:title(?video) nfo:duration(?video) nie:mimeType(?video) " \
        "nie:comment(?video) WHERE{?video a nmm:Video;tracker:added ?_added " \
        ". FILTER (nie:url(?video) = '%1') " \
        ". OPTIONAL {?video nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?video nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"
#define TRACKER_VIDEO_SID  "SELECT ?video tracker:id(?video) ?_added nao:prefLabel(?tagv) nao:prefLabel(?tagf) " \
        "nie:url(nie:isStoredAs(?video)) nie:title(?video) nfo:duration(?video) nie:mimeType(?video) " \
        "nie:comment(?video) WHERE{?video a nmm:Video;tracker:added ?_added " \
        ". FILTER (tracker:id(?video) = '%1') " \
        ". OPTIONAL {?video nao:hasTag ?tag . ?tag nao:identifier 'favorite' . } " \
        ". OPTIONAL {?video nao:hasTag ?tag2 . ?tag2 nao:identifier 'viewed' . }}"

#define IDX_VID_URI        IDX_CUSTOM_BEGIN
#define IDX_VID_TITLE      IDX_CUSTOM_BEGIN + 1
#define IDX_VID_DURATION   IDX_CUSTOM_BEGIN + 2
#define IDX_VID_MIME       IDX_CUSTOM_BEGIN + 3
#define IDX_VID_USER       IDX_CUSTOM_BEGIN + 4
#define VID_ARGS           IDX_VID_USER + 1

#define TRACKER_SERVICE       "org.freedesktop.Tracker1"
#define TRACKER_PATH          "/org/freedesktop/Tracker1/Resources"
#define TRACKER_INTERFACE     "org.freedesktop.Tracker1.Resources"
#define DAYSRECENT            -7
#define FAVORITETAG "INSERT { _:tag a nao:Tag ; nao:prefLabel 'favorite' ; nao:identifier 'favorite' . ?object nao:hasTag _:tag } " \
        "WHERE { ?object a nie:InformationElement . FILTER (nie:title(?object) = '%1') }"
#define VIEWEDTAG "INSERT { _:tag a nao:Tag ; nao:prefLabel '%2' ; nao:identifier 'viewed' . ?object nao:hasTag _:tag } " \
        "WHERE { ?object a nie:InformationElement . FILTER (nie:title(?object) = '%1') }"
#define DEFAULT_FLAVOR        "large"
#define USER_CONTENT_STRING "MeeGo User Generated Content"

class MediaItem: public QObject {
    Q_OBJECT
    Q_ENUMS(Role);
    Q_ENUMS(ItemType);
    Q_PROPERTY(QString itemid READ getID);
    Q_PROPERTY(int itemtype READ getType);
    Q_PROPERTY(QString uri READ getURI);
    Q_PROPERTY(QString thumburi READ getThumbURI);
    Q_PROPERTY(QStringList thumburilist READ getThumbURIList);
    Q_PROPERTY(QString title READ getTitle);
    Q_PROPERTY(QStringList artist READ getArtist);
    Q_PROPERTY(QString album READ getAlbum);
    Q_PROPERTY(int tracknum READ getTrackNum);
    Q_PROPERTY(int length READ getLength);
    Q_PROPERTY(QString camera READ getCamera);
    Q_PROPERTY(QString creationtime READ getCreationTime);
    Q_PROPERTY(QString addedtime READ getAddedTime);
    Q_PROPERTY(QString lastplayedtime READ getLastPlayedTime);
    Q_PROPERTY(bool favorite READ getFavorite);
    Q_PROPERTY(bool recentlyadded READ getRecentlyAdded);
    Q_PROPERTY(bool recentlyviewed READ getRecentlyViewed);
    Q_PROPERTY(bool isvirtual READ isVirtual);
    Q_PROPERTY(int width READ getWidth);
    Q_PROPERTY(int height READ getHeight);

public:
    /* create a mediaitem using tracker data */
    explicit MediaItem(int type, QDateTime recenttime, QStringList args, QObject *parent = 0);
    /* create a mediaitem using just a filename */
    explicit MediaItem(int type, QString uri, QObject *parent = 0);
    /* create a completely virtual media item filled with non-tracker data */
    explicit MediaItem(int type, QObject *parent = 0);
    /* create a completely uninitialized item */
    explicit MediaItem(QObject *parent = 0);

    enum Role {
        ID = Qt::UserRole + 1,
        ItemType = Qt::UserRole + 2,
        URI = Qt::UserRole + 3,
        ThumbURI = Qt::UserRole + 4,
        AlbumThumbURI = Qt::UserRole + 5,
        ArtistThumbURI = Qt::UserRole + 6,
        Title = Qt::UserRole + 7,
        Artist = Qt::UserRole + 8,
        Album = Qt::UserRole + 9,
        TrackNum = Qt::UserRole + 10,
        Length = Qt::UserRole + 11,
        Camera = Qt::UserRole + 12,
        CreationTime = Qt::UserRole + 13,
        AddedTime = Qt::UserRole + 14,
        LastPlayedTime = Qt::UserRole + 15,
        Favorite = Qt::UserRole + 16,
        RecentlyAdded = Qt::UserRole + 17,
        RecentlyViewed = Qt::UserRole + 18,
        Index = Qt::UserRole + 19,
        Virtual = Qt::UserRole + 20,
        PlayStatus = Qt::UserRole + 21,
        URN = Qt::UserRole + 22,
        Width = Qt::UserRole + 23,
        Height = Qt::UserRole + 24,
        UserContent = Qt::UserRole + 25
    };

    enum ItemType {
        PhotoItem = 0,
        PhotoAlbumItem = 1,
        SongItem = 2,
        MusicArtistItem = 3,
        MusicAlbumItem = 4,
        MusicPlaylistItem = 5,
        VideoItem = 6,
        TVShowItem = 7,
        MusicVideoItem = 8
    };

    enum ThumbType {
        NoThumb = 0,
        PhotoThumb = 1,
        AlbumThumb = 2,
        ArtistThumb = 3,
        VideoThumb = 4,
        PlaylistThumb = 5
    };

    void changeData(QDateTime recenttime, QStringList args);

    bool isPhoto()
        { return (m_type == PhotoItem); }
    bool isAnyVideoType()
        { return ((m_type == VideoItem)||(m_type == TVShowItem)||(m_type == MusicVideoItem)); }
    bool isVideo()
        { return (m_type == VideoItem); }
    bool isTVShow()
        { return (m_type == TVShowItem); }
    bool isMusicVideo()
        { return (m_type == MusicVideoItem); }
    bool isPhotoAlbum()
        { return (m_type == PhotoAlbumItem); }
    bool isSong()
        { return (m_type == SongItem); }
    bool isMusicAlbum()
        { return (m_type == MusicAlbumItem); }
    bool isMusicArtist()
        { return (m_type == MusicArtistItem); }
    bool isMusicPlaylist()
        { return (m_type == MusicPlaylistItem); }
    bool isVirtual()
        { return m_isvirtual; }

    /* properties functions for the photo item */
    QString getURI() const
        { return m_uri; }
    QString getThumbURI() const
        { return m_thumburi; }
    QStringList getThumbURIList() const
        { return m_thumburilist; }
    bool getFavorite() const
        { return m_favorite; }
    bool getRecentlyAdded() const
        { return m_recentlyadded; }
    bool getRecentlyViewed() const
        { return m_recentlyviewed; }
    QString getTitle() const
        { return m_title; }
    int getType() const
        { return m_type; }
    QString getID() const
        { return m_id; }
    QStringList getArtist() const
        { return m_artist; }
    QString getAlbum() const
        { return m_album; }
    int getTrackNum() const
        { return m_tracknum; }
    int getLength() const
        { return m_length; }
    QString getCamera() const
        { return m_camera; }
    QString getCreationTime() const
        { return m_creationtime; }
    QString getAddedTime() const
        { return m_addedtime; }
    QString getLastPlayedTime() const
        { return m_lastplayedtime; }
    int getWidth() const
        { return m_width; }
    int getHeight() const
        { return m_height; }

    void setRecentlyViewed(const QString &timestamp);
    void setFavorite(const bool &favorite);
    void setCoverArt(const QString &thumburi);
    bool thumbExists(const QString &name = "");

    bool m_isvirtual;
    bool m_favorite;
    bool m_recentlyadded;
    bool m_recentlyviewed;
    QString m_title;
    QStringList m_artist;
    int m_tracknum;
    int m_length;
    int m_width;
    int m_height;
    QString m_camera;
    int m_type;
    QString m_uri;
    QString m_mimetype;
    QString m_album;
    QString m_id;
    QString m_thumburi;
    QStringList m_thumburilist;
    QString m_creationtime;
    QDateTime m_timestamp;
    QString m_addedtime;
    QString m_lastplayedtime;
    QStringList m_artist_urn;
    QString m_album_urn;
    QString m_urn;
    bool m_isusercontent;
    int m_playstatus;
    int m_sid;

    bool m_thumburi_exists;
    bool m_thumburi_ignore;
    ThumbType m_thumbtype;

    MediaItem *artistitem;
    MediaItem *albumitem;
    QStringList children;

    static QString thumbMusicAlbum(const QString &artist, const QString &album);
    static QString thumbMusicArtist(const QString &artist);
    static QString thumbPlaylist(const QString &title);
    static QString thumbPlaylistImageProvider(const QString &title);
    static QString thumbPlaylistDir();
private:
    /* internal data for the item */
    QByteArray md5Result;
    static QString stripInvalidEntities(const QString& src);
    QString thumbPhoto(const QString &uri);
    QString fileFormatted(const QString &file);
};

#endif // MEDIAITEM_H
