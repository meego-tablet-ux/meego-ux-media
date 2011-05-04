/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDir>
#include <QDebug>
#include "musiclistmodel.h"
#include "musicdatabase.h"

MusicListModel::MusicListModel(QObject *parent)
    : MediaListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(MediaItem::ID, "itemid");
    roles.insert(MediaItem::URN, "urn");
    roles.insert(MediaItem::ItemType, "itemtype");
    roles.insert(MediaItem::URI, "uri");
    roles.insert(MediaItem::ThumbURI, "thumburi");
    roles.insert(MediaItem::AlbumThumbURI, "albumthumburi");
    roles.insert(MediaItem::ArtistThumbURI, "artistthumburi");
    roles.insert(MediaItem::Title, "title");
    roles.insert(MediaItem::Artist, "artist");
    roles.insert(MediaItem::Album, "album");
    roles.insert(MediaItem::TrackNum, "tracknum");
    roles.insert(MediaItem::Length, "length");
    roles.insert(MediaItem::CreationTime, "creationtime");
    roles.insert(MediaItem::AddedTime, "addedtime");
    roles.insert(MediaItem::LastPlayedTime, "lastplayedtime");
    roles.insert(MediaItem::Favorite, "favorite");
    roles.insert(MediaItem::RecentlyAdded, "recentlyadded");
    roles.insert(MediaItem::RecentlyViewed, "recentlyviewed");
    roles.insert(MediaItem::Index, "index");
    roles.insert(MediaItem::Virtual, "isvirtual");
    roles.insert(MediaItem::PlayStatus, "playstatus");
    roles.insert(MediaItem::UserContent, "usercontent");
    setRoleNames(roles);

    m_type = -1;
    m_mix = 0;
    shuffleindex = 0;

    connect(MusicDatabase::instance(),SIGNAL(itemAvailable(const QString)),this,SIGNAL(itemAvailable(const QString)));
    connect(MusicDatabase::instance(),SIGNAL(songItemAvailable(const QString)),this,SIGNAL(songItemAvailable(const QString)));
}

MusicListModel::~MusicListModel()
{
    clearData();
}

void MusicListModel::connectSignals(bool added, bool changed, bool removed)
{
    if(added)
        connect(MusicDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));

    if(changed)
        connect(MusicDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));

    if(removed)
        connect(MusicDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));
}

void MusicListModel::clearData()
{
    /* don't listen to new items coming from the database, we're starting over */
    disconnect(MusicDatabase::instance(),SIGNAL(itemsAdded(const QList<MediaItem *> *)),this,SLOT(itemsAdded(const QList<MediaItem *> *)));
    disconnect(MusicDatabase::instance(),SIGNAL(itemsChanged(const QStringList &, int)),this,SLOT(itemsChanged(const QStringList &, int)));
    disconnect(MusicDatabase::instance(),SIGNAL(itemsRemoved(const QStringList &)),this,SLOT(itemsRemoved(const QStringList &)));

    disable_filter = false;
    m_albums.clear();
    urnSortList.clear();
    if(!mediaItemsDisplay.isEmpty())
    {
        /* formally remove all the items from the list */
        beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
        mediaItemsList.clear();
        mediaItemsDisplay.clear();
        endRemoveRows();
    }
}

void MusicListModel::setType(const int type)
{
    clearData();
    m_type = type;
    hiddenURNsList.clear();
    emit typeChanged(m_type);
    m_default_sort = SortAsIs;

    /* for these items we need further input to proceed */
    switch(m_type) {
    case ListofSongsForAlbum:
        m_default_sort = SortByTrackNum;
        setSort(SortByTrackNum);
        if(!m_album.isEmpty())
            setAlbum(m_album);
        return;
    case ListofAlbumsForArtist:
        m_default_sort = SortByTitle;
    case ListofSongsForArtist:
    case ListofOrphanSongsForArtist:
        if(!m_artist.isEmpty())
            setArtist(m_artist);
        return;
    case MusicPlaylist:
        m_default_sort = SortByURNList;
        setSort(SortByDefault);
        connectSignals(false, true, true);
        if(!m_playlist.isEmpty())
            setPlaylist(m_playlist);
        return;
    case MixedList:
        if(m_mix != 0)
            setMix(m_mix);
        return;
    case NowPlaying:
        setSort(SortByDefault);
        connectSignals(false, true, true);
        return;
    case Editor:
        return;
    }

    /* for basic lists, grab a new database snapshot */
    QList<MediaItem *> tempList = MusicDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    if(m_type == ListofSongs)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isSong())
                newItemList << tempList[i];
    }
    else if(m_type == ListofUserSongs)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isSong()&&tempList[i]->m_isusercontent)
                newItemList << tempList[i];
    }
    else if(m_type == ListofAlbums)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isMusicAlbum()&&!tempList[i]->isVirtual())
                newItemList << tempList[i];
    }
    else if(m_type == ListofArtists)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isMusicArtist())
                newItemList << tempList[i];
    }
    else if(m_type == ListofPlaylists)
    {
        for(int i = 0; i < tempList.count(); i++)
            if(tempList[i]->isMusicPlaylist())
                newItemList << tempList[i];
    }
    else if(m_type == ListofRecentlyPlayed)
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

void MusicListModel::setArtist(const QString artist)
{
    m_artist = artist;
    if((m_type != ListofSongsForArtist)&&(m_type != ListofOrphanSongsForArtist)&&(m_type != ListofAlbumsForArtist))
    {
        emit artistChanged(m_artist);
        return;
    }

    clearData();

    /* grab a new database snapshot */
    QList<MediaItem *> tempList = MusicDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    if(m_type == ListofSongsForArtist)
    {
        for(int i = 0; i < tempList.count(); i++)
            if((tempList[i]->isSong())&&(tempList[i]->m_artist.contains(m_artist)))
                newItemList << tempList[i];
    }
    else if(m_type == ListofOrphanSongsForArtist)
    {
        for(int i = 0; i < tempList.count(); i++)
            if((tempList[i]->isSong())&&(tempList[i]->m_artist.contains(m_artist))&&
               tempList[i]->m_album.isEmpty())
                newItemList << tempList[i];
    }
    else
    {
        for(int i = 0; i < tempList.count(); i++)
            if((tempList[i]->isSong())&&
               (tempList[i]->m_artist.contains(m_artist))&&
                !m_albums.contains(tempList[i]->m_album))
                m_albums << tempList[i]->m_album;

        for(int i = 0; i < tempList.count(); i++)
            if((tempList[i]->isMusicAlbum())&&
               ((m_albums.contains(tempList[i]->m_title))||
                tempList[i]->m_artist.contains(m_artist)))
                newItemList << tempList[i];
    }

    if(!newItemList.isEmpty())
    {
        /* formally add the new items */
        displayNewItems(newItemList);
    }

    connectSignals(true, true, true);
    emit artistChanged(m_artist);
    emit typeChanged(m_type);
}

void MusicListModel::setAlbum(const QString album)
{
    m_album = album;
    if(m_type != ListofSongsForAlbum)
    {
        emit albumChanged(m_album);
        return;
    }

    clearData();

    /* grab a new database snapshot */
    QList<MediaItem *> tempList = MusicDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    for(int i = 0; i < tempList.count(); i++)
        if((tempList[i]->isSong())&&(tempList[i]->m_album == m_album))
            newItemList << tempList[i];

    if(!newItemList.isEmpty())
    {
        /* formally add the new items */
        displayNewItems(newItemList);
    }

    connectSignals(true, true, true);
    emit albumChanged(m_album);
}

void MusicListModel::setPlaylist(const QString playlist)
{
    m_playlist = playlist;
    if(m_type != MusicPlaylist)
    {
        emit playlistChanged(m_playlist);
        return;
    }

    clearData();
    m_filter = FilterAll;
    disable_filter = true;

    if(!playlist.isEmpty())
    {
        urnSortList = MusicDatabase::instance()->loadPlaylist(m_playlist);
        QList<MediaItem *> newItemList = MusicDatabase::instance()->getItemsByURN(urnSortList);

        if(!newItemList.isEmpty())
        {
            /* formally add the new items */
            displayNewItems(newItemList);
        }
    }

    connectSignals(true, true, true);
    emit playlistChanged(m_playlist);
}

void MusicListModel::setURNs(const QStringList urns)
{
    if(m_type != Editor)
    {
        qDebug() << "You can only set the urn for Editor models";
        return;
    }

    QList<MediaItem *> tempList = MusicDatabase::instance()->getItemsByURN(urns);
    if(!tempList.isEmpty())
    {
        m_urns = urns;
        if(!mediaItemsDisplay.isEmpty())
        {
            beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
            mediaItemsList.clear();
            mediaItemsDisplay.clear();
            endRemoveRows();
        }

        beginInsertRows(QModelIndex(), 0, tempList.count()-1);
        mediaItemsList = tempList;
        mediaItemsDisplay = tempList;
        endInsertRows();
        emit countChanged(tempList.count());
        emit totalChanged(tempList.count());
        emit urnsChanged(m_urns);
    }
}

void MusicListModel::setMix(int mix)
{
    m_mix = mix;
    if(m_type != MixedList)
    {
        emit mixChanged(m_mix);
        return;
    }

    clearData();

    qDebug() << "Mixed list mixing things: " << m_mix;
    /* for basic lists, grab a new database snapshot */
    QList<MediaItem *> tempList = MusicDatabase::instance()->getSnapshot();
    QList<MediaItem *> newItemList;

    for(int i = 0; i < tempList.count(); i++)
    {
        if(tempList[i]->isSong()&&(m_mix&Songs))
            newItemList << tempList[i];
        if(tempList[i]->isMusicAlbum()&&(m_mix&Albums))
            newItemList << tempList[i];
        if(tempList[i]->isMusicArtist()&&(m_mix&Artists))
            newItemList << tempList[i];
        if(tempList[i]->isMusicPlaylist()&&(m_mix&Playlists))
            newItemList << tempList[i];
    }

    if(!newItemList.isEmpty())
    {
        /* formally add the new items */
        displayNewItems(newItemList);
    }

    connectSignals(true, true, true);
    emit mixChanged(m_mix);
}

QList<MediaItem *> MusicListModel::unwrapItem(const QList<MediaItem *> &snapshot, MediaItem *item)
{
    QList<MediaItem *> newItemList;
    if(item->isSong())
    {
        newItemList << item;
    }
    else if(item->isMusicAlbum())
    {
        /* grab all the songs that belong to this album */
        for(int i = 0; i < snapshot.count(); i++)
            if((snapshot[i]->isSong())&&(snapshot[i]->m_album == item->m_title))
                newItemList << snapshot[i];
        /* sort the songs by tracknum */
        sortItems(newItemList, SortByTrackNum);
    }
    else if(item->isMusicArtist())
    {
        QList<MediaItem *> albumItemList, songItemList;
        QStringList albumNameList;
        /* grab all the album names that belong to this artist's songs */
        for(int i = 0; i < snapshot.count(); i++)
            if((snapshot[i]->isSong())&&(snapshot[i]->m_artist.contains(item->m_title)))
                albumNameList << snapshot[i]->m_album;

        /* grab all the albums named in the list */
        for(int i = 0; i < snapshot.count(); i++)
            if((snapshot[i]->isMusicAlbum())&&(albumNameList.contains(snapshot[i]->m_title)))
                albumItemList << snapshot[i];
        /* sort the albums by title */
        sortItems(albumItemList, SortByTitle);

        for(int i = 0; i < albumItemList.count(); i++)
        {
            songItemList.clear();
            /* grab all the songs that belong to this album and artist */
            for(int j = 0; j < snapshot.count(); j++)
                if((snapshot[j]->isSong())&&(snapshot[j]->m_album == albumItemList[i]->m_title)&&
                   (snapshot[j]->m_artist.contains(item->m_title)))
                    songItemList << snapshot[j];
            /* sort the songs by tracknum */
            sortItems(songItemList, SortByTrackNum);
            newItemList += songItemList;
        }
    }
    else if(item->isMusicPlaylist())
    {
        /* grab all the songs that belong to this playlist */
        /* recursively unwrap any albums or artists so that the output */
        /* list is just songs */
        QList<MediaItem *> playlistItems;
        QStringList playlistURNs;
        playlistURNs = MusicDatabase::instance()->loadPlaylist(item->m_title);
        playlistItems = MusicDatabase::instance()->getItemsByURN(playlistURNs);
        for(int i = 0; i < playlistItems.count(); i++)
            if(playlistItems[i]->isSong())
                newItemList << playlistItems[i];
            else
                newItemList << unwrapItem(snapshot, playlistItems[i]);
    }
    return newItemList;
}

void MusicListModel::savePlaylist(const QString &playlist)
{
    if(m_type != MusicPlaylist && m_type != NowPlaying)
    {
        qDebug() << "You can't save this model";
        return;
    }

    if(playlist.isEmpty())
    {
        QDateTime time = QDateTime();
        time.setTimeSpec(Qt::LocalTime);
        m_playlist = time.currentDateTime().toString(Qt::ISODate);
    }
    else
        m_playlist = playlist;

    MusicDatabase::instance()->savePlaylist(mediaItemsList, m_playlist);
    connectSignals(false, true, true);
    emit playlistChanged(m_playlist);
}

QStringList MusicListModel::getPlaylistThumbs(int num)
{
    QStringList list;
    if(m_type == MusicPlaylist)
    {
        for(int i = 0; i < mediaItemsList.count(); i++)
            if(mediaItemsList[i]->m_thumburi_exists)
            {
                list << mediaItemsList[i]->m_thumburi;
                if(list.count() >= num)
                    break;
            }
    }
    return list;
}

void MusicListModel::requestItem(int type, QString urn)
{
    if(!urn.isEmpty())
        MusicDatabase::instance()->requestItem(type, urn);
}

QString MusicListModel::getURNFromIndex(const int index)
{
    if ((index < mediaItemsDisplay.count()) && (index >= 0))
        return mediaItemsDisplay[index]->m_urn;
    return QString();
}

void MusicListModel::itemsAdded(const QList<MediaItem *> *list)
{
    QList<MediaItem *> newItemList;

    if(m_type == ListofSongs)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isSong())
                newItemList << list->at(i);
    }
    else if(m_type == ListofUserSongs)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isSong()&&list->at(i)->m_isusercontent)
                newItemList << list->at(i);
    }
    else if(m_type == ListofAlbums)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isMusicAlbum()&&!list->at(i)->isVirtual())
                newItemList << list->at(i);
    }
    else if(m_type == ListofArtists)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isMusicArtist())
                newItemList << list->at(i);
    }
    else if(m_type == ListofPlaylists)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->isMusicPlaylist())
                newItemList << list->at(i);
    }
    else if(m_type == MixedList)
    {
        for(int i = 0; i < list->count(); i++)
        {
            if(list->at(i)->isSong()&&(m_mix&Songs))
                newItemList << list->at(i);
            if(list->at(i)->isMusicAlbum()&&(m_mix&Albums))
                newItemList << list->at(i);
            if(list->at(i)->isMusicArtist()&&(m_mix&Artists))
                newItemList << list->at(i);
            if(list->at(i)->isMusicPlaylist()&&(m_mix&Playlists))
                newItemList << list->at(i);
        }
    }
    else if((m_type == ListofAlbumsForArtist)&&(!m_artist.isEmpty()))
    {
        /* We need a full snapshot because we could discover we need an */
        /* album that's already been added to the list */
        QList<MediaItem *> tempList = MusicDatabase::instance()->getSnapshot();

        for(int i = 0; i < list->count(); i++)
            if((list->at(i)->isSong())&&
               (list->at(i)->m_artist.contains(m_artist))&&
                !m_albums.contains(list->at(i)->m_album))
                m_albums << list->at(i)->m_album;

        for(int i = 0; i < tempList.count(); i++)
            if((tempList[i]->isMusicAlbum())&&
               ((m_albums.contains(tempList[i]->m_title))||
                tempList[i]->m_artist.contains(m_artist))&&
                !mediaItemsList.contains(tempList[i]))
                newItemList << tempList[i];
    }
    else if((m_type == ListofSongsForAlbum)&&(!m_album.isEmpty()))
    {
        for(int i = 0; i < list->count(); i++)
            if((list->at(i)->isSong())&&(list->at(i)->m_album == m_album))
                newItemList << list->at(i);
    }
    else if((m_type == ListofSongsForArtist)&&(!m_artist.isEmpty()))
    {
        for(int i = 0; i < list->count(); i++)
            if((list->at(i)->isSong())&&(list->at(i)->m_artist.contains(m_artist)))
                newItemList << list->at(i);
    }
    else if((m_type == ListofOrphanSongsForArtist)&&(!m_artist.isEmpty()))
    {
        for(int i = 0; i < list->count(); i++)
            if((list->at(i)->isSong())&&(list->at(i)->m_artist.contains(m_artist))&&
               list->at(i)->m_album.isEmpty())
                newItemList << list->at(i);
    }
    else if(m_type == ListofRecentlyPlayed)
    {
        for(int i = 0; i < list->count(); i++)
            if(list->at(i)->m_recentlyviewed&&!list->at(i)->isVirtual())
                newItemList << list->at(i);
    }
    else if(m_type == MusicPlaylist)
    {
        for(int i = 0; i < list->count(); i++)
            if(urnSortList.contains(list->at(i)->m_urn))
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

void MusicListModel::itemsChanged(const QStringList &ids, int reason)
{
    int i1=-1, i2=-1;

    if((m_type == ListofRecentlyPlayed)&&(reason == MusicDatabase::Viewed))
    {
        /* first add in any new recently viewed items */
        QList<MediaItem *> newItemList = MusicDatabase::instance()->getItemsByID(ids);
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
    else if((reason == MusicDatabase::Unviewed)&&(m_type == ListofRecentlyPlayed))
    {
        itemsRemoved(ids);
        return;
    }
    else if((((m_filter == FilterFavorite)||(m_sort == SortByFavorite))&&
             (reason == MusicDatabase::Favorited))||
            (((m_filter == FilterViewed)||(m_filter == FilterUnwatched)||
              (m_sort == SortByAccessTime)||(m_sort == SortByUnwatched))&&
             (reason == MusicDatabase::Viewed))||
            ((m_sort == SortByCreationTime)&&(reason == MusicDatabase::Contents)))
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

void MusicListModel::setFavorite(const QStringList &ids, bool val)
{
    MusicDatabase::instance()->setFavoriteMulti(ids, val);
}

void MusicListModel::setViewed(const QStringList &ids)
{
    MusicDatabase::instance()->setViewedMulti(ids);
}

void MusicListModel::setPlayStatus(const QString &id, int playstatus)
{
    MusicDatabase::instance()->setPlayStatus(id, playstatus);
}

void MusicListModel::addItems(const QStringList &ids)
{
    if((m_type != MusicPlaylist)&&(m_type != NowPlaying))
    {
        qDebug() << "You can't add items to this model";
        return;
    }

    QList<MediaItem *> newItemList = MusicDatabase::instance()->getItemsByID(ids);

    if(m_type == NowPlaying)
    {
        QList<MediaItem *> tempList = MusicDatabase::instance()->getSnapshot();
        QList<MediaItem *> newItemListFull;

        /* first grab the item we need */
        for(int i = 0; i < newItemList.count(); i++)
        {
            /* if this is a playlist, artist, or album, get its songs only */
            newItemListFull = unwrapItem(tempList, newItemList[i]);

            /* filter out any items already in the queue */
            filterDuplicates(mediaItemsList, newItemListFull);

            /* add any items left over */
            if(!newItemListFull.isEmpty())
            {
                displayNewItems(newItemListFull);
            }
        }
    }
    else if(m_type == MusicPlaylist)
    {
        for(int i = 0; i < newItemList.count(); i++)
            urnSortList << newItemList[i]->m_urn;

        displayNewItems(newItemList);
    }
}

void MusicListModel::removeItems(const QStringList &ids)
{
    if((m_type != MusicPlaylist)&&(m_type != NowPlaying))
    {
        qDebug() << "You can't remove items from this model";
        return;
    }

    QList<MediaItem *> deleteItemList;

    for (int i = 0; i < mediaItemsList.count(); i++)
        if(ids.contains(mediaItemsList[i]->m_id))
            deleteItemList << mediaItemsList[i];

    for (int i = 0; i < deleteItemList.count(); i++)
    {
        int index = mediaItemsDisplay.indexOf(deleteItemList[i]);
        if(index >= 0)
        {
            removeRows(index, 1);
        }
        mediaItemsList.removeAll(deleteItemList[i]);
    }

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void MusicListModel::removeIndex(const int index)
{
    if(m_type != MusicPlaylist)
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
    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

void MusicListModel::destroyItem(const int index)
{
    if((index < 0)||(index >= mediaItemsDisplay.count()))
    {
        qDebug() << "removeIndex: Invalid Index";
        return;
    }

    MusicDatabase::instance()->destroyItem(mediaItemsDisplay[index]);
}

void MusicListModel::destroyItemByID(const QString &id)
{
    for (int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_id == id)
            MusicDatabase::instance()->destroyItem(mediaItemsList[i]);
}

void MusicListModel::destroyItemsByID(const QStringList &ids)
{
    for (int i = 0; i < ids.count(); i++)
        destroyItemByID(ids[i]);
}

void MusicListModel::clear()
{
    if(m_type != MusicPlaylist && m_type != NowPlaying && m_type != ListofRecentlyPlayed)
    {
        qDebug() << "You can't remove items from this model";
        return;
    }

    urnSortList.clear();
    beginRemoveRows(QModelIndex(), 0, mediaItemsDisplay.count()-1);
    mediaItemsList.clear();
    mediaItemsDisplay.clear();
    endRemoveRows();

    if(m_type == ListofRecentlyPlayed)
        MusicDatabase::instance()->clearHistory();

    emit countChanged(mediaItemsDisplay.count());
    emit totalChanged(mediaItemsList.count());
}

QVariant MusicListModel::data(const QModelIndex &index, int role) const
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
            MusicDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]);
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
            MusicDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]->albumitem);
            return "";
        }
    }

    if (role == MediaItem::ArtistThumbURI)
    {
        if((mediaItemsDisplay[index.row()]->artistitem == NULL)||
           (mediaItemsDisplay[index.row()]->artistitem->m_thumburi_ignore))
        {
            return "";
        }
        else if(mediaItemsDisplay[index.row()]->artistitem->m_thumburi_exists)
        {
            return mediaItemsDisplay[index.row()]->artistitem->getThumbURI();
        }
        else
        {
            MusicDatabase::instance()->requestThumbnail(mediaItemsDisplay[index.row()]->artistitem);
            return "";
        }
    }

    if (role == MediaItem::Title)
        return mediaItemsDisplay[index.row()]->getTitle();

    if (role == MediaItem::Artist)
        return mediaItemsDisplay[index.row()]->getArtist();

    if (role == MediaItem::Album)
        return mediaItemsDisplay[index.row()]->getAlbum();

    if (role == MediaItem::TrackNum)
        return mediaItemsDisplay[index.row()]->getTrackNum();

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

    if (role == MediaItem::Virtual)
        return mediaItemsDisplay[index.row()]->isVirtual();

    if (role == MediaItem::PlayStatus)
        return mediaItemsDisplay[index.row()]->m_playstatus;

    if (role == MediaItem::UserContent)
        return mediaItemsDisplay[index.row()]->m_isusercontent;

    return QVariant();
}

void MusicListModel::changeTitle(QString uri, QString title)
{
    MusicDatabase::instance()->changeTitle(uri, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_uri == uri)
            emit dataChanged(index(i, 0), index(i, 0));
}

void MusicListModel::changeTitleByURN(QString urn, QString title)
{
    MusicDatabase::instance()->changeTitleByURN(urn, title);
    for (int i = 0; i < mediaItemsDisplay.count(); i++)
        if(mediaItemsList[i]->m_urn == urn)
            emit dataChanged(index(i, 0), index(i, 0));
}

/* create a random shuffle path for the play queue */
void MusicListModel::shuffleReset()
{
    int count = mediaItemsDisplay.count();

    /* the count changed, clear the old shuffle path */
    shuffler.clear();
    shuffleindex = 0;

    /* initialize a straight path, with space for the next one */
    for(int i = 0; i < count*2; i++)
        shuffler << i%count;

    /* randomize the current path */
    for(int i = 0; i < count; i++)
        shuffler.swap(rand()%count, rand()%count);

    /* randomize the next path */
    for(int i = 0; i < count; i++)
        shuffler.swap(count+(rand()%count), count+(rand()%count));

    /* if current last = next first, change it */
    if(shuffler[count-1] == shuffler[count])
        shuffler.swap(count, (count*2)-1);
}

/* return the next shuffled index */
int MusicListModel::shuffleIndex(int offset)
{
    if((mediaItemsDisplay.count() <= 0)||(offset < 0)||(offset > 2))
        return -1;

    if(shuffler.count() != mediaItemsDisplay.count()*2)
        shuffleReset();

    return shuffler[shuffleindex + offset];
}

void MusicListModel::shuffleIncrement()
{
    int count = mediaItemsDisplay.count();
    if(shuffler.count() != count*2)
    {
        shuffleReset();
        return;
    }

    /* the new index is dipping into the next path */
    if(++shuffleindex >= count)
    {
        /* move the next path to the current path */
        shuffleindex = 0;
        for(int i = 0; i < count; i++)
            shuffler[i] = shuffler[i+count];

        /* randomize the next path */
        for(int i = 0; i < count; i++)
            shuffler.swap(count+(rand()%count), count+(rand()%count));

        /* if current last = next first, change it */
        if(shuffler[count-1] == shuffler[count])
            shuffler.swap(count, (count*2)-1);
    }
}
