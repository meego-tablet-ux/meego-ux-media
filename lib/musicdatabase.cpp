/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QtDBus/QtDBus>
#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QPainter>
#include "musicdatabase.h"

#define DOWNLOADERPAUSE 500
#define GCONFKEY_RECENTRANGE "/MeeGo/Media/lastviewed-range-music"
#define PLAYTHUMBSIZE 400
MusicDatabase *MusicDatabase::musicDatabaseInstance = NULL;

MusicDatabase *MusicDatabase::instance()
{
    //qDebug() << "MusicDatabase::instance";
    if (musicDatabaseInstance)
        return musicDatabaseInstance;
    else {
        musicDatabaseInstance = new MusicDatabase();
        return musicDatabaseInstance;
    }
}

MusicDatabase::MusicDatabase(QObject *parent)
    : MediaDatabase(GCONFKEY_RECENTRANGE, parent)
{
    //qDebug() << "MusicDatabase::MusicDatabase";
    trackeritems = 100;
    trackerindex = 0;
    targetitemtype = MediaItem::MusicPlaylistItem;
    playlistthumbid = 0;

    if(!mediaart.supported("artistimage")&&!mediaart.supported("albumimage"))
    {
        disable_mediaart = true;
    }
    else
    {
        disable_mediaart = false;
        connect(&thumbnailerTimer, SIGNAL(timeout()), this, SLOT(startThumbnailerLoop()));
        connect(&mediaart,SIGNAL(error(QString, QString, QString, QString)),
                this,SLOT(error(QString, QString, QString, QString)));
        connect(&mediaart,SIGNAL(ready(QString, QString, QString, QString)),
                this,SLOT(ready(QString, QString, QString, QString)));
    }

    connect(this,SIGNAL(songItemAdded(int)),this,SLOT(trackerSongAdded(int)));
    connect(this,SIGNAL(playlistItemAdded(int)),this,SLOT(trackerPlaylistAdded(int)));
    qDebug() << "Initializing the database";
    trackerGetMusic(trackerindex, trackeritems);
}

MusicDatabase::~MusicDatabase()
{
    qDebug() << "~MusicDatabase";
}

MediaItem* MusicDatabase::getArtistItem(const QString &title)
{
    if(title.isEmpty())
        return NULL;

    if(!artistItemHash.contains(title))
        return NULL;

    return artistItemHash[title];
}


/* function used to ensure unique titles for all of an item type */
void MusicDatabase::enforceUniqueTitles(MediaItem *item)
{
    //qDebug() << "MusicDatabase::enforceUniqueTitles";
    if(titleCountHash.contains(item->m_title))
    {
        QString title = item->m_title;
        int count = titleCountHash[title] + 1;
        item->m_title += QString().sprintf(" (%d)", count);
        titleCountHash[title] = count;
    }
    else
    {
        titleCountHash.insert(item->m_title, 1);
    }
}

MediaItem* MusicDatabase::getAlbumItem(QString artist, QString album)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        MediaItem *m = mediaItemsList[i];
        if(m->isMusicAlbum()&&
           m->m_artist.contains(artist)&&
           (m->m_title == album))
          return m;
    }
    return NULL;
}

void MusicDatabase::processSong(MediaItem *item)
{
    /* we need to pull out the artist and album item for each song*/
    if(!item->m_artist_urn.isEmpty()&&
       mediaItemsUrnHash.contains(item->m_artist_urn.first()))
    {
        /* this song's artist has already been pulled in */
        item->artistitem = mediaItemsUrnHash[item->m_artist_urn.first()];
        item->artistitem->m_tracknum++;
        item->artistitem->m_length += item->m_length;
        itemChanged(item->artistitem, Other);
    }
    else if(!item->m_artist_urn.isEmpty()&&
       !mediaItemsUrnHash.contains(item->m_artist_urn.first()))
    {
        /* this song's artist needs to be pulled in */
        QString SqlCmd = TRACKER_ARTIST;
        QString sql = QString(SqlCmd).arg(item->m_artist_urn.first());
        QVector<QStringList> music;

        if(trackerCall(music, sql))
        {
            QVector<QStringList>::iterator i = music.begin();
            if(i != music.end())
            {
                QString title = (*i)[IDX_ART_NAME];
                if(!title.isEmpty())
                {
                    //qDebug() << "new MusicArtistItem " << (*i);
                    item->artistitem = new MediaItem(MediaItem::MusicArtistItem, recenttime, *i);
                    item->artistitem->m_tracknum++;
                    item->artistitem->m_length += item->m_length;
                    mediaItemsList << item->artistitem;

                    if(!disable_mediaart&&(!item->artistitem->m_thumburi_exists)&&
                       (!item->artistitem->m_thumburi_ignore))
                    {
                       //qDebug() << "needThumbList: added artist " << item->artistitem->m_title;
                       needThumbList << item->artistitem;
                    }

                    artistItemHash.insert(item->artistitem->m_title, item->artistitem);
                    mediaItemsUrnHash.insert(item->artistitem->m_urn, item->artistitem);
                    mediaItemsIdHash.insert(item->artistitem->m_id, item->artistitem);
                    mediaItemsSidHash.insert(item->artistitem->m_sid, item->artistitem);
                    itemAdded(item->artistitem);
                }
            }
        }
    }

    if(!item->m_album_urn.isEmpty()&&
       mediaItemsUrnHash.contains(item->m_album_urn))
    {
        /* this song's album has already been pulled in */
        item->albumitem = mediaItemsUrnHash[item->m_album_urn];
        item->albumitem->m_tracknum++;
        item->albumitem->m_length += item->m_length;
        itemChanged(item->albumitem, Other);
    }
    else if(!item->m_album_urn.isEmpty()&&
       !mediaItemsUrnHash.contains(item->m_album_urn))
    {
        /* this song's album needs to be pulled in */
        QString SqlCmd = TRACKER_MUSICALBUM;
        QString sql = QString(SqlCmd).arg(item->m_album_urn);
        QVector<QStringList> music;

        if(trackerCall(music, sql))
        {
            QVector<QStringList>::iterator i = music.begin();
            if(i != music.end())
            {
                QStringList args = (*i);
                QString title = args[IDX_MAL_TITLE];
                QString artist = args[IDX_MAL_ARTISTNAME];
                QString artist_urn = args[IDX_MAL_ARTISTURN];

                if(artist.isEmpty()&&(item->artistitem != NULL))
                    args.replace(IDX_MAL_ARTISTNAME, item->artistitem->m_title);
                if(artist_urn.isEmpty()&&(item->artistitem != NULL))
                   args.replace(IDX_MAL_ARTISTURN, item->artistitem->m_urn);

                if(!title.isEmpty())
                {
                    item->albumitem = new MediaItem(MediaItem::MusicAlbumItem, recenttime, args);
                    item->albumitem->m_tracknum++;
                    item->albumitem->m_length += item->m_length;
                    mediaItemsList << item->albumitem;

                    if(!disable_mediaart&&(((!item->albumitem->m_thumburi_exists)&&
                       (!item->albumitem->m_thumburi_ignore))||
                       (item->albumitem->m_thumbtype == MediaItem::ArtistThumb)))
                    {
                       //qDebug() << "needThumbList: added album " << item->albumitem->m_title;
                       needThumbList << item->albumitem;
                    }

                    mediaItemsUrnHash.insert(item->albumitem->m_urn, item->albumitem);
                    mediaItemsIdHash.insert(item->albumitem->m_id, item->albumitem);
                    mediaItemsSidHash.insert(item->albumitem->m_sid, item->albumitem);
                    if(item->artistitem != NULL)
                        item->albumitem->artistitem = item->artistitem;
                    itemAdded(item->albumitem);
                }
            }
        }
    }
    else if(item->m_album.isEmpty()&&!item->m_artist.isEmpty())
    {
        /* if there's no album for this song, create a virtual album */
        item->albumitem = getAlbumItem(item->m_artist.first(), "Unknown Album");
        if(item->albumitem == NULL)
        {
            //qDebug() << "Creating Unknown Album for " << item->m_artist.first() << " SONG: " << item->m_title;
            item->albumitem = new MediaItem(MediaItem::MusicAlbumItem);
            mediaItemsList << item->albumitem;
            item->albumitem->m_artist << item->m_artist.first();
            item->albumitem->m_artist_urn << item->m_artist_urn.first();
            item->albumitem->m_title = "Unknown Album";
            item->albumitem->m_tracknum = 1;
            item->albumitem->m_length = item->m_length;
            item->albumitem->artistitem = item->artistitem;
            mediaItemsIdHash.insert(item->albumitem->m_id, item->albumitem);
            itemAdded(item->albumitem);
        }
        else
        {
            //qDebug() << "Adding a song to Unknown Album for " << item->m_artist.first() << " SONG: " << item->m_title;
            item->albumitem->m_tracknum++;
            item->albumitem->m_length += item->m_length;
            itemChanged(item->albumitem, Other);
        }
    }
}

void MusicDatabase::trackerGetMusic(const int offset, const int limit)
{
    //qDebug() << "MusicDatabase::trackerGetMusic";
    QString SqlCmd;
    switch(targetitemtype) {
    case MediaItem::SongItem:
        SqlCmd = TRACKER_ALLSONGS;
        break;
    case MediaItem::MusicPlaylistItem:
        SqlCmd = TRACKER_ALLPLAYLISTS;
        break;
    default:
        return;
    }

    QString sql = QString(SqlCmd).arg(QString().sprintf("%d", limit), QString().sprintf("%d", offset));
    //qDebug() << "Tracker Query: " << sql;

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sql);
    QDBusPendingCall async = trackerinterface->asyncCallWithArgumentList(QLatin1String("SparqlQuery"), argumentList);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(trackerGetMusicFinished(QDBusPendingCallWatcher*)));
}

void MusicDatabase::trackerAddItems(int type, QVector<QStringList> trackerreply, int priority)
{
    /* organize new items into musics and albums */
    for (QVector<QStringList>::iterator i = trackerreply.begin(); i != trackerreply.end(); i++)
    {
         /* if this item is already in our list, skip it */
         if(mediaItemsUrnHash.contains((*i)[IDX_URN]))
         {
            if(priority == MainItem)
            {
                emit itemAvailable((*i)[IDX_URN]);
                break;
            }
            else if((priority == AssociatedSongs)&&(mediaItemsUrnHash[(*i)[IDX_URN]]->isSong()))
            {
                emit songItemAvailable((*i)[IDX_URN]);
            }
            continue;
         }

         /* check in case tracker returns a song with an empty uri */
         /* If this happens then this is clearly a tracker bug because */
         /* how the h@#$ did it get the song without a file?! */
         if(type == MediaItem::SongItem)
         {
             QString uri = (*i)[IDX_SNG_URI];
             if(uri.isEmpty())
                 continue;
         }
         else if(type == MediaItem::MusicArtistItem)
         {
             QString title = (*i)[IDX_ART_NAME];
             if(title.isEmpty())
                 continue;
         }
         else if(type == MediaItem::MusicAlbumItem)
         {
             QString title = (*i)[IDX_MAL_TITLE];
             if(title.isEmpty())
                 continue;
         }

         //qDebug() << "new item " << targetitemtype << " " << (*i);
         /* the item is valid, create an entry for it */
         MediaItem *item = new MediaItem(type, recenttime, *i);
         mediaItemsList << item;
         mediaItemsUrnHash.insert(item->m_urn, item);
         mediaItemsIdHash.insert(item->m_id, item);
         mediaItemsSidHash.insert(item->m_sid, item);
         if(type == MediaItem::MusicArtistItem)
             artistItemHash.insert(item->m_title, item);

         /* tell the world we have new data */
         itemAdded(item);

         /* if this data was specifically requested, send an alert */
         if(priority == MainItem)
         {
             emit itemAvailable(item->m_urn);
         }
         else if((priority == AssociatedSongs)&&(item->isSong()))
         {
             emit songItemAvailable(item->m_urn);
         }


         if(type == MediaItem::SongItem)
         {
             /* hack in case tracker doesn't give us a unique song title */
             //enforceUniqueTitles(item);
             processSong(item);
         }
    }

    /* generate the thumbnails after the items have been sent out */
    if(!disable_mediaart)
        thumbnailerTimer.start(DOWNLOADERPAUSE);
}

void MusicDatabase::trackerGetMusicFinished(QDBusPendingCallWatcher *call)
{
     QDBusPendingReply<QVector<QStringList> > reply = *call;

     /* error for this type, goto next type */
     if (reply.isError()) {
         qDebug() << "Get Music QDBus Error: " << reply.error().message();
         if(targetitemtype == MediaItem::MusicPlaylistItem)
         {
             targetitemtype = MediaItem::SongItem;
             trackerindex = 0;
             trackerGetMusic(trackerindex, trackeritems);
         }
         else
         {
             targetitemtype = -1;
         }
         return;
     }

     QVector<QStringList> music = reply.value();

     /* no more data for this type, goto next type */
     if(music.isEmpty())
     {
         if(targetitemtype == MediaItem::MusicPlaylistItem)
         {
             targetitemtype = MediaItem::SongItem;
             trackerindex = 0;
             trackerGetMusic(trackerindex, trackeritems);
         }
         else
         {
             targetitemtype = -1;
         }
         return;
     }

     trackerAddItems(targetitemtype, music);

     /* go get more from tracker */
     trackerindex += trackeritems;
     trackerGetMusic(trackerindex, trackeritems);
}

void MusicDatabase::createPlaylistThumb(QList<MediaItem *> &list, const QString &title)
{
    QStringList thumburilist;

    /* grab the first 9 viable/unique thumbs from the list */
    for(int i = 0; (i < list.count())&&(thumburilist.count() < 9); i++)
        if((list[i]->m_thumburi_exists)&&(!thumburilist.contains(list[i]->m_thumburi)))
            thumburilist << list[i]->m_thumburi;

    if(thumburilist.isEmpty())
        return;

    QPixmap thumb(PLAYTHUMBSIZE, PLAYTHUMBSIZE);
    thumb.fill(Qt::white);
    QPainter collage(&thumb);

    if(thumburilist.count() < 2)
    {
        collage.drawImage(0, 0,
            QPixmap(thumburilist[0]).scaled(PLAYTHUMBSIZE, PLAYTHUMBSIZE,
            Qt::IgnoreAspectRatio).toImage());
    }
    else if(thumburilist.count() < 4)
    {
        for(int i = 0; i < 2; i++)
        {
            collage.drawImage(0, i*(PLAYTHUMBSIZE/2),
                QPixmap(thumburilist[i]).scaled(PLAYTHUMBSIZE, PLAYTHUMBSIZE/2,
                Qt::IgnoreAspectRatio).toImage());
        }
    }
    else if(thumburilist.count() < 9)
    {
        for(int i = 0; i < 4; i++)
        {
            collage.drawImage((i%2)*(PLAYTHUMBSIZE/2), (i/2)*(PLAYTHUMBSIZE/2),
                QPixmap(thumburilist[i]).scaled(PLAYTHUMBSIZE/2, PLAYTHUMBSIZE/2,
                Qt::IgnoreAspectRatio).toImage());
        }
    }
    else
    {
        for(int i = 0; i < 9; i++)
        {
            collage.drawImage((i%3)*(PLAYTHUMBSIZE/3), (i/3)*(PLAYTHUMBSIZE/3),
                QPixmap(thumburilist[i]).scaled(PLAYTHUMBSIZE/3, PLAYTHUMBSIZE/3,
                Qt::IgnoreAspectRatio).toImage());
        }
    }

    QDir thumbdir(MediaItem::thumbPlaylistDir());
    if(!thumbdir.exists())
        thumbdir.mkpath(MediaItem::thumbPlaylistDir());
    thumb.save(MediaItem::thumbPlaylist(title));
}

void MusicDatabase::generatePlaylistThumbId(MediaItem *item)
{
    QStringList args = item->m_thumburi.split("?");
    QString thumburi = args[0] + "?" + QString::number(playlistthumbid++);
    item->m_thumburi = thumburi;
}

void MusicDatabase::savePlaylist(QList<MediaItem *> &list, const QString &title)
{
    createPlaylistThumb(list, title);

    /* see if the playlist already exists */
    MediaItem *item = NULL;
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->isMusicPlaylist()&&(mediaItemsList[i]->m_title == title))
            item = mediaItemsList[i];

    QString SqlCmd, sql;

    /* if this is an overwrite of an existing playlist, delete it first */
    if(item != NULL)
    {
        SqlCmd = "DELETE { ?playlist a nmm:Playlist } WHERE { ?playlist a nmm:Playlist . FILTER (nie:title(?playlist) = '%1')} ";
        sql = QString(SqlCmd).arg(title);
    }

    /* create a new playlist */
    QString SqlInsertBegin = "INSERT { _:a a nmm:Playlist; nie:title '%1' ; nfo:entryCounter %2 ";
    QString SqlListEntry = " nfo:hasMediaFileListEntry [ a nfo:MediaFileListEntry; nfo:entryUrl '%1'; nfo:listPosition %2 ] ";
    sql += QString(SqlInsertBegin).arg(title).arg(list.count());

    int j = 0;
    for (int  i = 0; i < list.count(); i++) {
        QString sqlItem = QString(SqlListEntry).arg(list[i]->m_urn).arg(j++);
        sql += QString(";") + sqlItem;
    }
    sql += " }";

    /* preserve the tags */
    if(item != NULL)
    {
        if(item->m_favorite)
        {
            QString SqlTag = FAVORITETAG;
            sql += QString(SqlTag).arg(title);
        }
        if(!item->m_lastplayedtime.isEmpty())
        {
            QString SqlTag = VIEWEDTAG;
            sql += QString(SqlTag).arg(title).arg(item->m_lastplayedtime);
        }
    }

    /* delete/insert the list */
    trackerCall(sql);
    SqlCmd = TRACKER_PLAYLIST_TITLE;
    sql = QString(SqlCmd).arg(title);

    QVector<QStringList> info;
    /* pull in the new list */
    if(trackerCall(info, sql))
    {
        if(item != NULL)
        {
            for (QVector<QStringList>::iterator j = info.begin(); j != info.end(); j++)
            {
                //qDebug() << "Changed Item: " << (*j);
                mediaItemsUrnHash.remove(item->m_urn);
                mediaItemsSidHash.remove(item->m_sid);
                item->changeData(recenttime, *j);
                generatePlaylistThumbId(item);
                mediaItemsUrnHash.insert(item->m_urn, item);
                mediaItemsSidHash.insert(item->m_sid, item);
                QStringList temp;
                temp << item->m_id;
                emit itemsChanged(temp, MusicDatabase::Contents);
            }
        }
        else
        {
            trackerAddItems(MediaItem::MusicPlaylistItem, info);
        }
    }
}

QStringList MusicDatabase::loadPlaylist(const QString &title)
{
    QStringList playlistItems;
    QString SqlCmd =
        "SELECT ?item WHERE { ?playlist nfo:hasMediaFileListEntry ?entry . ?entry nfo:entryUrl " \
        "?item . ?entry nfo:listPosition ?index { SELECT ?playlist WHERE {?playlist a nmm:Playlist . FILTER (nie:title(?playlist) = '%1')} } }" \
        "ORDER BY ?index";

    QString sql = QString(SqlCmd).arg(title);
    QVector<QStringList> info;

    if(trackerCall(info, sql))
    {
        for (QVector<QStringList>::iterator j = info.begin(); j != info.end(); j++)
        {
            QString urn = (*j)[0];
            if(!urn.isEmpty())
                playlistItems << urn;
        }
    }

    return playlistItems;
}

void MusicDatabase::trackerPlaylistAdded(int sid)
{
    qDebug() << "trackerPlaylistAdded " << sid;
    QString SqlCmd = TRACKER_PLAYLIST_SID;
    QString sql = QString(SqlCmd).arg(sid);

    QVector<QStringList> info;
    if(trackerCall(info, sql))
        trackerAddItems(MediaItem::MusicPlaylistItem, info);
}

void MusicDatabase::trackerSongAdded(int sid)
{
    qDebug() << "trackerSongAdded " << sid;
    QString SqlCmd = TRACKER_SONG_SID;
    QString sql = QString(SqlCmd).arg(sid);

    QVector<QStringList> info;
    if(trackerCall(info, sql))
        trackerAddItems(MediaItem::SongItem, info);
}

void MusicDatabase::error(QString reqid, QString type, QString info, QString errorString)
{
    QStringList args = info.split("|", QString::KeepEmptyParts);
    QString thumburi;
    if(type == "artistimage")
        thumburi = args[1];
    else if(type == "albumimage")
        thumburi = args[2];
    else
        return;

//    qDebug() << "Error requesting " << type << " (" << info << "): " << errorString;
    /* set ignore on any matching thumburis */
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        MediaItem *m = mediaItemsList[i];
        //qDebug() << "TESTING: " << m->m_title << ", " << m->m_thumburi;
        if((!m->m_thumburi_ignore)&&
           (!m->m_thumburi_exists)&&
           (m->m_thumburi == thumburi))
        {
            m->m_thumburi_ignore = true;
            m->m_thumburi_exists = false;
            needThumbList.removeAll(m);
//            if(m->isSong())
//                qDebug() << "SONG: " << m->m_urn << ", " << "Thumbnail FAILED";
//            else if(m->isMusicAlbum())
//                qDebug() << "ALBUM: " << m->m_urn << ", " << "Thumbnail FAILED";
//            else if(m->isMusicArtist())
//                qDebug() << "ARTIST: " << m->m_urn << ", " << "Thumbnail FAILED";
        }
    }
}

void MusicDatabase::ready(QString reqid, QString type, QString info, QString data)
{
    QStringList args = info.split("|", QString::KeepEmptyParts);
    QStringList ids;
    if(type == "albumimage")
    {
        QString album = args[1];
        QString thumburi = args[2];

        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            MediaItem *m = mediaItemsList[i];
            /* album thumbs are first priority for songs/albums */
            if(((m->isSong()&&(m->m_album == album))||
               (m->isMusicAlbum()&&(m->m_title == album)))&&
               ((m->m_thumbtype == MediaItem::ArtistThumb)||
               (!m->m_thumburi_exists)))
            {
                m->m_thumbtype = MediaItem::AlbumThumb;
                m->m_thumburi = thumburi;
                m->m_thumburi_ignore = false;
                m->m_thumburi_exists = true;
                ids << m->m_id;
                needThumbList.removeAll(m);
//                if(m->isSong())
//                    qDebug() << "SONG: " << m->m_urn << ", " << "Downloaded Album Thumbnail EXISTS";
//                else if(m->isMusicAlbum())
//                    qDebug() << "ALBUM: " << m->m_urn << ", " << "Downloaded Album Thumbnail EXISTS";
            }
        }
    }
    else if(type == "artistimage")
    {
        QString artist = args[0];
        QString thumburi = args[1];

        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            MediaItem *m = mediaItemsList[i];
            /* if matching artist */
            /* if matching song with nonexistant artist/album thumb */
            /* if matching album with nonexistant artist/album thumb */
            if((m->isMusicArtist()&&(m->m_title == artist))||
               (((m->isSong()||m->isMusicAlbum())&&(m->m_artist.contains(artist)))&&
               (((m->m_thumbtype == MediaItem::AlbumThumb)&&(!m->m_thumburi_exists))||
               ((m->m_thumbtype == MediaItem::ArtistThumb)&&(!m->m_thumburi_exists)))))
            {
                m->m_thumbtype = MediaItem::ArtistThumb;
                m->m_thumburi = thumburi;
                m->m_thumburi_ignore = false;
                m->m_thumburi_exists = true;
                ids << m->m_id;
                needThumbList.removeAll(m);
//                if(m->isSong())
//                    qDebug() << "SONG: " << m->m_urn << ", " << "Downloaded Artist Thumbnail EXISTS";
//                else if(m->isMusicAlbum())
//                    qDebug() << "ALBUM: " << m->m_urn << ", " << "Downloaded Artist Thumbnail EXISTS";
//                else if(m->isMusicArtist())
//                    qDebug() << "ARTIST: " << m->m_urn << ", " << "Downloaded Artist Thumbnail EXISTS";
            }
        }
    }
    if(!ids.isEmpty())
        emit itemsChanged(ids, MusicDatabase::Thumbnail);
}

void MusicDatabase::requestThumbnail(MediaItem *item)
{
    /* song items themselves can't be requested */
    /* try the album or artist items they're linked to */
    if(item->m_type == MediaItem::SongItem)
    {
        if(item->albumitem != NULL)
        {
            requestThumbnail(item->albumitem);
        }
        else if(item->artistitem != NULL)
        {
            requestThumbnail(item->artistitem);
        }
        else
        {
            item->m_thumburi_ignore = true;
            item->m_thumburi_exists = false;
        }
        return;
    }

    if(!needThumbList.contains(item))
        return;

    QString artist, album;

    switch(item->m_type) {
    case MediaItem::MusicArtistItem:
        artist = item->m_title;
        needThumbList.removeAll(item);
        mediaart.request(item->m_id, "artistimage",
            artist + "|" + MediaItem::thumbMusicArtist(artist));
        break;
    case MediaItem::MusicAlbumItem:
        artist = item->m_artist.first();
        album = item->m_title;
        needThumbList.removeAll(item);
        mediaart.request(item->m_id, "albumimage",
            artist + "|" + album + "|" + MediaItem::thumbMusicAlbum(artist, album));
        break;
    }
}

void MusicDatabase::startThumbnailerLoop()
{
    if(!needThumbList.isEmpty())
    {
        MediaItem *item = needThumbList.first();
        requestThumbnail(item);
    }
    else
    {
        thumbnailerTimer.stop();
    }
}

void MusicDatabase::requestSongItems(int type, QString identifier)
{
    /* get the song items for albums/artists/playlists */
    QString SqlCmd;
    switch(type) {
    case MediaItem::MusicAlbumItem:
        SqlCmd = TRACKER_SONGS_BYALBUM;
        break;
    case MediaItem::MusicArtistItem:
        SqlCmd = TRACKER_SONGS_BYARTIST;
        break;
    case MediaItem::MusicPlaylistItem:
        SqlCmd = TRACKER_SONGS_BYPLAYLIST;
        break;
    default:
        return;
    }
    QString sql = QString(SqlCmd).arg(identifier);
    QVector<QStringList> info;
    if(trackerCall(info, sql))
    {
        trackerAddItems(MediaItem::SongItem, info, AssociatedSongs);
    }
}

void MusicDatabase::requestItem(int type, QString identifier)
{
    if((type != MediaItem::SongItem)&&(type != MediaItem::MusicAlbumItem)&&
       (type != MediaItem::MusicArtistItem)&&(type != MediaItem::MusicPlaylistItem))
        return;

    /* instacheck to see if the item is already in the list */
    if(identifier.startsWith("urn:"))
    {
        if(mediaItemsUrnHash.contains(identifier))
        {
            emit itemAvailable(identifier);
            requestSongItems(type, identifier);
            return;
        }
    }
    else if(type == MediaItem::SongItem)
    {
        QString uri = (QUrl::fromEncoded(identifier.toAscii())).toString();
        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            MediaItem *m = mediaItemsList[i];
            if(m->isSong()&&(m->m_uri == uri))
            {
                emit itemAvailable(m->m_urn);
                return;
            }
        }
    }
    else
    {
        return;
    }

    /* Not in the list yet, ask tracker for it */
    QString SqlCmd;
    switch(type) {
    case MediaItem::SongItem:
        if(identifier.startsWith("urn:"))
            SqlCmd = TRACKER_SONG;
        else
            SqlCmd = TRACKER_SONG_URL;
        break;
    case MediaItem::MusicAlbumItem:
        SqlCmd = TRACKER_MUSICALBUM;
        break;
    case MediaItem::MusicArtistItem:
        SqlCmd = TRACKER_ARTIST;
        break;
    case MediaItem::MusicPlaylistItem:
        SqlCmd = TRACKER_PLAYLIST;
        break;
    }
    QString sql = QString(SqlCmd).arg(identifier);
    QVector<QStringList> info;

    if(trackerCall(info, sql))
    {
        trackerAddItems(type, info, MainItem);
        requestSongItems(type, identifier);
        return;
    }

    /* tracker doesn't have this item */
    if(!identifier.startsWith("urn:")&&(type == MediaItem::SongItem)&&
       fileExists(identifier))
    {
        QList<MediaItem *> newItemsList;
        MediaItem *item = new MediaItem(MediaItem::SongItem, identifier);
        newItemsList << item;
        mediaItemsIdHash.insert(item->m_id, item);
        mediaItemsList += newItemsList;
        emit itemsAdded(&newItemsList);
        emit itemAvailable(item->m_id);
    }
}
