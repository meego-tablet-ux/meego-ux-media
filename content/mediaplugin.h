/*
 * MeeGo Content Aggregation - Media Plugin
 * Copyright © 2010, Intel Corporation.
 */

#ifndef __mediaplugin_h
#define __mediaplugin_h

#include <QObject>

#include <feedplugin.h>

class McaServiceModel;
class McaFeedModel;
class MediaServiceModel;

class MediaPlugin: public QObject, public McaFeedPlugin
{
    Q_OBJECT
    Q_INTERFACES(McaFeedPlugin)

public:
    explicit MediaPlugin(QObject *parent = NULL);
    ~MediaPlugin();

    QAbstractItemModel *serviceModel();
    QAbstractItemModel *createFeedModel(const QString& service);
    McaSearchableFeed *createSearchModel(const QString& service,
                                         const QString& searchText);

private:
    MediaServiceModel *m_serviceModel;
};

#endif  // __mediaplugin_h
