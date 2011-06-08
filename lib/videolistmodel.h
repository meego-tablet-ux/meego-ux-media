/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef VIDEOLISTMODEL_H
#define VIDEOLISTMODEL_H

#include <QAbstractListModel>
#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "mediaitem.h"
#include "medialistmodel.h"

class VideoListModel: public MediaListModel
{
    Q_OBJECT
    Q_ENUMS(ModelType)
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY typeChanged);

public:
    VideoListModel(QObject *parent = 0);
    ~VideoListModel();

    enum ModelType {  ListofVideos = 0,
                      ListofTVShows = 1,
                      ListofMusicVideos = 2,
                      ListofAll = 3,
                      ListofUnwatched = 4,
                      ListofRecentlyViewed = 5,
                      Editor = 6,
                      ListofUserVideos = 7
                   };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int getType() const
        { return m_type; }
    virtual void setType(const int type);

public slots:
    void setFavorite(const QStringList &ids, bool val);
    void setViewed(const QStringList &ids);
    void setPlayStatus(const QString &id, int playstatus);
    void destroyItem(const int index);
    void destroyItemByID(const QString &id);
    void destroyItemsByID(const QStringList &ids);
    void requestItem(QString urn);
    void clear();
    void changeTitle(QString uri, QString title);
    void changeTitleByURN(QString urn, QString title);

signals:
    void typeChanged(const int type);

protected slots:
    /* from MusicDatabase signals */
    void itemsAdded(const QList<MediaItem *> *list);
    void itemsChanged(const QStringList &ids, int reason);

protected:
    /* private filter functions which respond to setting the filter property */
    int m_type;

    void clearData();
    void connectSignals(bool added, bool changed, bool removed);
};

#endif // VIDEOLISTMODEL_H
