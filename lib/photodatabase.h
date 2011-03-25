/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef PHOTODATABASE_H
#define PHOTODATABASE_H

#include "mediadatabase.h"
#include "mediaitem.h"
#include "thumbnailer.h"

class PhotoDatabase : public MediaDatabase {
    Q_OBJECT

public:
    static PhotoDatabase *instance();
    PhotoDatabase(QObject *parent = 0);
    ~PhotoDatabase();

    void requestThumbnail(MediaItem *item);
    void requestItem(int type, QString identifier);
    void saveAlbum(QList<MediaItem *> &list, const QString &title);
    QList<MediaItem *> loadAlbum(const QString &title);
    MediaItem* getAlbumItem(const QString &title);
    void destroyItem(MediaItem *item);
    void setCoverArt(const QString &title, const QString &thumburi);

public slots:
    void trackerPhotoAdded(int sid);
    void trackerAlbumAdded(int sid);
    void trackerGetPhotosFinished(QDBusPendingCallWatcher *call);
    void thumbReady(const MediaItem *item);

private:
    static PhotoDatabase *photoDatabaseInstance;
    Thumbnailer thumb;
    QHash<QString, MediaItem *> albumItemsHash;

    /* tracker calls */
    void trackerGetPhotos(const int offset, const int limit);
    void trackerAddItems(int type, QVector<QStringList> trackerreply, bool priority=false);
    void processPhoto(MediaItem *item);
    void processAlbum(MediaItem *item);
};

#endif // PHOTODATABASE_H
