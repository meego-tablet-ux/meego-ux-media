/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDir>
#include <QDebug>
#include "thumbnailer.h"

#define TUMBLER_SERVICE       "org.freedesktop.thumbnails.Thumbnailer1"
#define TUMBLER_PATH          "/org/freedesktop/thumbnails/Thumbnailer1"
#define TUMBLER_INTERFACE     "org.freedesktop.thumbnails.Thumbnailer1"
#define DEFAULT_SCHEDULER     "foreground"
#define THUMBNAILPAUSE        5000

Thumbnailer::Thumbnailer(QObject *parent)
    : QObject(parent)
{
    thumbnailerlooppause = false;
    thumbnailerinuse = false;
    connect(&thumbnailerTimer, SIGNAL(timeout()), this, SLOT(thumbnailerResumeLoop()));

    tumblerinterface = new QDBusInterface(TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE);

    QDBusConnection::sessionBus().connect(TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE,
        "Ready", this, SLOT(tumblerReady(const unsigned int, const QStringList)));
    QDBusConnection::sessionBus().connect(TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE,
        "Error", this, SLOT(tumblerError(const unsigned int, const QStringList, \
                                                const int, const QString )));
    QDBusConnection::sessionBus().connect(TUMBLER_SERVICE, TUMBLER_PATH, TUMBLER_INTERFACE,
        "Finished", this, SLOT(tumblerFinished(const unsigned int)));

    if(!mediaart.supported("dvdimage"))
    {
        disable_mediaart = true;
    }
    else
    {
        disable_mediaart = false;
        connect(&mediaart,SIGNAL(error(QString, QString, QString, QString)),
                this,SLOT(downloadError(QString, QString, QString, QString)));
        connect(&mediaart,SIGNAL(ready(QString, QString, QString, QString)),
                this,SLOT(downloadReady(QString, QString, QString, QString)));
    }
}

Thumbnailer::~Thumbnailer()
{
}

bool Thumbnailer::isValid(MediaItem *item)
{
    /* it has no thumbnail already, and its */
    /* thumbnail hasn't already tried and failed, */
    /* and it's a photo or video file */
    if(!item->m_thumburi_exists&&
       !item->m_thumburi_ignore&&
       (item->isPhoto()||item->isAnyVideoType()))
    {
        return true;
    }
    return false;
}

void Thumbnailer::queueRequest(MediaItem *item)
{
    if(isValid(item)&&!queue.contains(item))
    {
        queue << item;
        //qDebug() << "Tumbler Queueing: " <<  item->m_uri;
    }
}

void Thumbnailer::queueRequests(QList<MediaItem *> &items)
{
    for(int i = 0; i < items.count(); i++)
        queueRequest(items[i]);
}

void Thumbnailer::sendDownloadRequest(MediaItem *item)
{
    QString thumburi = item->m_thumburi;
    thumburi.replace("file://", "");
    mediaart.request(item->m_id, "dvdimage",
        item->m_title + "|" + thumburi);
}

void Thumbnailer::sendTumblerRequest(MediaItem *item)
{
    QStringList uris, mimetypes;
    uris << item->m_uri;
    mimetypes << item->m_mimetype;
    quint32 handle = 0;
    thumbnailerinuse = true;
    tumblerinterface->asyncCall(QLatin1String("Queue"), uris, mimetypes,
        DEFAULT_FLAVOR, DEFAULT_SCHEDULER, handle);
}

void Thumbnailer::startLoop()
{
    /* if it's already running or paused, leave */
    if(thumbnailerinuse||thumbnailerlooppause)
        return;

    /* create a list of uris/mimetypes for the tumbler call */
    if(!queue.isEmpty())
    {
        if((queue[0]->isAnyVideoType())&&(!disable_mediaart))
            sendDownloadRequest(queue[0]);
        else
            sendTumblerRequest(queue[0]);
    }
}

/* high priority request from the view itself */
void Thumbnailer::requestImmediate(MediaItem *item)
{
    if(isValid(item))
    {
        if(!queue.contains(item))
            queue << item;
        thumbnailerlooppause = true;
        thumbnailerTimer.stop();
        thumbnailerTimer.start(THUMBNAILPAUSE);
        if((item->isAnyVideoType())&&(!disable_mediaart))
            sendDownloadRequest(item);
        else
            sendTumblerRequest(item);
    }
}

void Thumbnailer::thumbnailerResumeLoop()
{
    thumbnailerTimer.stop();
    thumbnailerlooppause = false;
    thumbnailerinuse = false;
    startLoop();
}


void Thumbnailer::tumblerFinished(const unsigned int &handle)
{
    thumbnailerinuse = false;
    if(!thumbnailerlooppause)
        startLoop();
}

void Thumbnailer::tumblerReady(const unsigned int &handle, const QStringList &urls)
{
    QList<MediaItem *> removeList;

    for(int i = 0; i < queue.count(); i++)
    {
        if(urls.contains(queue[i]->m_uri))
        {
            queue[i]->m_thumburi_exists = true;
            queue[i]->m_thumburi_ignore = false;
            removeList << queue[i];
            emit success(queue[i]);
        }
    }

    for(int i = 0; i < removeList.count(); i++)
        queue.removeAll(removeList[i]);
}

void Thumbnailer::downloadReady(QString reqid, QString type, QString info, QString data)
{
    qDebug() << "Media Info Ready: " << reqid << type << info << data;
    QList<MediaItem *> removeList;

    for(int i = 0; i < queue.count(); i++)
    {
        if(queue[i]->m_id == reqid)
        {
            queue[i]->m_thumburi_exists = true;
            queue[i]->m_thumburi_ignore = false;
            removeList << queue[i];
            emit success(queue[i]);
        }
    }

    for(int i = 0; i < removeList.count(); i++)
        queue.removeAll(removeList[i]);
}

void Thumbnailer::tumblerError(const unsigned int &handle, const QStringList &urls, const int &errorCode, const QString &message)
{
    QList<MediaItem *> removeList;

    for(int i = 0; i < queue.count(); i++)
    {
        if(urls.contains(queue[i]->m_uri))
        {
            queue[i]->m_thumburi_exists = false;
            queue[i]->m_thumburi_ignore = true;
            removeList << queue[i];
            emit failure(queue[i]);
        }
    }

    for(int i = 0; i < removeList.count(); i++)
        queue.removeAll(removeList[i]);
}

void Thumbnailer::downloadError(QString reqid, QString type, QString info, QString errorString)
{
    qDebug() << "Media Downloader Error: " << reqid << type << info << errorString;
    for(int i = 0; i < queue.count(); i++)
        if(queue[i]->m_id == reqid)
        {
            sendTumblerRequest(queue[i]);
            return;
        }
}
