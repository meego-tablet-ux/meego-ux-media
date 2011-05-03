/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDir>
#include <QDebug>
#include "videolistmodel.h"
#include "videodatabase.h"

VideoListModel::VideoListModel(QObject *parent)
    : MediaListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(MediaItem::ID, "itemid");
    roles.insert(MediaItem::URN, "urn");
    roles.insert(MediaItem::ItemType, "itemtype");
    roles.insert(MediaItem::URI, "uri");
    roles.insert(MediaItem::ThumbURI, "thumburi");
    roles.insert(MediaItem::Title, "title");
    roles.insert(MediaItem::CreationTime, "creationtime");
    roles.insert(MediaItem::AddedTime, "addedtime");
    roles.insert(MediaItem::Length, "length");
    roles.insert(MediaItem::LastPlayedTime, "lastviewedtime");
    roles.insert(MediaItem::Favorite, "favorite");
    roles.insert(MediaItem::RecentlyAdded, "recentlyadded");
    roles.insert(MediaItem::RecentlyViewed, "recentlyviewed");
    roles.insert(MediaItem::Index, "index");
    roles.insert(MediaItem::PlayStatus, "playstatus");
    roles.insert(MediaItem::UserContent, "usercontent");
    setRoleNames(roles);

    m_type = -1;

    connect(VideoDatabase::instance(),SIGNAL(itemAvailable(const QString)),this,SIGNAL(itemAvailable(const QString)));
}

VideoListModel::~VideoListModel()
{
    clearData();
}

void VideoListModel::connectSignals(bool added, bool changed, bool removed)
{
    if(added)
        connect(VideoDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));

    if(changed)
        connect(VideoDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));

    if(removed)
        connect(VideoDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));
}

void VideoListModel::clearData()
{
    /* don't listen to new items coming from the database, we're starting over */
    disconnect(VideoDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));
    disconnect(VideoDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));
    disconnect(VideoDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));

    if(!mediaItemsDisplay.isEmpty())
    {
        /* formally remove all the items from the list */
        beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
        mediaItemsList.clear();
        mediaItemsDisplay.clear();
        endRemoveRows();
    }
}

void VideoListModel::setType(const int type)
{
    clearData();
    m_type = type;
    m_default_sort = SortAsIs;
    hiddenURNsList.clear();
    emit typeChanged(m_type);

    /* for these items we need further input to proceed */
    switch(m_type) {
    case Editor:
        return;
    }

    /* for basic lists, grab a new database snapshot */
    QList<MediaItem *> tempList = VideoDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    if(m_type == ListofVideos)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isVideo())
                newItemList << tempList[i];
    }
    else if(m_type == ListofUserVideos)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isAnyVideoType()&&tempList[i]->m_isusercontent)
                newItemList << tempList[i];
    }
    else if(m_type == ListofTVShows)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isTVShow())
                newItemList << tempList[i];
    }
    else if(m_type == ListofMusicVideos)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isMusicVideo())
                newItemList << tempList[i];
    }
    else if(m_type == ListofAll)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isAnyVideoType())
                newItemList << tempList[i];
    }
    else if(m_type == ListofUnwatched)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->m_lastplayedtime.isEmpty())
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

void VideoListModel::requestItem(QString urn)
{
    if(!urn.isEmpty())
        VideoDatabase::instance()->requestItem(urn);
}

void VideoListModel::itemsAdded(const QList<MediaItem *> *list)
{
    QList<MediaItem *> newItemList;

    if(m_type == ListofVideos)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isVideo())
                newItemList << list->at(i);
    }
    else if(m_type == ListofUserVideos)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isAnyVideoType()&&list->at(i)->m_isusercontent)
                newItemList << list->at(i);
    }
    else if(m_type == ListofTVShows)
    {
        for(int i = 0; i < list->count(); i++)
           if(list->at(i)->isTVShow())
               newItemList << list->at(i);
    }
    else if(m_type == ListofMusicVideos)
    {
        for(int i = 0; i < list->count(); i++)
           if(list->at(i)->isMusicVideo())
               newItemList << list->at(i);
    }
    else if(m_type == ListofAll)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isAnyVideoType())
                newItemList << list->at(i);
    }
    else if(m_type == ListofUnwatched)
    {
        for(int i = 0; i < list->count(); i++)
           if(list->at(i)->m_lastplayedtime.isEmpty())
               newItemList << list->at(i);
    }
    else if(m_type == ListofRecentlyViewed)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->m_recentlyviewed&&!list->at(i)->isVirtual())
                newItemList << list->at(i);
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

void VideoListModel::itemsChanged(const QStringList &ids, int reason)
{
    int i1=-1, i2=-1;

    if(reason == VideoDatabase::Viewed)
    {
        if(m_type == ListofRecentlyViewed)
        {
            /* first add in any new recently viewed items */
            QList<MediaItem *> newItemList = VideoDatabase::instance()->getItemsByID(ids);
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
        else if(m_type == ListofUnwatched)
        {
            itemsRemoved(ids);
            return;
        }
        else if((m_filter == FilterViewed)||(m_filter == FilterUnwatched)||
                (m_sort == SortByAccessTime)||(m_sort == SortByUnwatched))
        {
            redisplay();
            return;
        }
    }
    else if((reason == VideoDatabase::Unviewed)&&(m_type == ListofRecentlyViewed))
    {
        itemsRemoved(ids);
        return;
    }
    else if(((m_filter == FilterFavorite)||(m_sort == SortByFavorite))&&
            (reason == VideoDatabase::Favorited))
    {
        redisplay();
        return;
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

void VideoListModel::setFavorite(const QStringList &ids, bool val)
{
    VideoDatabase::instance()->setFavoriteMulti(ids, val);
}

void VideoListModel::setViewed(const QStringList &ids)
{
    VideoDatabase::instance()->setViewedMulti(ids);
}

void VideoListModel::setPlayStatus(const QString &id, int playstatus)
{
    VideoDatabase::instance()->setPlayStatus(id, playstatus);
}

void VideoListModel::destroyItem(const int index)
{
    if((index < 0)||(index >= mediaItemsDisplay.count()))
    {
        qDebug() << "removeIndex: Invalid Index";
        return;
    }

    VideoDatabase::instance()->destroyItem(mediaItemsDisplay[index]);
}

void VideoListModel::destroyItemByID(const QString &id)
{
    for (int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            VideoDatabase::instance()->destroyItem(mediaItemsList[i]);
}

void VideoListModel::destroyItemsByID(const QStringList &ids)
{
    for (int i = 0; i < ids.count(); i++)
        destroyItemByID(ids[i]);
}

QVariant VideoListModel::data(const QModelIndex &index, int role) const
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
            VideoDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]);
            return "";
        }
    }

    if (role == MediaItem::Title)
        return mediaItemsDisplay[index.row()]->getTitle();

    if (role == MediaItem::Length)
        return mediaItemsDisplay[index.row()]->getLength();

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

    if (role == MediaItem::Index)
        return index.row();

    if (role == MediaItem::PlayStatus)
        return mediaItemsDisplay[index.row()]->m_playstatus;

    if (role == MediaItem::UserContent)
        return mediaItemsDisplay[index.row()]->m_isusercontent;

    return QVariant();
}

void VideoListModel::clear()
{
    if(m_type != ListofRecentlyViewed)
    {
        qDebug() << "You can't remove items from this model";
        return;
    }
    beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
    mediaItemsList.clear();
    mediaItemsDisplay.clear();
    endRemoveRows();

    VideoDatabase::instance()->clearHistory();

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void VideoListModel::changeTitle(QString uri, QString title)
{
    VideoDatabase::instance()->changeTitle(uri, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_uri == uri)
            emit dataChanged(index(i, 0), index(i, 0));
}

void VideoListModel::changeTitleByURN(QString urn, QString title)
{
    VideoDatabase::instance()->changeTitleByURN(urn, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            emit dataChanged(index(i, 0), index(i, 0));
}
