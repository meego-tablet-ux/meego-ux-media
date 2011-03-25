/*
 * MeeGo Content Aggregation - Media Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include <QDebug>

#include <QDateTime>
#include <QStringList>
#include <QDesktopServices>

#include <actions.h>

#include "mediafeedmodel.h"
#include "medialistmodel.h"
#include "photolistmodel.h"
#include "videolistmodel.h"
#include "musiclistmodel.h"

MediaFeedModel::MediaFeedModel(MediaType type, const QString &searchText, QObject *parent):
        McaFeedModel(parent)
{
    m_type = type;

    if (type == MediaPhotos) {
        PhotoListModel *model = new PhotoListModel;
        model->setType(PhotoListModel::ListofPhotos);
        // TODO: add albums later
        m_source = model;
    }
    else if (type == MediaVideos) {
        VideoListModel *model = new VideoListModel;
        model->setType(VideoListModel::ListofAll);
        m_source = model;
    }
    else {
        MusicListModel *model = new MusicListModel;
        model->setType(MusicListModel::ListofSongs);
        m_source = model;
        // TODO: add albums later
        // TODO: add playlists later
        Q_ASSERT(type == MediaMusic);
    }

    m_actions = new McaActions;

    // TODO: media list model has code to move rows, but only when called from
    //   above it seems, so that shouldn't happen -- maybe we should watch for
    //   it here just in case and at least print a warning?
    connect(m_source, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeInserted(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
    connect(m_source, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(m_source, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(sourceRowsMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(m_source, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(sourceDataChanged(QModelIndex,QModelIndex)));
    connect(m_source, SIGNAL(modelAboutToBeReset()),
            this, SLOT(sourceModelAboutToBeReset()));
    connect(m_source, SIGNAL(modelReset()),
            this, SLOT(sourceModelReset()));

    connect(m_actions, SIGNAL(standardAction(QString,QString)),
            this, SLOT(performAction(QString,QString)));

    setSearchText(searchText);
}

MediaFeedModel::~MediaFeedModel()
{
    delete m_source;
}

//
// public member functions
//

void MediaFeedModel::setSearchText(const QString &text)
{
    if (m_searchText == text)
        return;

    if (text.isEmpty()) {
        int count = rowCount();
        if (count > 0) {
            beginRemoveRows(QModelIndex(), 0, count - 1);
            m_searchText = text;
            endRemoveRows();
        }
        return;
    }

    m_source->setSearch(text);

    if (m_searchText.isEmpty()) {
        int count = m_source->rowCount();
        if (count > 0) {
            beginInsertRows(QModelIndex(), 0, count - 1);
            m_searchText = text;
            endInsertRows();
        }
        return;
    }

    m_searchText = text;
}

int MediaFeedModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_searchText.isEmpty())
        return 0;
    return m_source->rowCount();
}

QVariant MediaFeedModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (!index.isValid() || row >= m_source->rowCount())
        return QVariant();

    QModelIndex sourceIndex = m_source->index(row);

    switch (role) {
    case RequiredTypeRole:
        return "content";

    case RequiredUniqueIdRole:
        return m_source->data(sourceIndex, MediaItem::URN).toString();

    case RequiredTimestampRole:
        {
            QString dt = m_source->data(sourceIndex, MediaItem::CreationTime).toString();
            if (dt.isEmpty())
                dt = m_source->data(sourceIndex, MediaItem::AddedTime).toString();
            return QDateTime::fromString(dt, Qt::ISODate);
        }

    case GenericTitleRole:
        return m_source->data(sourceIndex, MediaItem::Title);

    case GenericContentRole:
        return m_source->data(sourceIndex, MediaItem::Artist);

    case GenericAvatarUrlRole:
        {
            QString uri = m_source->data(sourceIndex, MediaItem::ThumbURI).toString();
            if (uri.isEmpty() && m_type != MediaVideos)
                uri = m_source->data(sourceIndex, MediaItem::AlbumThumbURI).toString();
            if (uri.isEmpty() && m_type == MediaMusic)
                uri = m_source->data(sourceIndex, MediaItem::ArtistThumbURI).toString();
            return uri;
        }

    case CommonActionsRole:
        return QVariant::fromValue<McaActions*>(m_actions);

    default:
        return QVariant();
    }

    return QVariant();
}

//
// protected slots
//

void MediaFeedModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)

    if (m_searchText.isEmpty())
        return;
    beginInsertRows(QModelIndex(), start, end);
}

void MediaFeedModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    if (m_searchText.isEmpty())
        return;
    endInsertRows();
}

void MediaFeedModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)

    if (m_searchText.isEmpty())
        return;
    beginRemoveRows(QModelIndex(), start, end);
}

void MediaFeedModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    if (m_searchText.isEmpty())
        return;
    endRemoveRows();
}

void MediaFeedModel::sourceRowsAboutToBeMoved(const QModelIndex &source, int start, int end,
                                              const QModelIndex &dest, int destStart)
{
    Q_UNUSED(source)
    Q_UNUSED(dest)

    if (m_searchText.isEmpty())
        return;
    beginMoveRows(QModelIndex(), start, end, QModelIndex(), destStart);
}

void MediaFeedModel::sourceRowsMoved(const QModelIndex &source, int start, int end,
                                     const QModelIndex &dest, int destStart)
{
    Q_UNUSED(source)
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(dest)
    Q_UNUSED(destStart)

    if (m_searchText.isEmpty())
        return;
    endMoveRows();
}

void MediaFeedModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (m_searchText.isEmpty())
        return;
    QModelIndex top = index(topLeft.row(), 0);
    QModelIndex bottom = index(bottomRight.row(), 0);
    emit dataChanged(top, bottom);
}

void MediaFeedModel::sourceModelAboutToBeReset()
{
    if (m_searchText.isEmpty())
        return;
    beginResetModel();
}

void MediaFeedModel::sourceModelReset()
{
    if (m_searchText.isEmpty())
        return;
    endResetModel();
}

void MediaFeedModel::performAction(QString action, QString uniqueid)
{
    qDebug() << "Action" << action << "called for media item" << uniqueid;

    if (action == "default") {
        QString app, cmd;
        if (m_type == MediaPhotos) {
            app = "meego-app-photos";
            cmd = "showPhoto";
        }
        else if (m_type == MediaVideos) {
            app = "meego-app-video";
            cmd = "video";
        }
        else {
            app = "meego-app-music";
            cmd = "playSong";
        }

        QString executable("meego-qml-launcher");
        QStringList parameters;
        parameters << "--opengl" << "--fullscreen";
        parameters << "--app" << app;
        parameters << "--cmd" << cmd;
        parameters << "--cdata" << uniqueid;
        QProcess::startDetached(executable, parameters);
    }
}
