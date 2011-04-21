/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "mediaqml.h"
#include "musiclistmodel.h"
#include "photolistmodel.h"
#include "videolistmodel.h"
#include "playlistimageprovider.h"

void MediaQML::registerTypes(const char *uri)
{
    qmlRegisterType<MediaItem>(uri, 0, 0, "MediaItem");
    qmlRegisterType<MusicListModel>(uri, 0, 0, "MusicListModel");
    qmlRegisterType<PhotoListModel>(uri, 0, 0, "PhotoListModel");
    qmlRegisterType<VideoListModel>(uri, 0, 0, "VideoListModel");
}

void MediaQML::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
    engine->addImageProvider("playlist", new PlaylistImageProvider);
}

Q_EXPORT_PLUGIN(MediaQML);
