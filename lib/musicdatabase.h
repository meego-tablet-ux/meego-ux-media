/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MUSICDATABASE_H
#define MUSICDATABASE_H

#include "mediadatabase.h"
#include "mediaitem.h"
#include "mediainfodownloader.h"

class MusicDatabase : public MediaDatabase {
    Q_OBJECT

public:
    static MusicDatabase *instance();
    MusicDatabase(QObject *parent = 0);
    ~MusicDatabase();

    void requestThumbnail(MediaItem *item);
    void requestItem(int type, QString identifier);
    void savePlaylist(QList<MediaItem *> &list, const QString &title);
    QStringList loadPlaylist(const QString &title);
    MediaItem* getArtistItem(const QString &title);

public slots:
    void trackerSongAdded(int sid);
    void trackerPlaylistAdded(int sid);
    void trackerGetMusicFinished(QDBusPendingCallWatcher *call);

private slots:
    void error(QString reqid, QString type, QString info, QString errorString);
    void ready(QString reqid, QString type, QString info, QString data);
    void startThumbnailerLoop();

private:
    static MusicDatabase *musicDatabaseInstance;

    MediaInfoDownloader mediaart;
    bool disable_mediaart;

    /* tracker calls */
    void trackerGetMusic(const int offset, const int limit);
    void processSong(MediaItem *item);
    void enforceUniqueTitles(MediaItem *item);
    void trackerAddItems(int type, QVector<QStringList> trackerreply, int priority=0);
    MediaItem* getAlbumItem(QString artist, QString album);
    /* unique title variables */
    QHash<QString, int> titleCountHash;
    void createPlaylistThumb(QList<MediaItem *> &list, const QString &title);
    void generatePlaylistThumbId(MediaItem *item);
    void requestSongItems(int type, QString identifier);
    int playlistthumbid;

    /* music database hashes for easy lookup */
    QHash<QString, MediaItem *> artistItemHash;
};

#endif // MUSICDATABASE_H
