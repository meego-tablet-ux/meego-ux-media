/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDir>
#include <QDebug>
#include "documentlistmodel.h"
#include "documentdatabase.h"

DocumentListModel::DocumentListModel(QObject *parent)
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
    roles.insert(MediaItem::Favorite, "favorite");
    roles.insert(MediaItem::RecentlyAdded, "recentlyadded");
    roles.insert(MediaItem::RecentlyViewed, "recentlyviewed");
    roles.insert(MediaItem::UserContent, "usercontent");
    setRoleNames(roles);

    m_type = -1;

    /*
    QStringList filenameList;
    filenameList
            << "msword.doc"
            << "adobepdf.pdf"
            << "plaintext.txt";
    QStringList thumbURIList;
    thumbURIList
            << "image://themedimage/images/media/icn_ms_word"
            << "image://themedimage/images/media/icn_adobe_pdf"
            << "image://themedimage/images/media/icn_plain_txt";

    beginInsertRows(QModelIndex(), 0, filenameList.count()-1);
    for (int index=0;index<filenameList.count();++index) {
        QString filename = filenameList.at(index);
        QString thumburi = thumbURIList.at(index);
        MediaItem *item = new MediaItem(MediaItem::DocumentItem);
        item->setTitle(filename);
        item->setThumbURI(thumburi);
        mediaItemsDisplay.insert(index, item);
    }
    endInsertRows();*/
    connect(DocumentDatabase::instance(),SIGNAL(itemAvailable(const QString)),this,SIGNAL(itemAvailable(const QString)));
}

DocumentListModel::~DocumentListModel()
{
    clearData();
}

void DocumentListModel::connectSignals(bool added, bool changed, bool removed)
{
    if(added)
        connect(DocumentDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));

    if(changed)
        connect(DocumentDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));

    if(removed)
        connect(DocumentDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));

}

void DocumentListModel::clearData()
{
    disconnect(DocumentDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));
    disconnect(DocumentDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));
    disconnect(DocumentDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));

    if(!mediaItemsDisplay.isEmpty())
    {
        /* formally remove all the items from the list */
        beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
        mediaItemsDisplay.clear();
        endRemoveRows();
    }
    mediaItemsList.clear();
}

void DocumentListModel::setType(const int type)
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
    QList<MediaItem *> tempList = DocumentDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    if(m_type == ListofDocuments)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isDocument())
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

void DocumentListModel::requestItem(QString urn)
{
    if(!urn.isEmpty())
        DocumentDatabase::instance()->requestItem(urn);
}

void DocumentListModel::itemsAdded(const QList<MediaItem *> *list)
{
    QList<MediaItem *> newItemList;

    if(m_type == ListofDocuments)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isDocument())
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

void DocumentListModel::itemsChanged(const QStringList &ids, int reason)
{
    int i1=-1, i2=-1;

    if(reason == DocumentDatabase::Viewed)
    {
        if(m_type == ListofRecentlyViewed)
        {
            /* first add in any new recently viewed items */
            QList<MediaItem *> newItemList = DocumentDatabase::instance()->getItemsByID(ids);
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
        else if((m_filter == FilterViewed)||(m_filter == FilterUnwatched)||
                (m_sort == SortByAccessTime)||(m_sort == SortByUnwatched))
        {
            redisplay();
            return;
        }
    }
    else if((reason == DocumentDatabase::Unviewed)&&(m_type == ListofRecentlyViewed))
    {
        itemsRemoved(ids);
        return;
    }
    else if(((m_filter == FilterFavorite)||(m_sort == SortByFavorite))&&
            (reason == DocumentDatabase::Favorited))
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

void DocumentListModel::setFavorite(const QStringList &ids, bool val)
{
    DocumentDatabase::instance()->setFavoriteMulti(ids, val);
}

void DocumentListModel::setViewed(const QStringList &ids)
{
    DocumentDatabase::instance()->setViewedMulti(ids);
}

void DocumentListModel::destroyItem(const int index)
{
    if((index < 0)||(index >= mediaItemsDisplay.count()))
    {
        qDebug() << "removeIndex: Invalid Index";
        return;
    }

    DocumentDatabase::instance()->destroyItem(mediaItemsDisplay[index]);
}

void DocumentListModel::destroyItemByID(const QString &id)
{
    for (int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            DocumentDatabase::instance()->destroyItem(mediaItemsList[i]);
}

void DocumentListModel::destroyItemsByID(const QStringList &ids)
{
    for (int i = 0; i < ids.count(); i++)
        destroyItemByID(ids[i]);
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > mediaItemsDisplay.count()) {
        return QVariant();   
    }

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
            DocumentDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]);
            return "";
        }
    }

    if (role == MediaItem::Title) {
        return mediaItemsDisplay[index.row()]->getTitle();
    }

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

    if (role == MediaItem::PlayStatus)
        return mediaItemsDisplay[index.row()]->m_playstatus;

    if (role == MediaItem::UserContent)
        return mediaItemsDisplay[index.row()]->m_isusercontent;

    return QVariant();
}

void DocumentListModel::clear()
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

    DocumentDatabase::instance()->clearHistory();

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void DocumentListModel::changeTitle(QString uri, QString title)
{
    DocumentDatabase::instance()->changeTitle(uri, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_uri == uri)
            emit dataChanged(index(i, 0), index(i, 0));
}

void DocumentListModel::changeTitleByURN(QString urn, QString title)
{
    DocumentDatabase::instance()->changeTitleByURN(urn, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_urn == urn) {
            mediaItemsList[i]->setProperty("title",title);
            emit dataChanged(index(i, 0), index(i, 0));
        }
}
