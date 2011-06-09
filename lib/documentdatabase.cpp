/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QtDBus/QtDBus>
#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include "documentdatabase.h"

#define GCONFKEY_RECENTRANGE "/MeeGo/Media/lastviewed-range-document"

DocumentDatabase *DocumentDatabase::documentDatabaseInstance = NULL;

DocumentDatabase *DocumentDatabase::instance()
{
    if (documentDatabaseInstance)
        return documentDatabaseInstance;
    else {
        documentDatabaseInstance = new DocumentDatabase();
        return documentDatabaseInstance;
    }
}

DocumentDatabase::DocumentDatabase(QObject *parent)
    : MediaDatabase(GCONFKEY_RECENTRANGE, parent)
{
    trackeritems = 500;
    trackerindex = 0;
    targetitemtype = MediaItem::DocumentItem;

    connect(this,SIGNAL(documentItemAdded(int)),this,SLOT(trackerDocumentAdded(int)));
    connect(&thumb,SIGNAL(success(const MediaItem *)),this,SLOT(thumbReady(const MediaItem *)));

    qDebug() << "Initializing the database";
    trackerGetDocuments(trackerindex, trackeritems);
}

DocumentDatabase::~DocumentDatabase()
{
    qDebug() << "~PhotoListModel";
}

void DocumentDatabase::trackerAddItems(int type, QVector<QStringList> trackerreply, bool priority)
{
    QList<MediaItem *> newItemsList;
    QSet<QString> documentMimetypes;
    documentMimetypes
            << "application/msword"
            << "application/vnd.openxmlformats-officedocument.wordprocessingml.document"
            << "application/vnd.openxmlformats-officedocument.wordprocessingml.template"
            << "application/vnd.ms-word.document.macroEnabled.12"
            << "application/vnd.ms-word.template.macroEnabled.12"
            << "application/vnd.ms-excel"
            << "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"
            << "application/vnd.openxmlformats-officedocument.spreadsheetml.template"
            << "application/vnd.ms-excel.sheet.macroEnabled.12"
            << "application/vnd.ms-excel.template.macroEnabled.12"
            << "application/vnd.ms-excel.addin.macroEnabled.12"
            << "application/vnd.ms-excel.sheet.binary.macroEnabled.12"
            << "application/vnd.ms-powerpoint"
            << "application/vnd.openxmlformats-officedocument.presentationml.presentation"
            << "application/vnd.openxmlformats-officedocument.presentationml.template"
            << "application/vnd.openxmlformats-officedocument.presentationml.slideshow"
            << "application/vnd.ms-powerpoint.addin.macroEnabled.12"
            << "application/vnd.ms-powerpoint.presentation.macroEnabled.12"
            << "application/vnd.ms-powerpoint.template.macroEnabled.12"
            << "application/vnd.ms-powerpoint.slideshow.macroEnabled.12"
               ;

    if(type == MediaItem::DocumentItem)
    {
        for (QVector<QStringList>::iterator i = trackerreply.begin(); i != trackerreply.end(); i++)
        {
            /* if this item is already in our list, skip it */
            if(mediaItemsUrnHash.contains((*i)[IDX_URN]))
                continue;

            QString url = (QUrl::fromEncoded((*i)[IDX_DOC_URI].toAscii())).toLocalFile();
            QString mimetype = (*i)[IDX_DOC_MIME];

            /* only create documentItems for files that exist */
            /* and have document mimetypes */
#warning "check this mimetype match"
            if (QFile::exists(url)&& documentMimetypes.contains(mimetype))
            {
                MediaItem *item = new MediaItem(type, recenttime, *i);
                newItemsList << item;
                mediaItemsSidHash.insert(item->m_sid, item);
                mediaItemsUrnHash.insert(item->m_urn, item);
                mediaItemsIdHash.insert(item->m_id, item);
            }
        }
    }

    mediaItemsList += newItemsList;
    thumb.queueRequests(newItemsList);

    /* tell the world we have new data */
    emit itemsAdded(&newItemsList);

    /* if this data was specifically requested, send an alert */
    if(priority)
    {
        for(int i = 0; i < newItemsList.count(); i++)
            emit itemAvailable(newItemsList[i]->m_urn);
    }
}

void DocumentDatabase::trackerGetDocuments(const int offset, const int limit)
{
    //qDebug() << "MusicDatabase::trackerGetDocuments";
    QString SqlCmd;
    switch(targetitemtype) {
    case MediaItem::DocumentItem:
        SqlCmd = TRACKER_ALLDOCUMENTS;
        break;
    default:
        return;
    }

    QString sql = QString(SqlCmd).arg(QString().sprintf("%d", limit), QString().sprintf("%d", offset));
    //qDebug() << "Tracker Query: " << sql;

    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(sql);
    QDBusPendingCall async = trackerinterface->asyncCallWithArgumentList(QLatin1String("SparqlQuery"), argumentList);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(trackerGetDocumentsFinished(QDBusPendingCallWatcher*)));
}

void DocumentDatabase::trackerGetDocumentsFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVector<QStringList> > reply = *call;

    /* no documents, either by error or end-of-query, quit */
    if (reply.isError()||reply.value().isEmpty()) {
        if(reply.isError())
            qDebug() << "QDBus Error: " << reply.error().message();
        return;
    }

    QVector<QStringList> documents = reply.value();
    trackerAddItems(targetitemtype, documents);

    /* generate the thumbnails after the items have been sent out */
    thumb.startLoop();

    /* go get more from tracker */
    trackerindex += trackeritems;
    trackerGetDocuments(trackerindex, trackeritems);
}

void DocumentDatabase::requestItem(QString identifier)
{
    /* instacheck, is the item already in the database? */
    if(identifier.startsWith("urn:"))
    {
        if(mediaItemsUrnHash.contains(identifier))
        {
            emit itemAvailable(identifier);
            return;
        }
    }
    else
    {
        QString uri = (QUrl::fromEncoded(identifier.toAscii())).toString();
        for(int i = 0; i < mediaItemsList.count(); i++)
        {
            MediaItem *m = mediaItemsList[i];
            if(m->isDocument()&&(m->m_uri == uri))
            {
                emit itemAvailable(m->m_urn);
                return;
            }
        }
    }

    /* item's not in the database, request it from tracker */
    QString SqlCmd;
    if(identifier.startsWith("urn:"))
        SqlCmd = TRACKER_DOCUMENT;
    else
        SqlCmd = TRACKER_DOCUMENT_URL;
    QString sql = QString(SqlCmd).arg(sparqlEscape(identifier));
    QVector<QStringList> info;

    if(trackerCall(info, sql))
    {
        trackerAddItems(MediaItem::DocumentItem, info, true);
        /* generate the thumbnails after the items have been sent out */
        thumb.startLoop();
        return;
    }

    /* the file exists, add a media item anyway */
    if(!identifier.startsWith("urn:")&&fileExists(identifier))
    {
        QList<MediaItem *> newItemsList;
        MediaItem *item = new MediaItem(MediaItem::DocumentItem, identifier);
        newItemsList << item;
        mediaItemsIdHash.insert(item->m_id, item);
        mediaItemsList += newItemsList;
        emit itemsAdded(&newItemsList);
        emit itemAvailable(item->m_id);
    }
}

void DocumentDatabase::trackerDocumentAdded(int sid)
{
    qDebug() << "trackerDocumentAdded " << sid;
    QString SqlCmd = TRACKER_DOCUMENT_SID;
    QString sql = QString(SqlCmd).arg(sid);

    QVector<QStringList> info;
    if(trackerCall(info, sql))
        trackerAddItems(MediaItem::DocumentItem, info);
}

/* high priority request from the view itself */
void DocumentDatabase::requestThumbnail(MediaItem *item)
{
    thumb.requestImmediate(item);
}

void DocumentDatabase::thumbReady(const MediaItem *item)
{
    QStringList temp;
    temp << item->m_id;
    emit itemsChanged(temp, DocumentDatabase::Thumbnail);
}
