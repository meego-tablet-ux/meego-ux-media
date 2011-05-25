/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef DOCUMENTDATABASE_H
#define DOCUMENTDATABASE_H

#include "mediadatabase.h"
#include "mediaitem.h"
#include "thumbnailer.h"

class DocumentDatabase : public MediaDatabase {
    Q_OBJECT

public:
    static DocumentDatabase *instance();
    DocumentDatabase(QObject *parent = 0);
    ~DocumentDatabase();

    void requestThumbnail(MediaItem *item);
    void requestItem(QString identifier);

public slots:
    void trackerDocumentAdded(int sid);
    void trackerGetDocumentsFinished(QDBusPendingCallWatcher *call);
    void thumbReady(const MediaItem *item);

private:
    static DocumentDatabase *documentDatabaseInstance;
    Thumbnailer thumb;

    /* tracker calls */
    void trackerGetDocuments(const int offset, const int limit);
    void trackerAddItems(int type, QVector<QStringList> trackerreply, bool priority=false);
};

#endif // DOCUMENTDATABASE_H
