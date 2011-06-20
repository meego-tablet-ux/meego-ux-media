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

#define THUMBNAILPAUSE        5000

Thumbnailer::Thumbnailer(QObject *parent)
    : QObject(parent)
{
    thumbnailerlooppause = false;
    thumbnailerinuse = false;
    connect(&thumbnailerTimer, SIGNAL(timeout()), this, SLOT(thumbnailerResumeLoop()));
    connect(&mediaart,SIGNAL(error(QString, QString, QString, QString)),
            this,SLOT(downloadError(QString, QString, QString, QString)));
    connect(&mediaart,SIGNAL(ready(QString, QString, QString, QString)),
            this,SLOT(downloadReady(QString, QString, QString, QString)));

    videosupport = mediaart.supported("dvdimage");
    artistsupport = mediaart.supported("artistimage");
    albumsupport = mediaart.supported("albumimage");
}

Thumbnailer::~Thumbnailer()
{
}

bool Thumbnailer::isValid(MediaItem *item)
{
    /* if the thumbnail hasn't already tried and failed, */
    /* and it's a video, music album, or music artist */
    /* and we found support for video, album, or artist */
    if(!item->m_thumburi_ignore&&
       ((item->isAnyVideoType()&&videosupport)||
        (item->isMusicArtist()&&artistsupport)||
        (item->isMusicAlbum()&&!item->m_artist.isEmpty()&&albumsupport)))
    {
        return true;
    }
    return false;
}

void Thumbnailer::queueRequest(MediaItem *item)
{
    /* if we can process the item, and it's not already being processed */
    if(isValid(item)&&!queue.contains(item))
    {
        queue << item;
    }
}

void Thumbnailer::queueRequests(QList<MediaItem *> &items)
{
    for(int i = 0; i < items.count(); i++)
        queueRequest(items[i]);
}

void Thumbnailer::sendDownloadRequest(MediaItem *item)
{
    if(item->isAnyVideoType())
    {
        QString thumburi = MediaItem::thumbVideo(item->m_uri);
        mediaart.request(item->m_id, "dvdimage",
            item->m_title + "|" + thumburi);
    }
    else if(item->isMusicArtist())
    {
        QString thumburi = MediaItem::thumbMusicArtist(item->m_title);
        mediaart.request(item->m_id, "artistimage",
            item->m_title + "|" + thumburi);
    }
    else if(item->isMusicAlbum())
    {
        QString thumburi = MediaItem::thumbMusicAlbum(item->m_artist[0], item->m_title);
        mediaart.request(item->m_id, "albumimage",
            item->m_artist[0] + "|" + item->m_title + "|" + thumburi);
    }
}

void Thumbnailer::startLoop()
{
    /* if it's already running or paused, leave */
    if(thumbnailerinuse||thumbnailerlooppause)
        return;

    /* create a list of uris/mimetypes for the tumbler call */
    if(!queue.isEmpty())
    {
        sendDownloadRequest(queue[0]);
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
        sendDownloadRequest(item);
    }
}

void Thumbnailer::thumbnailerResumeLoop()
{
    thumbnailerTimer.stop();
    thumbnailerlooppause = false;
    thumbnailerinuse = false;
    startLoop();
}

void Thumbnailer::downloadReady(QString reqid, QString type, QString info, QString data)
{
    qDebug() << "Media Info Ready: " << reqid << type << info << data;
    QList<MediaItem *> removeList;
    QStringList args = info.split("|", QString::KeepEmptyParts);
    QString thumburi = QString("file://") + args.last();

    for(int i = 0; i < queue.count(); i++)
    {
        if((queue[i]->m_id == reqid)&&(MediaItem::fileExists(thumburi)))
        {
            queue[i]->m_thumburi = thumburi;
            queue[i]->m_thumburi_exists = true;
            queue[i]->m_thumburi_ignore = false;
            removeList << queue[i];
            emit success(queue[i]);
        }
    }

    for(int i = 0; i < removeList.count(); i++)
        queue.removeAll(removeList[i]);

    if(!thumbnailerlooppause)
        startLoop();
}

void Thumbnailer::downloadError(QString reqid, QString type, QString info, QString errorString)
{
    qDebug() << "Media Downloader Error: " << reqid << type << info << errorString;
    QList<MediaItem *> removeList;

    for(int i = 0; i < queue.count(); i++)
    {
        if(queue[i]->m_id == reqid)
        {
            removeList << queue[i];
            emit failure(queue[i]);
        }
    }

    for(int i = 0; i < removeList.count(); i++)
        queue.removeAll(removeList[i]);

    if(!thumbnailerlooppause)
        startLoop();
}
