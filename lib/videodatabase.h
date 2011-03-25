/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef VIDEODATABASE_H
#define VIDEODATABASE_H

#include "mediadatabase.h"
#include "mediaitem.h"
#include "thumbnailer.h"

class VideoDatabase : public MediaDatabase {
    Q_OBJECT

public:
    static VideoDatabase *instance();
    VideoDatabase(QObject *parent = 0);
    ~VideoDatabase();

    void requestThumbnail(MediaItem *item);
    void requestItem(QString identifier);

public slots:
    void trackerVideoAdded(int sid);
    void trackerGetVideosFinished(QDBusPendingCallWatcher *call);
    void thumbReady(const MediaItem *item);

private:
    static VideoDatabase *videoDatabaseInstance;
    Thumbnailer thumb;

    /* tracker calls */
    void trackerGetVideos(const int offset, const int limit);
    void trackerAddItems(int type, QVector<QStringList> trackerreply, bool priority=false);
};

#endif // VIDEODATABASE_H
