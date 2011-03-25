/*
 * MeeGo Content Aggregation - Media Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include <QDebug>

#include <QtPlugin>

#include "mediaplugin.h"
#include "mediaservicemodel.h"
#include "mediafeedmodel.h"

MediaPlugin::MediaPlugin(QObject *parent): QObject(parent), McaFeedPlugin()
{
    m_serviceModel = new MediaServiceModel(this);
}

MediaPlugin::~MediaPlugin()
{
}

QAbstractItemModel *MediaPlugin::serviceModel()
{
    return m_serviceModel;
}

QAbstractItemModel *MediaPlugin::createFeedModel(const QString& service)
{
    qDebug() << "MediaPlugin::createFeedModel: " << service;

    // TODO: when Carl adds account setting need to specify which one we want
    return NULL;
}

McaSearchableFeed *MediaPlugin::createSearchModel(const QString& service,
                                                  const QString& searchText)
{
    qDebug() << "MediaPlugin::createSearchModel: " << service << searchText;

    MediaFeedModel *model = NULL;
    if (service == "photos")
        model = new MediaFeedModel(MediaFeedModel::MediaPhotos, searchText, this);
    else if (service == "videos")
        model = new MediaFeedModel(MediaFeedModel::MediaVideos, searchText, this);
    else if (service == "music")
        model = new MediaFeedModel(MediaFeedModel::MediaMusic, searchText, this);
    return model;
}

Q_EXPORT_PLUGIN2(mediaplugin, MediaPlugin)
