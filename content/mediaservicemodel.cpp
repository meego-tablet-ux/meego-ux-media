/*
 * MeeGo Content Aggregation - Media Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include <QDebug>
#include <QString>

#include <actions.h>

#include "mediaservicemodel.h"

MediaServiceModel::MediaServiceModel(QObject *parent):
        McaServiceModel(parent)
{
    MediaService service;
    service.displayName = tr("Photos");
    service.id = "photos";
    service.category = "photo";
    m_services.append(service);

    service.displayName = tr("Videos");
    service.id = "videos";
    service.category = "video";
    m_services.append(service);

    service.displayName = tr("Music");
    service.id = "music";
    service.category = "audio";
    m_services.append(service);
}

MediaServiceModel::~MediaServiceModel()
{
}

//
// public member functions
//

int MediaServiceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_services.count();
}

QVariant MediaServiceModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (!index.isValid() || row >= m_services.count())
        return QVariant();

    switch (role) {
    case CommonDisplayNameRole:
        return m_services.at(row).displayName;

    case RequiredCategoryRole:
        return m_services.at(row).category;

    case RequiredNameRole:
        return m_services.at(row).id;

    case CommonConfigErrorRole:
        return false;

    case CommonActionsRole:
        // unless we start sending "true" for CommonConfigErrorRole, not needed
    default:
        qWarning() << "Unhandled data role requested!";
    case CommonIconUrlRole:
        // expect no icon header for media
        return QVariant();
    }
}
