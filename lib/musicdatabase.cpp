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
    trackerGetMusic(trackerindex, trackeritems);
}

MusicDatabase::~MusicDatabase()
{
}

MediaItem* MusicDatabase::getArtistItem(const QString &title)
{
    if(title.isEmpty())
        return NULL;

    if(!artistItemHash.contains(title))
        return NULL;

    return artistItemHash[title];
}

MediaItem* MusicDatabase::getPlaylistItem(const QString &title)
{
    if(title.isEmpty())
        return NULL;

    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        MediaItem *m = mediaItemsList[i];
        if(m->isMusicPlaylist()&&
          (m->m_title == title))
          return m;
    }
    return NULL;
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
    /* first check to see if the song is in any playlists */
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->isMusicPlaylist())
            for(int j = 0; j < mediaItemsList[i]->children.count(); j++)
            {
                if(item->m_uri.compare(mediaItemsList[i]->children[j], Qt::CaseInsensitive) == 0)
                {
                    mediaItemsList[i]->children[j] = item->m_urn;
                    mediaItemsList[i]->m_length += item->m_length;
                    itemChanged(mediaItemsList[i], UpdatePlaylist, j);
                }
            }

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
            trackerAddItems(MediaItem::MusicArtistItem, music);
            if(mediaItemsUrnHash.contains(item->m_artist_urn.first()))
            {
                item->artistitem = mediaItemsUrnHash[item->m_artist_urn.first()];
                item->artistitem->m_tracknum++;
                item->artistitem->m_length += item->m_length;
                itemChanged(item->artistitem, Other);
            }
        }
    }

    //Now that we have the item's artistitem, we can update the albumcount for it
    if( item && item->artistitem )
    {
        if( !(item->artistitem->children.contains(item->m_album)))
            item->artistitem->children << item->m_album;
        itemChanged(item->artistitem, Other);
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
            trackerAddItems(MediaItem::MusicAlbumItem, music);
            if(mediaItemsUrnHash.contains(item->m_album_urn))
            {
                item->albumitem = mediaItemsUrnHash[item->m_album_urn];
                item->albumitem->m_tracknum++;
                item->albumitem->m_length += item->m_length;
                if(item->artistitem != NULL)
                    item->albumitem->artistitem = item->artistitem;
                itemChanged(item->albumitem, Other);
            }
        }
    }
    else if(item->m_album.isEmpty()&&!item->m_artist.isEmpty())
    {
        /* if there's no album for this song, create a virtual album */
        item->albumitem = getAlbumItem(item->m_artist.first(), "Unknown Album");
        if(item->albumitem == NULL)
        {
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
            item->albumitem->m_tracknum++;
            item->albumitem->m_length += item->m_length;
            itemChanged(item->albumitem, Other);
        }
    }
}

void MusicDatabase::trackerGetMusic(const int offset, const int limit)
{
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

         /* the item is valid, create an entry for it */
         MediaItem *item = new MediaItem(type, recenttime, *i);
         mediaItemsList << item;
         mediaItemsUrnHash.insert(item->m_urn, item);
         mediaItemsIdHash.insert(item->m_id, item);
         mediaItemsSidHash.insert(item->m_sid, item);
         if(type == MediaItem::MusicArtistItem)
             artistItemHash.insert(item->m_title, item);
         if(type == MediaItem::MusicPlaylistItem)
             loadPlaylist(item);

         /* if this is an album or artist (from a command line call) get the thumb */
         if(!disable_mediaart&&
            ((((type == MediaItem::MusicArtistItem)||(type == MediaItem::MusicAlbumItem))&&(!item->m_thumburi_exists)&&(!item->m_thumburi_ignore))||
            ((type == MediaItem::MusicAlbumItem)&&(item->m_thumbtype == MediaItem::ArtistThumb))))
         {
            needThumbList << item;
         }

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
             initialized = true;
             emit databaseInitComplete();
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
             initialized = true;
             emit databaseInitComplete();
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

QString MusicDatabase::renamePlaylist(const QString &urn, const QString &title)
{
    if(urn.isEmpty())
        return "";

    if(mediaItemsUrnHash.contains(urn))
    {
        MediaItem *item = mediaItemsUrnHash[urn];
        QString uri = item->m_uri;
        uri.replace("file://", "");
        if(item->m_title == title)
            return uri;

        QString oldthumb = MediaItem::thumbPlaylist(item->m_title);
        QString newthumb = MediaItem::thumbPlaylist(title);
        if(!item->m_title.isEmpty()&&QFile::exists(oldthumb))
            QFile::rename(oldthumb, newthumb);
        item->m_thumburi = MediaItem::thumbPlaylistImageProvider(title);

        QString uri2 = uri.mid(0, uri.lastIndexOf("/")+1) + title + ".m3u";
        QFile::rename(uri, uri2);
        item->m_uri = "file://" + uri2;
        item->m_title = title;
        itemChanged(item, Title);
        return uri2;
    }

    return "";
}

void MusicDatabase::savePlaylist(QList<MediaItem *> &list, const QString &title)
{
    createPlaylistThumb(list, title);

    /* see if the playlist already exists */
    MediaItem *item = NULL;
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->isMusicPlaylist()&&(mediaItemsList[i]->m_title == title))
            item = mediaItemsList[i];

    QString homePath = QDir::toNativeSeparators(QDir::homePath()) + "/Music/";
    QString uri;
    if(item == NULL) /* new playlist */
    {
        uri = homePath + title + ".m3u";
    }
    else /* existing playlist */
    {
        item->children.clear();
        item->m_length = 0;
        item->m_tracknum = 0;

        /* if the title has changed, rename the file */
        if(item->m_title != title)
        {
            uri = renamePlaylist(item->m_urn, title);
        }
        else
        {
            uri = item->m_uri;
            uri.replace("file://", "");
        }

        if (list.count() > 0) {
            generatePlaylistThumbId(item);
            item->m_thumburi_exists = true;
        } else {
            QFile f(MediaItem::thumbPlaylist(item->m_title));
            f.remove();
            item->m_thumburi_exists = false;
        }
    }

    QFile fp(uri);
    if (!fp.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&fp);

    out << "#EXTM3U" << endl;
    for(int i = 0; i < list.count(); i++)
    {
        QString track = list[i]->m_uri;
        QString homePath = QDir::toNativeSeparators(QDir::homePath()) + "/Music/";
        track.replace("file://", "");
        track.replace(homePath, "");
        out << track << endl;
        if(item != NULL)
        {
            item->children << list[i]->m_urn;
            item->m_length += list[i]->m_length;
            item->m_tracknum++;
        }
    }

    out.flush();
    fp.close();

    if(item != NULL)
        itemChanged(item, Contents);
}

void MusicDatabase::loadPlaylist(MediaItem *item)
{
    QStringList playlistItems;

    QString sql = QString(TRACKER_PLAYLIST_CONTENTS_BY_URN).arg(item->m_urn);
    QVector<QStringList> info;

    if(trackerCall(info, sql))
    {
        for (QVector<QStringList>::iterator j = info.begin(); j != info.end(); j++)
        {
            QString track = (*j)[0];
            if(!track.isEmpty())
                playlistItems << MediaItem::fileFormatted(track);
        }
    }

    item->children = playlistItems;
    item->m_tracknum = item->children.count();

    /* see if the songs are already loaded into memory */
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->isSong())
            for(int j = 0; j < item->children.count(); j++)
                if(mediaItemsList[i]->m_uri.compare(item->children[j], Qt::CaseInsensitive) == 0)
                {
                    item->children[j] = mediaItemsList[i]->m_urn;
                    item->m_length += mediaItemsList[i]->m_length;
                }
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
        /* let's just put whatever's already online in the playqueue */
        for(int i = 0; i < mediaItemsList.count(); i++)
            if(mediaItemsList[i]->isMusicPlaylist()&&(mediaItemsList[i]->m_urn == identifier))
                for(int j = 0; j < mediaItemsList[i]->children.count(); j++)
                    if(mediaItemsList[i]->children[j].startsWith("urn"))
                        emit songItemAvailable(mediaItemsList[i]->children[j]);
        return;
    default:
        return;
    }
    QString sql = QString(SqlCmd).arg(sparqlEscape(identifier));
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
    QString sql = QString(SqlCmd).arg(sparqlEscape(identifier));
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
