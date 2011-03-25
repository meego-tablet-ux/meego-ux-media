/*
 * MeeGo Content Aggregation - Media Plugin
 * Copyright © 2010, Intel Corporation.
 */

#ifndef __mediaservicemodel_h
#define __mediaservicemodel_h

#include <QList>

#include <servicemodel.h>

struct MediaService {
    QString displayName;
    QString id;
    QString category;
};

class MediaServiceModel: public McaServiceModel
{
    Q_OBJECT

public:
    MediaServiceModel(QObject *parent = NULL);
    ~MediaServiceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QList<MediaService> m_services;
};

#endif  // __mediaservicemodel_h
