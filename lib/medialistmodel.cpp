/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "medialistmodel.h"

#include <QDir>
#include <QDebug>

MediaListModel::MediaListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_filter = FilterAll;
    m_limit = -1;
    m_sort = -1;
    m_default_sort = SortAsIs;
    firstsort = true;
    datesearch = false;
    disable_filter = false;
}

MediaListModel::~MediaListModel()
{
    qDebug() << "~MediaListModel";
    mediaItemsList.clear();
    mediaItemsDisplay.clear();
}

bool MediaListModel::isURN(const QString &val)
{
    return val.startsWith("urn:");
}

int MediaListModel::itemIndex(const QString &id)
{
    for(int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsDisplay[i]->m_id == id)
            return i;
    return -1;
}

QVariant MediaListModel::datafromItem(MediaItem *item, int role)
{
    if(item == NULL)
        return QVariant();

    if (role == MediaItem::ID)
        return item->getID();

    if (role == MediaItem::ItemType)
        return item->getType();

    if (role == MediaItem::URI)
        return item->getURI();

    if (role == MediaItem::ThumbURI)
    {
        if(item->m_thumburi_exists)
        {
            return item->getThumbURI();
        }
        else
        {
            return "";
        }
    }

    if (role == MediaItem::Title)
        return item->getTitle();

    if (role == MediaItem::Artist)
        return item->getArtist();

    if (role == MediaItem::Album)
        return item->getAlbum();

    if (role == MediaItem::TrackNum)
        return item->getTrackNum();

    if (role == MediaItem::Length)
        return item->getLength();

    if (role == MediaItem::CreationTime)
        return item->getCreationTime();

    if (role == MediaItem::AddedTime)
        return item->getAddedTime();

    if (role == MediaItem::LastPlayedTime)
        return item->getLastPlayedTime();

    if (role == MediaItem::Favorite)
        return item->getFavorite();

    if (role == MediaItem::RecentlyAdded)
        return item->getRecentlyAdded();

    if (role == MediaItem::RecentlyViewed)
        return item->getRecentlyViewed();

    if (role == MediaItem::Virtual)
        return item->isVirtual();

    if (role == MediaItem::PlayStatus)
        return item->m_playstatus;

    if (role == MediaItem::UserContent)
        return item->m_isusercontent;

    if (role == MediaItem::Camera)
        return item->getCamera();

    if (role == MediaItem::Width)
        return item->getWidth();

    if (role == MediaItem::Height)
        return item->getHeight();

    return QVariant();
}

QVariant MediaListModel::datafromURN(const QString &urn, int role)
{
    MediaItem *item = NULL;
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            item = mediaItemsList[i];

    if(item == NULL)
        return QVariant();

    return datafromItem(item, role);
}

QVariant MediaListModel::datafromID(const QString &id, int role)
{
    MediaItem *item = NULL;
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            item = mediaItemsList[i];

    if(item == NULL)
        return QVariant();

    return datafromItem(item, role);
}

QVariant MediaListModel::datafromIndex(const int index, int role)
{
    if((index >= 0)&&(index < mediaItemsDisplay.count()))
        return datafromItem(mediaItemsDisplay[index], role);

    return QVariant();
}

QString MediaListModel::getURIfromIndex(const int index)
{
    if((index >= 0)&&(index < mediaItemsDisplay.count()))
        return mediaItemsDisplay[index]->m_uri;
    return "";
}

QString MediaListModel::getIDfromURN(const QString &urn)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            return mediaItemsList[i]->m_id;

    return "";
}

QString MediaListModel::getTitlefromURN(const QString &urn)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            return mediaItemsList[i]->m_title;

    return "";
}

QString MediaListModel::getTitlefromID(const QString &id)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            return mediaItemsList[i]->m_title;

    return "";
}

int MediaListModel::getIndexfromURN(const QString &urn)
{
    for(int i = 0; i< mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            return i;

    return -1;
}

int MediaListModel::getTypefromURN(const QString &urn)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            return mediaItemsList[i]->m_type;

    return -1;
}

int MediaListModel::getTypefromID(const QString &id)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            return mediaItemsList[i]->m_type;

    return -1;
}

QString MediaListModel::getURIfromURN(const QString &urn)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            return mediaItemsList[i]->m_uri;

    return "";
}

QString MediaListModel::getURIfromID(const QString &id)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            return mediaItemsList[i]->m_uri;

    return "";
}

bool MediaListModel::isFavorite(const QString &id)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            return mediaItemsList[i]->m_favorite;
    return false;
}

void MediaListModel::setSelected(const QString &id, bool selected)
{
    MediaItem *item = NULL;
    int idx = -1;

    for(idx = 0; idx < mediaItemsDisplay.count(); idx++)
        if(mediaItemsDisplay[idx]->m_id == id)
        {
            item = mediaItemsDisplay[idx];
            break;
        }

    if((item == NULL)||(idx < 0))
        return;

    if(selected)
        selectedItemsList.append(item);
    else
        selectedItemsList.removeAll(item);

    emit dataChanged(index(idx, 0), index(idx, 0));
}

bool MediaListModel::isSelected(const QString &id)
{
    for(int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsDisplay[i]->m_id == id)
            return(selectedItemsList.contains(mediaItemsDisplay[i]));

    return false;
}

void MediaListModel::filterDuplicates(QList<MediaItem *> &currentList, QList<MediaItem *> &additionList)
{
    int count = additionList.count();
    for (int i = 0; i < count; i++)
    {
        if(currentList.contains(additionList[i]))
        {
            additionList.removeAt(i);
            i--;
            count = additionList.count();
        }
    }
}

void MediaListModel::hideItemByURN(const QString &urn)
{
    QList<MediaItem *> hiddenItems;

    hiddenURNsList << urn;
    for (int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            hiddenItems << mediaItemsList[i];

    if(!hiddenItems.isEmpty())
    {
        for (int i = 0; i < hiddenItems.count(); i++)
        {
            mediaItemsList.removeAll(hiddenItems[i]);
            int count = mediaItemsDisplay.count();
            for (int j = 0; j < count; j++)
            {
                if(mediaItemsDisplay[j] == hiddenItems[i])
                {
                    beginRemoveRows(QModelIndex(), j, j);
                    mediaItemsDisplay.removeAt(j);
                    endRemoveRows();
                    j--;
                    count = mediaItemsDisplay.count();
                }
            }
        }
    }
    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void MediaListModel::hideItemsByURN(const QStringList &urns)
{
    for (int i = 0; i < urns.count(); i++)
        hideItemByURN(urns[i]);
}

void MediaListModel::hideItems(QList<MediaItem *> &list)
{
    QList<MediaItem *> hiddenItems;

    for (int i = 0; i < list.count(); i++)
        if(hiddenURNsList.contains(list[i]->m_urn))
            hiddenItems << list[i];

    for (int i = 0; i < hiddenItems.count(); i++)
        list.removeAll(hiddenItems[i]);
}

void MediaListModel::clearSelected()
{
    QList<MediaItem *> prevSelected = selectedItemsList;
    selectedItemsList.clear();
    for(int i = 0; i < mediaItemsDisplay.count(); i++)
        if(prevSelected.contains(mediaItemsDisplay[i]))
        {
            emit dataChanged(index(i, 0), index(i, 0));
        }
}

QStringList MediaListModel::getSelectedURIs()
{
    QStringList uris;
    for(int i = 0; i < selectedItemsList.count(); i++)
        uris << selectedItemsList[i]->m_uri;
    return uris;
}

QStringList MediaListModel::getSelectedIDs()
{
    QStringList ids;
    for(int i = 0; i < selectedItemsList.count(); i++)
        ids << selectedItemsList[i]->m_id;
    return ids;
}

QStringList MediaListModel::getAllIDs()
{
    QStringList ids;
    for(int i = 0; i < mediaItemsDisplay.count(); i++)
        ids << mediaItemsDisplay[i]->m_id;
    return ids;
}

int MediaListModel::selectionCount()
{
    return selectedItemsList.count();
}

void MediaListModel::setLimit(const int limit)
{
    if(m_limit == limit)
        return;

    if(limit < 0)
    {
        qDebug() << "limit must be 0 (all) or greater";
        return;
    }

    m_limit = limit;
    redisplay();
    emit limitChanged(m_limit);
}

void MediaListModel::setSort(const int sort)
{
    switch(sort) {
    case SortByDefault:
        m_sort = m_default_sort;
        redisplay();
        break;
    case SortByTitle:
    case SortByAddedTime:
    case SortByCreationTime:
    case SortByAccessTime:
    case SortByUnwatched:
    case SortByFavorite:
    case SortByTrackNum:
    case SortByURNList:
    case SortAsIs:
        m_sort = sort;
        redisplay();
        break;
    default:
        qDebug() << "Invalid sort type";
        return;
    }

    emit sortChanged(m_sort);
}

void MediaListModel::checkForTimeString(const QString search)
{
    QDate cdate = QDate::currentDate();
    QString year;
    year.sprintf("%04d", cdate.year());

    /* year */
    rangeStart = QDateTime::fromString(search, "yyyy");
    if(rangeStart.isValid()) goto foundyear;

    /* month */
    rangeStart = QDateTime::fromString(search, "MMMM yyyy");
    if(rangeStart.isValid()) goto foundmonth;
    rangeStart = QDateTime::fromString(search, "MMMM, yyyy");
    if(rangeStart.isValid()) goto foundmonth;
    rangeStart = QDateTime::fromString(search, "MMM yyyy");
    if(rangeStart.isValid()) goto foundmonth;
    rangeStart = QDateTime::fromString(search, "MMM, yyyy");
    if(rangeStart.isValid()) goto foundmonth;

    rangeStart = QDateTime::fromString(search, "MMMM");
    if(rangeStart.isValid())
    {
        rangeStart = QDateTime::fromString(search + " " + year, "MMMM yyyy");
        goto foundmonth;
    }
    rangeStart = QDateTime::fromString(search, "MMM");
    if(rangeStart.isValid())
    {
        rangeStart = QDateTime::fromString(search + " " + year, "MMM yyyy");
        goto foundmonth;
    }

    /* day */
    rangeStart = QDateTime::fromString(search, "MMMM d, yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "MMMM d yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "d MMMM, yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "d MMMM yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "MMM d, yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "MMM d yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "d MMM, yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "d MMM yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "M-d-yyyy");
    if(rangeStart.isValid()) goto foundday;
    rangeStart = QDateTime::fromString(search, "M/d/yyyy");
    if(rangeStart.isValid()) goto foundday;

    rangeStart = QDateTime::fromString(search, "MMMM d");
    if(rangeStart.isValid())
    {
        rangeStart = QDateTime::fromString(search + " " + year, "MMMM d yyyy");
        goto foundday;
    }
    rangeStart = QDateTime::fromString(search, "MMM d");
    if(rangeStart.isValid())
    {
        rangeStart = QDateTime::fromString(search + " " + year, "MMM d yyyy");
        goto foundday;
    }
    rangeStart = QDateTime::fromString(search, "d MMMM");
    if(rangeStart.isValid())
    {
        rangeStart = QDateTime::fromString(search + " " + year, "d MMMM yyyy");
        goto foundday;
    }
    rangeStart = QDateTime::fromString(search, "d MMM");
    if(rangeStart.isValid())
    {
        rangeStart = QDateTime::fromString(search + " " + year, "d MMM yyyy");
        goto foundday;
    }

    datesearch = false;
    return;

foundyear:
    rangeEnd = rangeStart.addYears(1);
    datesearch = true;
    return;

foundmonth:
    rangeEnd = rangeStart.addMonths(1);
    datesearch = true;
    return;

foundday:
    rangeEnd = rangeStart.addDays(1);
    datesearch = true;
    return;
}

void MediaListModel::setSearch(const QString search)
{
    if(m_search == search)
        return;

    bool filterchange = false;
    if(search.isEmpty())
    {
        m_search = search;
        datesearch = false;
        if(m_filter != FilterAll)
        {
            m_filter = FilterAll;
            filterchange = true;
        }
        redisplay();
        if(filterchange)
            emit filterChanged(m_filter);
        emit searchChanged(m_search);
        return;
    }

    if(m_filter != FilterSearch)
    {
        m_filter = FilterSearch;
        filterchange = true;
    }
    m_search = search;
    datesearch = false;
    checkForTimeString(search);
    redisplay();
    if(filterchange)
        emit filterChanged(m_filter);
    emit searchChanged(m_search);
}

void MediaListModel::notifyChanged(const QStringList &ids)
{
    int i1=-1, i2=-1;

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

void MediaListModel::setFilter(const int filter)
{
    if((m_filter == filter)||(disable_filter))
        return;

    m_filter = filter;
    redisplay();
    emit filterChanged(filter);
}

QVariant MediaListModel::data(int index) const
{
    if(index >= mediaItemsDisplay.size())
        return 0;

    return QVariant::fromValue(static_cast<void *>(mediaItemsDisplay[index]));
}

int MediaListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mediaItemsDisplay.size();
}

int MediaListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant MediaListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        qDebug() << "unexpected data call in MediaListModel";
        return QVariant();
    }
    return data(index.row());
}

bool MediaListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for(int i = 0; i < count; i++)
        mediaItemsDisplay.removeAt(row);
    endRemoveRows();
    return true;
}

void MediaListModel::insertRow(int row, MediaItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    mediaItemsDisplay.insert(row, item);
    endInsertRows();
}

void MediaListModel::moveRow(int rowsrc, int rowdst)
{
    beginMoveRows(QModelIndex(), rowsrc, rowsrc, QModelIndex(), rowdst);
    mediaItemsDisplay.move(rowsrc, rowdst);
    endMoveRows();
}

bool MediaListModel::isItemDisplayed(MediaItem *item)
{
    if(m_filter == FilterViewed)
    {
        return item->m_recentlyviewed;
    }
    else if(m_filter == FilterAdded)
    {
        return item->m_recentlyadded;
    }
    else if(m_filter == FilterFavorite)
    {
        return item->m_favorite;
    }
    else if(m_filter == FilterSearch)
    {
        if(datesearch)
        {
            if((item->m_timestamp > rangeStart)&&(item->m_timestamp < rangeEnd))
                return true;
        }
        else if(item->m_title.contains(m_search, Qt::CaseInsensitive))
        {
            return true;
        }
        return false;
    }
    else if(m_filter == FilterUnwatched)
    {
        return item->m_lastplayedtime.isEmpty();
    }
    else
    {
        return true;
    }
}

bool MediaListModel::isYbeforeX(MediaItem *x, MediaItem *y)
{
    if((m_sort == SortByAccessTime)||(m_sort == SortByCreationTime))
    {
        QDateTime xtime, ytime;
        if(m_sort == SortByAccessTime)
        {
            xtime = QDateTime::fromString(x->m_lastplayedtime, Qt::ISODate);
            ytime = QDateTime::fromString(y->m_lastplayedtime, Qt::ISODate);
        }
        else
        {
            xtime = QDateTime::fromString(x->m_addedtime, Qt::ISODate);
            ytime = QDateTime::fromString(y->m_addedtime, Qt::ISODate);
        }
        return(ytime > xtime);
    }
    else if(m_sort == SortByTitle)
    {
        QString xtitle, ytitle;
        xtitle = x->m_title;
        ytitle = y->m_title;
        return(ytitle < xtitle);
    }
    else if(m_sort == SortByTrackNum)
    {
        QString xtrack, ytrack;
        xtrack = x->m_tracknum;
        ytrack = y->m_tracknum;
        return(ytrack < xtrack);
    }
    else if(m_sort == SortByURNList)
    {
        int xidx, yidx;
        xidx = urnSortList.indexOf(x->m_urn);
        yidx = urnSortList.indexOf(y->m_urn);
        if(yidx < 0)
            return false;
        if(xidx < 0)
            return true;
        return(yidx < xidx);
    }
    return false;
}

void MediaListModel::sortItems(QList<MediaItem *> &list, int sort)
{
    if(list.isEmpty())
        return;

    if((sort == SortByAccessTime)||(sort == SortByCreationTime)||
       (sort == SortByAddedTime)||(sort == SortByUnwatched))
    {
        QMap<QDateTime, MediaItem *> map;

        /* create a QMap of all the items mapped to their timestamps */
        for(int i = 0; i < list.count(); i++)
        {
            QDateTime ptime;
            if((sort == SortByAccessTime)||(sort == SortByUnwatched))
                ptime = QDateTime::fromString(list[i]->m_lastplayedtime, Qt::ISODate);
            else if((sort == SortByAddedTime)||(sort ==SortByFavorite))
                ptime = QDateTime::fromString(list[i]->m_addedtime, Qt::ISODate);
            else if(!list[i]->m_creationtime.isEmpty())
                ptime = QDateTime::fromString(list[i]->m_creationtime, Qt::ISODate);
            else
                ptime = QDateTime::fromString(list[i]->m_addedtime, Qt::ISODate);
            map.insertMulti(ptime, list[i]);
        }

        /* pull a list of the unique keys, in case two items have the same timestamp */
        QList<QDateTime> timestamps = map.uniqueKeys();
        qSort( timestamps.begin(), timestamps.end(), qGreater<QDateTime>() );
        list.clear();

        if(sort == SortByUnwatched)
        {
            /* pop the values for each sorted key */
            for(int i = timestamps.count()-1; i >= 0; i--)
            {
                QList<MediaItem *> temp = map.values(timestamps[i]);
                for(int j = 0; j < temp.count(); j++)
                    list << temp[j];
            }
        }
        else
        {
            /* pop the values for each sorted key */
            for(int i = 0; i < timestamps.count(); i++)
            {
                QList<MediaItem *> temp = map.values(timestamps[i]);
                for(int j = temp.count() - 1; j >= 0; j--)
                    list << temp[j];
            }
        }

        if(sort ==SortByFavorite)
        {
            QList<MediaItem *> temp;
            for(int i = 0; i < list.count(); i++)
            {
                if(list[i]->m_favorite)
                {
                    temp << list[i];
                    list.removeAt(i);
                }
            }
            temp += list;
            list.clear();
            list = temp;
        }
    }
    else if(sort == SortByTitle)
    {
        QMap<QString, MediaItem *> map;

        /* create a QMap of all the items mapped to their titles */
        for(int i = 0; i < list.count(); i++)
        {
            map.insertMulti(list[i]->m_title, list[i]);
        }

        /* pull a list of the unique keys, in case two items have the same title */
        QStringList titles = map.uniqueKeys();
        qSort( titles );
        list.clear();

        /* pop the values for each sorted key */
        for(int i = 0; i < titles.count(); i++)
        {
            list << map.values(titles[i]);
        }
    }
    else if(sort == SortByTrackNum)
    {
        QMap<int, MediaItem *> map;

        /* create a QMap of all the items mapped to their track nums */
        for(int i = 0; i < list.count(); i++)
        {
            map.insertMulti(list[i]->m_tracknum, list[i]);
        }

        /* pull a list of the unique keys, in case two items have the same tracknum */
        QList<int> tracks = map.uniqueKeys();
        qSort( tracks );
        list.clear();

        /* pop the values for each sorted key */
        for(int i = 0; i < tracks.count(); i++)
        {
            list << map.values(tracks[i]);
        }
    }
    else if(sort == SortByURNList)
    {
        /* the assumption here is that the list given is the entire list */
        /* we will pay no attention to what's already in mediaItemsList */
        /* we further assume that the list only contains items that are in */
        /* urnSortList, though not necessarily in the exact amount */
        QHash<QString, MediaItem *> hash;

        /* create a hash of items by urn for what we have available */
        for(int i = 0; i < list.count(); i++)
            if(!hash.contains(list[i]->m_urn))
                hash.insert(list[i]->m_urn, list[i]);

        list.clear();

        /* fill out the list with what's available */
        for(int i = 0; i < urnSortList.count(); i++)
            if(hash.contains(urnSortList[i]))
                list << hash[urnSortList[i]];
    }
}

QList<MediaItem *> MediaListModel::filterItems(const QList<MediaItem *> list)
{
    QList<MediaItem *> displayList;
    if(m_filter == FilterAll)
    {
        displayList = list;
    }
    else
    {
        QList<MediaItem *> temp;
        for(int i = 0; i < list.count(); i++)
            if(isItemDisplayed(list[i]))
                temp << list[i];
        displayList = temp;
    }
    return displayList;
}

void MediaListModel::resortItems(QList<MediaItem *> &list, const QModelIndex &parent)
{
    /* if either limit or sort isn't set, we just store the data and leave */
    if((m_limit < 0)||(m_sort < 0))
    {
        return;
    }

    redisplay();
}

void MediaListModel::displayNewItems(QList<MediaItem *> &list, const QModelIndex &parent)
{
    /* if either limit or sort isn't set, we just store the data and leave */
    if((m_limit < 0)||(m_sort < 0))
    {
        if(!list.isEmpty())
            mediaItemsList += list;
        emit countChanged(mediaItemsDisplay.count());
        emit totalChanged(mediaItemsList.count());
        return;
    }

    /* if this is the first sort just do a full redisplay */
    /* this code causes the entire ListView or GridView to repopulate */
    /* which can look terrible if it happens more than once */
    if(firstsort)
    {
        if(!list.isEmpty())
            mediaItemsList += list;
        redisplay();
        emit countChanged(mediaItemsDisplay.count());
        emit totalChanged(mediaItemsList.count());
        return;
    }

    /* the code below integrates new items into the existing list in a manner */
    /* that minimizes changes to what's already on the screen, this makes things */
    /* look smoother while items are being frequently added or removed */

    /* at this point we assume that the existing items have */
    /* already been sorted and filtered */
    if(m_sort == SortAsIs)
    {
        /* sorting as-is is easiest, just mash the two lists together */
        mediaItemsList += list;
        QList<MediaItem *> displayList = filterItems(list);
        int lastidx = mediaItemsDisplay.count();
        int listcount = displayList.count();
        int limit = lastidx + listcount;
        if((m_limit > 0)&&(m_limit < limit))
        {
            limit = m_limit;
            listcount = limit - lastidx;
        }
        if(listcount > 0)
        {
            beginInsertRows(parent, lastidx, limit-1);
            for (int i = 0; i < listcount; ++i)
                mediaItemsDisplay.insert(i+lastidx, list[i]);
            endInsertRows();
        }
    }
    else
    {
        if(m_sort == SortByURNList)
        {
            /* sorting by urn list is the hardest, since there can be */
            /* multiple instances of the same items, which may or may not */
            /* already be in mediaItemsList */
            /* the assumption here is that mediaItemsList is what's already */
            /* on screen, which fills in some portion of urnSortList, and that */
            /* urnSortList has missing pieces that need to be filled as fully */
            /* as possible with the items in list */

            QHash<QString, MediaItem *> hash;

            /* create a hash of items by urn for what we have available */
            for(int i = 0; i < mediaItemsList.count(); i++)
                if(!hash.contains(mediaItemsList[i]->m_urn))
                    hash.insert(mediaItemsList[i]->m_urn, mediaItemsList[i]);

            for(int i = 0; i < list.count(); i++)
                if(!hash.contains(list[i]->m_urn))
                    hash.insert(list[i]->m_urn, list[i]);

            int j = 0;
            for(int i = 0; i < urnSortList.count(); i++)
            {
                /* if this index has the right item, increment and continue */
                if((j < mediaItemsList.count())&&(mediaItemsList[j]->m_urn == urnSortList[i]))
                {
                    j++;
                    continue;
                }

                /* if the index has the wrong item, but we have it in the list */
                /* insert it and continue */
                if(hash.contains(urnSortList[i]))
                {
                    beginInsertRows(parent, j, j);
                    mediaItemsList.insert(j, hash[urnSortList[i]]);
                    mediaItemsDisplay.insert(j, hash[urnSortList[i]]);
                    endInsertRows();
                    j++;
                    continue;
                }
            }
        }
        else
        {
            /* for all other sort types we can combine the existing and new lists */
            /* into a single sorted list using simple insertions, this causes */
            /* the ListView to appear to expand organically on screen as new items */
            /* are inserted */

            sortItems(list, m_sort);
            int j = 0;
            for(int i = 0; i < list.count(); i++)
            {
                while((j < mediaItemsList.count())&&!isYbeforeX(mediaItemsList[j], list[i])) j++;
                mediaItemsList.insert(j, list[i]);
                j++;
            }
            QList<MediaItem *> displayList = filterItems(list);
            j = 0;
            for(int i = 0; i < displayList.count(); i++)
            {
                while((j < mediaItemsDisplay.count())&&!isYbeforeX(mediaItemsDisplay[j], displayList[i])) j++;
                beginInsertRows(parent, j, j);
                mediaItemsDisplay.insert(j, displayList[i]);
                endInsertRows();
                j++;
            }
        }

        if((m_limit > 0)&&(mediaItemsDisplay.count() > m_limit))
        {
            beginRemoveRows(parent, m_limit, mediaItemsDisplay.count()-1);
            for(int i = mediaItemsDisplay.count()-1; i >= m_limit; i--)
                mediaItemsDisplay.removeAt(i);
            endRemoveRows();
        }
    }

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void MediaListModel::redisplay(const QModelIndex &parent)
{
    firstsort = false;
    sortItems(mediaItemsList, m_sort);
    QList<MediaItem *> filterlist = filterItems(mediaItemsList);
    QList<MediaItem *> displaylist = (m_limit > 0)?(filterlist.mid(0, m_limit)):filterlist;
    if(!mediaItemsDisplay.isEmpty())
    {
        beginRemoveRows(parent, 0, mediaItemsDisplay.count()-1);
        mediaItemsDisplay.clear();
        endRemoveRows();
    }
    if(!displaylist.isEmpty())
    {
        beginInsertRows(parent, 0, displaylist.count()-1);
        mediaItemsDisplay = displaylist;
        endInsertRows();
    }
}

void MediaListModel::itemsRemoved(const QStringList &ids)
{
    QList<MediaItem *> temp = mediaItemsDisplay;
    int count = 0;
    /* remove them all from the displayed list */
    for (int i = 0; i < temp.count(); i++)
        if(ids.contains(temp[i]->m_id))
        {
            removeRows(i-count, 1);
            count++;
        }

    temp = mediaItemsList;
    count = 0;
    /* remove them all from the master list */
    for (int i = 0; i < temp.count(); i++)
        if(ids.contains(temp[i]->m_id))
        {
            mediaItemsList.removeAt(i-count);
            count++;
        }

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

