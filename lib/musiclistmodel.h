/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MUSICLISTMODEL_H
#define MUSICLISTMODEL_H

#include <QAbstractListModel>
#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "mediaitem.h"
#include "medialistmodel.h"

class MusicListModel: public MediaListModel
{
    Q_OBJECT
    Q_ENUMS(ModelType)
    Q_ENUMS(MixedType)
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY typeChanged);
    Q_PROPERTY(int mixtypes READ getMix WRITE setMix NOTIFY mixChanged);
    Q_PROPERTY(QString artist READ getArtist WRITE setArtist NOTIFY artistChanged);
    Q_PROPERTY(QString album READ getAlbum WRITE setAlbum NOTIFY albumChanged);
    Q_PROPERTY(QString playlist READ getPlaylist WRITE setPlaylist NOTIFY playlistChanged);
    Q_PROPERTY(QStringList urns READ getURNs WRITE setURNs NOTIFY urnsChanged);
    Q_PROPERTY(int playindex READ getPlayIndex WRITE setPlayIndex NOTIFY playIndexChanged);
    Q_PROPERTY(int playstatus READ getPlayStatus WRITE setPlayStatus NOTIFY playStatusChanged);

public:
    MusicListModel(QObject *parent = 0);
    ~MusicListModel();

    enum ModelType {  ListofSongs = 0,
                      ListofAlbums = 1,
                      ListofArtists = 2,
                      ListofPlaylists = 3,
                      ListofAlbumsForArtist = 4,
                      ListofSongsForAlbum = 5,
                      ListofSongsForArtist = 6,
                      ListofOrphanSongsForArtist = 7,
                      ListofRecentlyPlayed = 8,
                      MusicPlaylist = 9,
                      NowPlaying = 10,
                      Editor = 11,
                      MixedList = 12,
                      ListofUserSongs = 13
                   };

    enum MixedType {  Songs = 1,
                      Albums = 2,
                      Artists = 4,
                      Playlists = 8
                   };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int getType() const
        { return m_type; }
    virtual void setType(const int type);
    int getMix() const
        { return m_mix; }
    virtual void setMix(const int mix);
    QString getArtist() const
        { return m_artist; }
    virtual void setArtist(const QString artist);
    QString getAlbum() const
        { return m_album; }
    virtual void setAlbum(const QString album);
    QString getPlaylist() const
        { return m_playlist; }
    virtual void setPlaylist(const QString playlist);
    QStringList getURNs() const
        { return m_urns; }
    virtual void setURNs(const QStringList urns);
    int getPlayIndex() const
        { return m_playindex; }
    void setPlayIndex(const int index);
    int getPlayStatus() const
        { return m_playstatus; }
    void setPlayStatus(const int status);

public slots:
    /* view calls for all models */
    void setFavorite(const QStringList &ids, bool val);
    void setViewed(const QStringList &ids);
    void destroyItem(const int index);
    void destroyItemByID(const QString &id);
    void destroyItemsByID(const QStringList &ids);
    QStringList getPlaylistThumbs(int num);
    void requestItem(int type, QString urn);
    void changeTitle(QString uri, QString title);
    void changeTitleByURN(QString urn, QString title);
    QString getURNFromIndex(const int index);
    void removeSelected();
    void playAllSongs();

    /* view calls for editable models */
    void addItems(const QStringList &ids);
    void removeItems(const QStringList &ids);
    void removeIndex(const int index);
    void savePlaylist(const QString &title);
    void clear();
    int shuffleIndex(int offset);
    void shuffleIncrement();

signals:
    void typeChanged(const int type);
    void mixChanged(const int mixed);
    void artistChanged(const QString artist);
    void albumChanged(const QString album);
    void playlistChanged(const QString playlist);
    void songChanged(const QString song);
    void urnsChanged(const QStringList urns);
    void playIndexChanged(const int playindex);
    void playStatusChanged(const int playstatus);
    void beginPlayback();

protected slots:
    /* from MusicDatabase signals */
    void itemsAdded(const QList<MediaItem *> *list);
    void itemsChanged(const QStringList &ids, int reason);
    void databaseInitComplete();

protected:
    /* private filter functions which respond to setting the filter property */
    QList<MediaItem *> unwrapItem(const QList<MediaItem *> &snapshot, MediaItem *item);
    void clearData();
    void connectSignals(bool added, bool changed, bool removed);
    void shuffleReset();

    /* properties for this model */
    int m_type;
    int m_mix;
    int m_playindex;
    int m_playstatus;
    QString m_artist;
    QString m_album;
    QString m_playlist;
    QStringList m_urns;
    QStringList m_albums;
    QList<int> shuffler;
    int shuffleindex;
    bool loadplayqueue;
    bool needplaycall;
};

#endif // MUSICLISTMODEL_H
