/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDir>
#include <QDebug>
#include "photolistmodel.h"
#include "photodatabase.h"

PhotoListModel::PhotoListModel(QObject *parent)
    : MediaListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(MediaItem::ID, "itemid");
    roles.insert(MediaItem::URN, "urn");
    roles.insert(MediaItem::ItemType, "itemtype");
    roles.insert(MediaItem::URI, "uri");
    roles.insert(MediaItem::ThumbURI, "thumburi");
    roles.insert(MediaItem::AlbumThumbURI, "albumthumburi");
    roles.insert(MediaItem::Title, "title");
    roles.insert(MediaItem::Camera, "camera");
    roles.insert(MediaItem::CreationTime, "creationtime");
    roles.insert(MediaItem::AddedTime, "addedtime");
    roles.insert(MediaItem::LastPlayedTime, "lastviewedtime");
    roles.insert(MediaItem::Favorite, "favorite");
    roles.insert(MediaItem::RecentlyAdded, "recentlyadded");
    roles.insert(MediaItem::RecentlyViewed, "recentlyviewed");
    roles.insert(MediaItem::Virtual, "isvirtual");
    roles.insert(MediaItem::Index, "index");
    roles.insert(MediaItem::Width, "width");
    roles.insert(MediaItem::Height, "height");
    roles.insert(MediaItem::UserContent, "usercontent");
    setRoleNames(roles);

    m_type = -1;
    isVirtualAlbum = false;

    connect(PhotoDatabase::instance(),SIGNAL(itemAvailable(const QString)),this,SIGNAL(itemAvailable(const QString)));
}

PhotoListModel::~PhotoListModel()
{
    clearData();
}

void PhotoListModel::connectSignals(bool added, bool changed, bool removed)
{
    if(added)
        connect(PhotoDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));

    if(changed)
        connect(PhotoDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));

    if(removed)
        connect(PhotoDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));
}

void PhotoListModel::clearData()
{
    /* don't listen to new items coming from the database, we're starting over */
    disconnect(PhotoDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));
    disconnect(PhotoDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));
    disconnect(PhotoDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));

    if(!mediaItemsDisplay.isEmpty())
    {
        /* formally remove all the items from the list */
        beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
        mediaItemsList.clear();
        mediaItemsDisplay.clear();
        endRemoveRows();
    }
}

void PhotoListModel::setType(const int type)
{
    clearData();
    m_type = type;
    m_default_sort = SortAsIs;
    hiddenURNsList.clear();
    emit typeChanged(m_type);

    /* a photoalbum can start empty, so we need signals now */
    if(m_type == PhotoAlbum)
        connectSignals(false, true, true);

    /* for these items we need further input to proceed */
    switch(m_type) {
    case PhotoAlbum:
        if(!m_album.isEmpty())
            setAlbum(m_album);
        return;
    case Editor:
        return;
    }

    /* for basic lists, grab a new database snapshot */
    QList<MediaItem *> tempList = PhotoDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    if(m_type == ListofPhotos)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isPhoto())
                newItemList << tempList[i];
    }
    else if(m_type == ListofUserPhotos)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isPhoto()&&tempList[i]->m_isusercontent)
                newItemList << tempList[i];
    }
    else if(m_type == ListofAlbums)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isPhotoAlbum())
                newItemList << tempList[i];
    }
    else if(m_type == ListofUserAlbums)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isPhotoAlbum()&&!tempList[i]->isVirtual())
                newItemList << tempList[i];
    }
    else if(m_type == ListofRecentlyViewed)
    {
        m_default_sort = SortByAccessTime;
        setSort(SortByAccessTime);
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->m_recentlyviewed&&!tempList[i]->isVirtual())
                newItemList << tempList[i];
    }
    else
    {
        qDebug() << "Invalid Type";
        return;
    }

    if(!newItemList.isEmpty())
    {
        /* formally add the new items */
        displayNewItems(newItemList);
    }

    connectSignals(true, true, true);
}

void PhotoListModel::setAlbum(const QString &album)
{
    m_album = album;
    if(m_type != PhotoAlbum)
    {
        emit albumChanged(m_album);
        return;
    }

    clearData();

    if(!album.isEmpty())
    {
        MediaItem *albumitem = PhotoDatabase::instance()->getAlbumItem(album);
        if(albumitem != NULL)
        {
            isVirtualAlbum = albumitem->isVirtual();
            if(albumitem->m_thumburi_exists)
                m_coveruri = albumitem->m_thumburi;

            QList<MediaItem *> newItemList = PhotoDatabase::instance()->loadAlbum(album);
            if(!newItemList.isEmpty())
            {
                /* formally add the new items */
                displayNewItems(newItemList);
            }
        }
    }

    connectSignals(true, true, true);
    emit albumChanged(m_album);
}

void PhotoListModel::saveAlbum(const QString &album)
{
    if(m_type != PhotoAlbum)
    {
        qDebug() << "You can't save this model";
        return;
    }

    if(isVirtualAlbum)
    {
        qDebug() << "You can't save a virtual album: " << m_album;
        return;
    }

    if(album.isEmpty())
    {
        QDateTime time = QDateTime();
        time.setTimeSpec(Qt::LocalTime);
        m_album = time.currentDateTime().toString(Qt::ISODate);
    }
    else
        m_album = album;

    PhotoDatabase::instance()->saveAlbum(mediaItemsList, m_album);
    connectSignals(true, true, true);
    emit albumChanged(m_album);
}

void PhotoListModel::saveAlbum()
{
    saveAlbum(m_album);
}

QStringList PhotoListModel::getAllURIs()
{
    QStringList uris;
    if(m_type != PhotoAlbum)
    {
        qDebug() << "It's not safe to get all uris for anything by photoalbums";
        return uris;
    }

    for(int i = 0; i < mediaItemsList.count(); i++)
        uris << mediaItemsList[i]->m_uri;
    return uris;
}

void PhotoListModel::requestItem(int type, QString urn)
{
    if(!urn.isEmpty())
        PhotoDatabase::instance()->requestItem(type, urn);
}

void PhotoListModel::itemsAdded(const QList<MediaItem *> *list)
{
    QList<MediaItem *> newItemList;

    if(m_type == ListofPhotos)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isPhoto())
                newItemList << list->at(i);
    }
    else if(m_type == ListofUserPhotos)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isPhoto()&&list->at(i)->m_isusercontent)
                newItemList << list->at(i);
    }
    else if(m_type == ListofAlbums)
    {
        for(int i = 0; i < list->count(); i++)
           if(list->at(i)->isPhotoAlbum())
               newItemList << list->at(i);
    }
    else if(m_type == ListofUserAlbums)
    {
        for(int i = 0; i < list->count(); i++)
           if(list->at(i)->isPhotoAlbum()&&!list->at(i)->isVirtual())
               newItemList << list->at(i);
    }
    else if(m_type == ListofRecentlyViewed)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->m_recentlyviewed&&!list->at(i)->isVirtual())
                newItemList << list->at(i);
    }
    else if((m_type == PhotoAlbum)&&(!m_album.isEmpty()))
    {
        MediaItem *albumitem = PhotoDatabase::instance()->getAlbumItem(m_album);
        if((albumitem != NULL)&&!albumitem->children.isEmpty())
        {
            QList<MediaItem *> albumitems = PhotoDatabase::instance()->getItemsByURN(albumitem->children);
            for(int i = 0; i < albumitems.count(); i++)
                if(!mediaItemsList.contains(albumitems[i]))
                    newItemList << albumitems[i];
        }
    }
    else
    {
        return;
    }

    hideItems(newItemList);
    if(!newItemList.isEmpty())
    {
        displayNewItems(newItemList);
    }
}

void PhotoListModel::itemsChanged(const QStringList &ids, int reason)
{
    int i1=-1, i2=-1;

    if((m_type == ListofRecentlyViewed)&&(reason == PhotoDatabase::Viewed))
    {
        /* first add in any new recently viewed items */
        QList<MediaItem *> newItemList = PhotoDatabase::instance()->getItemsByID(ids);
        QList<MediaItem *> knownItemList;
        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            if(ids.contains(mediaItemsList[i]->m_id))
            {
                newItemList.removeAll(mediaItemsList[i]);
                knownItemList << mediaItemsList[i];
            }
        }
        if(!newItemList.isEmpty())
            displayNewItems(newItemList);
        if(!knownItemList.isEmpty())
            resortItems(newItemList);
        return;
    }
    else if((reason == PhotoDatabase::Unviewed)&&(m_type == ListofRecentlyViewed))
    {
        itemsRemoved(ids);
        return;
    }
    else if((((m_filter == FilterFavorite)||(m_sort == SortByFavorite))&&
             (reason == PhotoDatabase::Favorited))||
            (((m_filter == FilterViewed)||(m_filter == FilterUnwatched)||(m_sort == SortByAccessTime)||(m_sort == SortByUnwatched))&&
             (reason == PhotoDatabase::Viewed)))
    {
        redisplay();
        return;
    }

    if((m_type != PhotoAlbum)&&(!m_album.isEmpty()))
    {
        MediaItem *albumitem = PhotoDatabase::instance()->getAlbumItem(m_album);
        if(ids.contains(albumitem->m_id)&&(albumitem->m_thumburi_exists))
            m_coveruri = albumitem->m_thumburi;
    }

    for(int i = 0; i < mediaItemsDisplay.count(); i++)
    {
        if(ids.contains(mediaItemsDisplay[i]->getID()))
        {
            if(i1 < 0)
            {
                i1 = i;
                i2 = i;
            }
            else if(i == i2 + 1)
            {
                i2++;
            }
            else
            {
                emit dataChanged(index(i1, 0), index(i2, 0));
                i1 = i;
                i2 = i;
            }
        }
    }
    if(i1 >= 0)
    {
        emit dataChanged(index(i1, 0), index(i2, 0));
    }
}

void PhotoListModel::setFavorite(const QStringList &ids, bool val)
{
    PhotoDatabase::instance()->setFavoriteMulti(ids, val);
}

void PhotoListModel::setViewed(const QStringList &ids)
{
    qDebug() << "setViewed: " << ids;
    PhotoDatabase::instance()->setViewedMulti(ids);
}

void PhotoListModel::addItems(const QStringList &ids)
{
    if(m_type != PhotoAlbum)
    {
        qDebug() << "You can't add items to this model";
        return;
    }

    QList<MediaItem *> possibleItemList = PhotoDatabase::instance()->getItemsByID(ids);
    QList<MediaItem *> newItemList;

    for (int i = 0; i < possibleItemList.count(); i++)
        if(!mediaItemsList.contains(possibleItemList[i]))
            newItemList << possibleItemList[i];

    displayNewItems(newItemList);
    saveAlbum(m_album);
}

void PhotoListModel::removeItems(const QStringList &ids)
{
    if(m_type != PhotoAlbum)
    {
        qDebug() << "You can't remove items from this model";
        return;
    }

    //qDebug() << "removing items " << ids;
    QList<MediaItem *> deleteItemList;

    for (int i = 0; i < mediaItemsList.count(); i++)
        if(ids.contains(mediaItemsList[i]->m_id))
            deleteItemList << mediaItemsList[i];

    for (int i = 0; i < deleteItemList.count(); i++)
    {
        qDebug() << "removing item " << deleteItemList[i]->m_title;
        int index = mediaItemsDisplay.indexOf(deleteItemList[i]);
        if(index >= 0)
        {
            qDebug() << "removing index " << index;
            removeRows(index, 1);
        }
        mediaItemsList.removeAll(deleteItemList[i]);
    }
    saveAlbum(m_album);
    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void PhotoListModel::removeIndex(const int index)
{
    if(m_type != PhotoAlbum)
    {
        qDebug() << "You can't remove items from this model";
        return;
    }

    if((index < 0)||(index >= mediaItemsDisplay.count()))
    {
        qDebug() << "removeIndex: Invalid Index";
        return;
    }

    int nth = 0;
    MediaItem *item = mediaItemsDisplay[index];
    /* find out which instance of item we're deleting */
    for(int i = 0; i < mediaItemsDisplay.count(); i++)
    {
        if(i == index)
            break;
        if(mediaItemsDisplay[i] == item)
           nth++;
    }

    removeRows(index, 1);
    int count = 0;
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        /* delete the nth item from the master list */
        if((mediaItemsList[i] == item)&&(nth == count++))
        {
            mediaItemsList.removeAt(i);
            break;
        }
    }
    saveAlbum(m_album);
    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void PhotoListModel::destroyItem(const int index)
{
    if((index < 0)||(index >= mediaItemsDisplay.count()))
    {
        qDebug() << "removeIndex: Invalid Index";
        return;
    }

    PhotoDatabase::instance()->destroyItem(mediaItemsDisplay[index]);
}

void PhotoListModel::destroyItemByID(const QString &id)
{
    for (int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            PhotoDatabase::instance()->destroyItem(mediaItemsList[i]);
}

void PhotoListModel::destroyItemsByID(const QStringList &ids)
{
    for (int i = 0; i < ids.count(); i++)
        destroyItemByID(ids[i]);
}

void PhotoListModel::setCoverArt(const QString &thumburi)
{
    if(m_type != PhotoAlbum)
    {
        qDebug() << "You can only set cover art in the album editor";
        return;
    }

    if(!m_album.isEmpty())
    {
        MediaItem *albumitem = PhotoDatabase::instance()->getAlbumItem(m_album);
        albumitem->m_thumburi = thumburi;
        albumitem->m_thumburi_ignore = false;
        if(albumitem->thumbExists())
            albumitem->m_thumburi_exists = true;
        QStringList ids;
        ids << albumitem->m_id;
        PhotoDatabase::instance()->broadcastMyChanges(ids, PhotoDatabase::Thumbnail);
        m_coveruri = thumburi;
        emit coverArtChanged(m_coveruri);
        PhotoDatabase::instance()->setCoverArt(m_album, thumburi);
    }
}

void PhotoListModel::clear()
{
    if(m_type != PhotoAlbum && m_type != ListofRecentlyViewed)
    {
        qDebug() << "You can't remove items from this model";
        return;
    }
    beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
    mediaItemsList.clear();
    mediaItemsDisplay.clear();
    endRemoveRows();

    if(m_type == ListofRecentlyViewed)
        PhotoDatabase::instance()->clearHistory();

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

QVariant PhotoListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > mediaItemsDisplay.count())
        return QVariant();

    if (role == MediaItem::ID)
        return mediaItemsDisplay[index.row()]->getID();

    if (role == MediaItem::URN)
        return mediaItemsDisplay[index.row()]->m_urn;

    if (role == MediaItem::ItemType)
        return mediaItemsDisplay[index.row()]->getType();

    if (role == MediaItem::URI)
        return mediaItemsDisplay[index.row()]->getURI();

    if (role == MediaItem::ThumbURI)
    {
        /* tell the database we need a thumbnail if one isn't ready */
        if(mediaItemsDisplay[index.row()]->m_thumburi_ignore)
        {
            return "";
        }
        else if(mediaItemsDisplay[index.row()]->m_thumburi_exists)
        {
            return mediaItemsDisplay[index.row()]->getThumbURI();
        }
        else
        {
            PhotoDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]);
            return "";
        }
    }

    if (role == MediaItem::AlbumThumbURI)
    {
        if((mediaItemsDisplay[index.row()]->albumitem == NULL)||
           (mediaItemsDisplay[index.row()]->albumitem->m_thumburi_ignore))
        {
            return "";
        }
        else if(mediaItemsDisplay[index.row()]->albumitem->m_thumburi_exists)
        {
            return mediaItemsDisplay[index.row()]->albumitem->getThumbURI();
        }
        else
        {
            PhotoDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]->albumitem);
            return "";
        }
    }

    if (role == MediaItem::Title)
        return mediaItemsDisplay[index.row()]->getTitle();

    if (role == MediaItem::Album)
        return mediaItemsDisplay[index.row()]->getAlbum();

    if (role == MediaItem::Camera)
        return mediaItemsDisplay[index.row()]->getCamera();

    if (role == MediaItem::CreationTime)
        return mediaItemsDisplay[index.row()]->getCreationTime();

    if (role == MediaItem::AddedTime)
        return mediaItemsDisplay[index.row()]->getAddedTime();

    if (role == MediaItem::LastPlayedTime)
        return mediaItemsDisplay[index.row()]->getLastPlayedTime();

    if (role == MediaItem::Favorite)
        return mediaItemsDisplay[index.row()]->getFavorite();

    if (role == MediaItem::RecentlyAdded)
        return mediaItemsDisplay[index.row()]->getRecentlyAdded();

    if (role == MediaItem::RecentlyViewed)
        return mediaItemsDisplay[index.row()]->getRecentlyViewed();

    if (role == MediaItem::Virtual)
        return mediaItemsDisplay[index.row()]->isVirtual();

    if (role == MediaItem::Index)
        return index.row();

    if (role == MediaItem::Width)
        return mediaItemsDisplay[index.row()]->getWidth();

    if (role == MediaItem::Height)
        return mediaItemsDisplay[index.row()]->getHeight();

    if (role == MediaItem::UserContent)
        return mediaItemsDisplay[index.row()]->m_isusercontent;

    return QVariant();
}

void PhotoListModel::changeTitle(QString uri, QString title)
{
    PhotoDatabase::instance()->changeTitle(uri, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_uri == uri)
            emit dataChanged(index(i, 0), index(i, 0));
}

void PhotoListModel::changeTitleByURN(QString urn, QString title)
{
    PhotoDatabase::instance()->changeTitleByURN(urn, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            emit dataChanged(index(i, 0), index(i, 0));
}
