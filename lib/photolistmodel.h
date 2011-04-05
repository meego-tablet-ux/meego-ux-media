/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef PHOTOLISTMODEL_H
#define PHOTOLISTMODEL_H

#include <QAbstractListModel>
#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "mediaitem.h"
#include "medialistmodel.h"

class PhotoListModel: public MediaListModel
{
    Q_OBJECT
    Q_ENUMS(ModelType)
    Q_PROPERTY(QString album READ getAlbum WRITE setAlbum NOTIFY albumChanged);
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY typeChanged);
    Q_PROPERTY(QString coveruri READ getCoverArt WRITE setCoverArt NOTIFY coverArtChanged);

public:
    PhotoListModel(QObject *parent = 0);
    ~PhotoListModel();

    enum ModelType {  ListofPhotos = 0,
                      ListofAlbums = 1,
                      ListofRecentlyViewed = 2,
                      PhotoAlbum = 3,
                      Editor = 4,
                      ListofUserAlbums = 5,
                      ListofUserPhotos = 6
                   };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QString getAlbum() const
        { return m_album; }
    void setAlbum(const QString &album);
    int getType() const
        { return m_type; }
    virtual void setType(const int type);
    virtual void setCoverArt(const QString &thumburi);
    QString getCoverArt() const
        { return m_coveruri; }

public slots:
    void setFavorite(const QStringList &ids, bool val);
    void setViewed(const QStringList &ids);
    void destroyItem(const int index);
    void destroyItemByID(const QString &id);
    void destroyItemsByID(const QStringList &ids);
    void requestItem(int type, QString urn);
    void changeTitle(QString uri, QString title);
    void changeTitleByURN(QString urn, QString title);
    QStringList getAllURIs();

    /* view calls for editable models */
    void addItems(const QStringList &ids);
    void removeItems(const QStringList &ids);
    void removeIndex(const int index);
    void saveAlbum();
    void clear();

signals:
    void albumChanged(const QString &album);
    void typeChanged(const int type);
    void coverArtChanged(const QString &coveruri);

protected slots:
    /* from MusicDatabase signals */
    void itemsAdded(const QList<MediaItem *> *list);
    void itemsChanged(const QStringList &ids, int reason);

protected:
    /* private filter functions which respond to setting the filter property */
    QString m_album;
    int m_type;
    QString m_coveruri;
    bool isVirtualAlbum;

    void saveAlbum(const QString &title);
    void clearData();
    void connectSignals(bool added, bool changed, bool removed);
};

#endif // PHOTOLISTMODEL_H
