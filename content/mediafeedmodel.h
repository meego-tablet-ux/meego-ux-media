/*
 * MeeGo Content Aggregation - Media Plugin
 * Copyright © 2010, Intel Corporation.
 */

#ifndef __mediafeedmodel_h
#define __mediafeedmodel_h

#include <QObject>
#include <QList>
#include <QDateTime>

#include <feedmodel.h>
#include <actions.h>

class MediaListModel;

class MediaFeedModel: public McaFeedModel, public McaSearchableFeed
{
    Q_OBJECT

public:
    enum MediaType {
        MediaPhotos,
        MediaVideos,
        MediaMusic
    };

    MediaFeedModel(MediaType type, const QString& searchText, QObject *parent = 0);
    ~MediaFeedModel();

    void setSearchText(const QString &text);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

protected slots:
    void sourceRowsAboutToBeInserted(const QModelIndex& parent, int first, int last);
    void sourceRowsInserted(const QModelIndex& parent, int first, int last);
    void sourceRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
    void sourceRowsRemoved(const QModelIndex& parent, int first, int last);
    void sourceRowsAboutToBeMoved(const QModelIndex &source, int first, int last,
                                  const QModelIndex &dest, int destStart);
    void sourceRowsMoved(const QModelIndex &source, int first, int last,
                         const QModelIndex &dest, int destStart);
    void sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void sourceModelAboutToBeReset();
    void sourceModelReset();;
    void performAction(QString uniqueid, QString action);

private:
    MediaType m_type;
    MediaListModel *m_source;
    QString m_searchText;
    McaActions *m_actions;
};

#endif  // __mediafeedmodel_h
