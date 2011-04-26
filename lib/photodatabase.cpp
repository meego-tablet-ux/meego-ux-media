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
#include "photodatabase.h"

#define TRACKER_ALBUMPHOTOS "SELECT ?image " \
        "WHERE { ?imagelist nfo:hasMediaFileListEntry ?entry . ?entry nfo:entryUrl ?image " \
        "{ SELECT ?imagelist WHERE {?imagelist a nmm:ImageList ; nie:title '%1'} } }"
#define COVERARTTAG "INSERT { _:tag a nao:Tag ; nao:prefLabel '%2' ; nao:identifier 'coverart' . ?object nao:hasTag _:tag } " \
        "WHERE { ?object a nie:InformationElement . FILTER (nie:title(?object) = '%1') }"
#define GCONFKEY_RECENTRANGE "/MeeGo/Media/lastviewed-range-photo"

PhotoDatabase *PhotoDatabase::photoDatabaseInstance = NULL;

PhotoDatabase *PhotoDatabase::instance()
{
    if (photoDatabaseInstance)
        return photoDatabaseInstance;
    else {
        photoDatabaseInstance = new PhotoDatabase();
        return photoDatabaseInstance;
    }
}

PhotoDatabase::PhotoDatabase(QObject *parent)
    : MediaDatabase(GCONFKEY_RECENTRANGE, parent)
{
    trackeritems = 500;
    trackerindex = 0;
    targetitemtype = MediaItem::PhotoAlbumItem;

    connect(this,SIGNAL(photoItemAdded(int)),this,SLOT(trackerPhotoAdded(int)));
    connect(this,SIGNAL(photoAlbumItemAdded(int)),this,SLOT(trackerAlbumAdded(int)));
    connect(&thumb,SIGNAL(success(const MediaItem *)),this,SLOT(thumbReady(const MediaItem *)));

    qDebug() << "Initializing the database";
    trackerGetPhotos(trackerindex, trackeritems);
}

PhotoDatabase::~PhotoDatabase()
{
    qDebug() << "~PhotoListModel";
}

MediaItem* PhotoDatabase::getAlbumItem(const QString &title)
{
    if(title.isEmpty())
        return NULL;

    if(!albumItemsHash.contains(title))
        return NULL;

    return albumItemsHash[title];
}

/* fill in cover art */
void PhotoDatabase::processAlbum(MediaItem *album)
{
    /* if the album has cover art, skip it */
    if(!album->m_thumburi.isEmpty())
        return;

    for(int i = 0; i < album->children.count(); i++)
    {
        /* set the cover to the first child found */
        if(mediaItemsUrnHash.contains(album->children[i]))
        {
            MediaItem *item = mediaItemsUrnHash[album->children[i]];

            /* set this photo as the cover art */
            album->m_thumburi = item->m_thumburi;
            album->m_thumburi_ignore = item->m_thumburi_ignore;
            album->m_thumburi_exists = item->m_thumburi_exists;
            break;
        }
    }
}

/* add this photo as cover art to any blank albums the own it */
/* or create a virtual album if the photo is an orphan */
void PhotoDatabase::processPhoto(MediaItem *item)
{
    QList<MediaItem *> values = albumItemsHash.values();
    QStringList ids;
    bool orphan = true;

    /* check if this photo belongs to an album that has */
    /* no cover art, set it as the cover if so */
    for(int i = 0; i < values.size(); ++i)
    {
        MediaItem *album = values[i];

        for(int j = 0; j < album->children.count(); j++)
        {
            if(item->m_urn == album->children[j])
            {
                orphan = false;

                /* if the album has cover art, skip it */
                if(!album->m_thumburi.isEmpty())
                    break;

                /* set this photo as the cover art */
                album->m_thumburi = item->m_thumburi;
                album->m_thumburi_ignore = false;
                if(item->thumbExists())
                {
                    /* if the thumb is online, send a signal to update */
                    ids << album->m_id;
                    album->m_thumburi_exists = true;
                }
                break;
            }
        }
    }

    if(!ids.isEmpty())
        emit itemsChanged(ids, PhotoDatabase::Thumbnail);

    if(!orphan)
        return;

    /* this photo's an orphan, add it to a virtual album */
    QString timestring = (item->m_creationtime.isEmpty())?item->m_addedtime:item->m_creationtime;
    if(timestring.isEmpty())
        return;

    QDateTime ctime = QDateTime::currentDateTime();
    QDateTime timestamp = QDateTime::fromString(timestring, Qt::ISODate);
    QString name;

    if(timestamp.daysTo(ctime) == 0)
        name = "Today";
    else if(timestamp.daysTo(ctime) == 1)
        name = "Yesterday";
    else
        name = timestamp.toString("MMMM d, yyyy");

    MediaItem *albumitem = albumItemsHash[name];
    if(albumitem == NULL)
    {
        MediaItem *albumitem = new MediaItem(MediaItem::PhotoAlbumItem);
        albumitem->m_title = name;
        albumitem->m_tracknum = 1;
        albumitem->children << item->m_urn;
        albumitem->m_thumburi = item->m_thumburi;
        albumitem->m_thumburi_ignore = item->m_thumburi_ignore;
        albumitem->m_thumburi_exists = item->m_thumburi_exists;
        albumitem->m_addedtime = timestring;
        albumitem->m_creationtime = timestring;
        mediaItemsList << albumitem;
        albumItemsHash.insert(name, albumitem);
        mediaItemsIdHash.insert(albumitem->m_id, albumitem);
        QList<MediaItem *> newItemsList;
        newItemsList << albumitem;
        emit itemsAdded(&newItemsList);
    }
    else if(!albumitem->isVirtual())
    {
        /* if the user has an album with the same name as */
        /* a virtual album, leave this photo orphaned */
        return;
    }
    else
    {
        int reason = PhotoDatabase::Other;
        albumitem->m_tracknum++;
        albumitem->children << item->m_urn;
        if(!albumitem->m_thumburi_exists&&
           item->m_thumburi_exists)
        {
            albumitem->m_thumburi = item->m_thumburi;
            albumitem->m_thumburi_ignore = false;
            albumitem->m_thumburi_exists = true;
            reason = PhotoDatabase::Thumbnail;
        }
        ids << albumitem->m_id;
        emit itemsChanged(ids, reason);
    }
}

void PhotoDatabase::trackerGetPhotos(const int offset, const int limit)
{
    //qDebug() << "MusicDatabase::trackerGetMusic";
    QString SqlCmd;
    switch(targetitemtype) {
    case MediaItem::PhotoItem:
        SqlCmd = TRACKER_ALLPHOTOS;
        break;
    case MediaItem::PhotoAlbumItem:
        SqlCmd = TRACKER_ALLPHOTOALBUMS;
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
                     this, SLOT(trackerGetPhotosFinished(QDBusPendingCallWatcher*)));
}

void PhotoDatabase::trackerAddItems(int type, QVector<QStringList> trackerreply, bool priority)
{
    QList<MediaItem *> newItemsList;

    if(type == MediaItem::PhotoAlbumItem)
    {
        for (QVector<QStringList>::iterator i = trackerreply.begin(); i != trackerreply.end(); i++)
        {
            /* if this item is already in our list, skip it */
            if(mediaItemsUrnHash.contains((*i)[IDX_URN]))
            {
                //qDebug() << "Album already online: " << (*i)[IDX_URN];
                if(priority)
                    emit itemAvailable((*i)[IDX_URN]);
                continue;
            }

            //qDebug() << "new album: " << (*i);
            MediaItem *item = new MediaItem(MediaItem::PhotoAlbumItem, recenttime, *i);

            QString SqlCmd = TRACKER_ALBUMPHOTOS;
            QString sql = QString(SqlCmd).arg(item->m_title);
            QVector<QStringList> info;
            if(trackerCall(info, sql))
            {
                for (QVector<QStringList>::iterator j = info.begin(); j != info.end(); j++)
                {
                    QString urn = (*j)[IDX_URN];
                    if(!urn.isEmpty())
                        item->children << urn;
                }
            }

            mediaItemsList << item;
            mediaItemsUrnHash.insert(item->m_urn, item);
            mediaItemsIdHash.insert(item->m_id, item);
            mediaItemsSidHash.insert(item->m_sid, item);
            albumItemsHash.insert(item->m_title, item);
            processAlbum(item);

            /* tell the world we have new data */
            itemAdded(item);

            /* if this data was specifically requested, send an alert */
            if(priority)
                emit itemAvailable(item->m_urn);
        }
    }
    else if(type == MediaItem::PhotoItem)
    {
        for (QVector<QStringList>::iterator i = trackerreply.begin(); i != trackerreply.end(); i++)
        {
            /* if this item is already in our list, skip it */
            if(mediaItemsUrnHash.contains((*i)[IDX_URN]))
            {
                //qDebug() << "Photo already online: " << (*i)[IDX_URN];
                if(priority)
                    emit itemAvailable((*i)[IDX_URN]);
                continue;
            }

            QString url = (QUrl::fromEncoded((*i)[IDX_PHO_URI].toAscii())).toLocalFile();
            QString mimetype = (*i)[IDX_PHO_MIME];

            /* only create photoItems for files that exist */
            /* and are of type image */
            if (QFile::exists(url)&&(mimetype.startsWith("image")))
            {
                //qDebug() << "new photo: " << (*i);
                MediaItem *item = new MediaItem(MediaItem::PhotoItem, recenttime, *i);
                newItemsList << item;
                mediaItemsList << item;
                mediaItemsUrnHash.insert(item->m_urn, item);
                mediaItemsIdHash.insert(item->m_id, item);
                mediaItemsSidHash.insert(item->m_sid, item);
                processPhoto(item);
           }
        }
        thumb.queueRequests(newItemsList);

        /* tell the world we have new data */
        emit itemsAdded(&newItemsList);

        /* if this data was specifically requested, send an alert */
        if(priority)
        {
            for(int i = 0; i < newItemsList.count(); i++)
                emit itemAvailable(newItemsList[i]->m_urn);
        }
    }
}

void PhotoDatabase::trackerGetPhotosFinished(QDBusPendingCallWatcher *call)
{
     QDBusPendingReply<QVector<QStringList> > reply = *call;

     /* error for this type, goto next type */
     if (reply.isError()) {
         qDebug() << "QDBus Error: " << reply.error().message();
         if(targetitemtype == MediaItem::PhotoAlbumItem)
         {
             targetitemtype = MediaItem::PhotoItem;
             trackerindex = 0;
             trackerGetPhotos(trackerindex, trackeritems);
         }
         return;
     }

     QVector<QStringList> photos = reply.value();

     /* no more data for this type, goto next type */
     if(photos.isEmpty())
     {
         if(targetitemtype == MediaItem::PhotoAlbumItem)
         {
             targetitemtype = MediaItem::PhotoItem;
             trackerindex = 0;
             trackerGetPhotos(trackerindex, trackeritems);
         }
         return;
     }

     trackerAddItems(targetitemtype, photos);

     /* generate the thumbnails after the items have been sent out */
     if(targetitemtype == MediaItem::PhotoItem)
         thumb.startLoop();

     /* go get more from tracker */
     trackerindex += trackeritems;
     trackerGetPhotos(trackerindex, trackeritems);
}

void PhotoDatabase::trackerPhotoAdded(int sid)
{
    qDebug() << "trackerPhotoAdded " << sid;
    QString SqlCmd = TRACKER_PHOTO_SID;
    QString sql = QString(SqlCmd).arg(sid);

    QVector<QStringList> info;
    if(trackerCall(info, sql))
        trackerAddItems(MediaItem::PhotoItem, info);
}

void PhotoDatabase::trackerAlbumAdded(int sid)
{
    qDebug() << "trackerPhotoAlbumAdded " << sid;
    QString SqlCmd = TRACKER_PHOTOALBUM_SID;
    QString sql = QString(SqlCmd).arg(sid);

    QVector<QStringList> info;
    if(trackerCall(info, sql))
        trackerAddItems(MediaItem::PhotoAlbumItem, info);
}

void PhotoDatabase::saveAlbum(QList<MediaItem *> &list, const QString &title)
{
    /* see if the album already exists */
    MediaItem *item = NULL;
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->isPhotoAlbum()&&(mediaItemsList[i]->m_title == title))
            item = mediaItemsList[i];

    QString SqlCmd, sql;

    /* if this is an overwrite of an existing album, delete it first */
    if(item != NULL)
    {
        SqlCmd = "DELETE { ?album a nmm:ImageList } WHERE { ?album a nmm:ImageList . FILTER (nie:title(?album) = '%1')} ";
        sql = QString(SqlCmd).arg(title);
    }

    /* create a new album */
    QString SqlInsertBegin = "INSERT { _:a a nmm:ImageList; nie:title '%1' ; nfo:entryCounter %2 ";
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
        if(!item->m_thumburi.isEmpty())
        {
            QString SqlTag = COVERARTTAG;
            sql += QString(SqlTag).arg(title).arg(item->m_thumburi);
        }
        else if(list.count() > 0)
        {
            /* set the first image as the cover art by default */
            item->m_thumburi = list[0]->m_thumburi;
            item->m_thumburi_exists = list[0]->m_thumburi_exists;
            item->m_thumburi_ignore = list[0]->m_thumburi_ignore;
            QString SqlTag = COVERARTTAG;
            sql += QString(SqlTag).arg(title).arg(item->m_thumburi);
            QStringList ids;
            ids << item->m_id;
            ids << list[0]->m_id;
            emit itemsChanged(ids, PhotoDatabase::Thumbnail);
        }
    }

    /* delete/insert the list */
    trackerCall(sql);
    SqlCmd = TRACKER_PHOTOALBUM_TITLE;
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
                mediaItemsUrnHash.insert(item->m_urn, item);
                mediaItemsSidHash.insert(item->m_sid, item);
                item->children.clear();
                for (int k = 0; k < list.count(); k++)
                    item->children << list[k]->m_urn;
                QStringList temp;
                temp << item->m_id;
                emit itemsChanged(temp, PhotoDatabase::Contents);
            }
        }
        else
        {
            trackerAddItems(MediaItem::PhotoAlbumItem, info);
        }
    }
}

QList<MediaItem *> PhotoDatabase::loadAlbum(const QString &title)
{
    QList<MediaItem *> albumItems;

    if(!albumItemsHash.contains(title))
        return albumItems;

    QStringList photouris = albumItemsHash[title]->children;
    for(int i = 0; i < photouris.count(); i++)
        if(mediaItemsUrnHash.contains(photouris[i]))
            albumItems << mediaItemsUrnHash[photouris[i]];

    return albumItems;
}

void PhotoDatabase::setCoverArt(const QString &title, const QString &thumburi)
{
    QString SqlCmd =
        "DELETE {?tag a rdfs:Resource } WHERE {?object nao:hasTag ?tag . ?tag nao:identifier 'coverart' " \
        ". { SELECT ?object WHERE {?object a nie:InformationElement . FILTER (nie:title(?object) = '%1') }}} " \
        "INSERT { _:tag a nao:Tag ; nao:prefLabel '%2' ; nao:identifier 'coverart' . ?object nao:hasTag _:tag }" \
        "WHERE { ?object a nie:InformationElement . FILTER (nie:title(?object) = '%1') }";

    QString sql = QString(SqlCmd).arg(title, thumburi);
    trackerCallAsync(sql);
}

/* high priority request from the view itself */
void PhotoDatabase::requestThumbnail(MediaItem *item)
{
    thumb.requestImmediate(item);
}

void PhotoDatabase::thumbReady(const MediaItem *item)
{
    QStringList temp;
    temp << item->m_id;
    emit itemsChanged(temp, PhotoDatabase::Thumbnail);
}

void PhotoDatabase::destroyItem(MediaItem *item)
{
    mediaItemsList.removeAll(item);
    if(item->m_type == MediaItem::PhotoAlbumItem)
    {
        QString SqlCmd = "DELETE { ?album a nmm:ImageList } WHERE { ?album a nmm:ImageList . FILTER (nie:title(?album) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_title);
        trackerCallAsync(sql);
        albumItemsHash.remove(item->m_title);
    }
    else if(item->m_type == MediaItem::PhotoItem)
    {
        QString SqlCmd = "DELETE { ?item a nmm:Photo } WHERE { ?item a nmm:Photo . FILTER (str(?item) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);
        QUrl url = QUrl::fromEncoded(item->m_uri.toAscii());
        qDebug() << "delete " << url.toLocalFile();
        /* Are you sure? (y/n) */
        QFile f(url.toLocalFile());
        f.remove();
        mediaItemsUrnHash.remove(item->m_urn);
        mediaItemsSidHash.remove(item->m_sid);
        /* save off all albums that had this image */
        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            if(mediaItemsList[i]->isPhotoAlbum()&&
               mediaItemsList[i]->children.contains(item->m_urn))
            {
                mediaItemsList[i]->children.removeAll(item->m_urn);
                QList<MediaItem *> albumItems;
                QStringList photouris = mediaItemsList[i]->children;
                for(int j = 0; j < photouris.count(); j++)
                    if(mediaItemsUrnHash.contains(photouris[j]))
                        albumItems << mediaItemsUrnHash[photouris[j]];
                saveAlbum(albumItems, mediaItemsList[i]->m_title);
            }
        }
    }

    QStringList ids;
    ids << item->m_id;
    emit itemsRemoved(ids);
}

void PhotoDatabase::requestItem(int type, QString identifier)
{
    if((type != MediaItem::PhotoItem)&&(type != MediaItem::PhotoAlbumItem))
        return;

    if(identifier.startsWith("urn:"))
    {
        if(mediaItemsUrnHash.contains(identifier))
        {
            emit itemAvailable(identifier);
            return;
        }
    }
    else if(type == MediaItem::PhotoItem)
    {
        QString uri = (QUrl::fromEncoded(identifier.toAscii())).toString();
        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            MediaItem *m = mediaItemsList[i];
            if(m->isPhoto()&&(m->m_uri == uri))
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

    QString SqlCmd;
    if(type == MediaItem::PhotoItem)
    {
        if(identifier.startsWith("urn:"))
            SqlCmd = TRACKER_PHOTO;
        else
            SqlCmd = TRACKER_PHOTO_URL;
    }
    else
        SqlCmd = TRACKER_PHOTOALBUM;
    QString sql = QString(SqlCmd).arg(identifier);
    QVector<QStringList> info;

    if(trackerCall(info, sql))
    {
        trackerAddItems(type, info, true);
        return;
    }

    /* tracker doesn't have this item */
    if(!identifier.startsWith("urn:")&&(type == MediaItem::PhotoItem)&&
       fileExists(identifier))
    {
        QList<MediaItem *> newItemsList;
        MediaItem *item = new MediaItem(MediaItem::PhotoItem, identifier);
        newItemsList << item;
        mediaItemsIdHash.insert(item->m_id, item);
        mediaItemsList += newItemsList;
        emit itemsAdded(&newItemsList);
        emit itemAvailable(item->m_id);
    }
}
