/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QtCore/QtCore>
#include <QtCore/QObject>
#include "mediaitem.h"
#include "medialistmodel.h"

class DocumentListModel: public MediaListModel
{
    Q_OBJECT
    Q_ENUMS(ModelType)
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY typeChanged)

public:
    DocumentListModel(QObject *parent = 0);
    ~DocumentListModel();

    enum ModelType {  ListofDocuments = 0,
                      ListofRecentlyViewed = 1,
                      Editor = 2
                   };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int getType() const
    {
        return m_type;
    }
    virtual void setType(const int type);

public slots:
    void setFavorite(const QStringList &ids, bool val);
    void setViewed(const QStringList &ids);
    //void setPlayStatus(const QString &id, int playstatus);
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
    /* from DocumentDatabase signals */
    void itemsAdded(const QList<MediaItem *> *list);
    void itemsChanged(const QStringList &ids, int reason);

protected:
    /* properties for this model */
    int m_type;

    void clearData();
    void connectSignals(bool added, bool changed, bool removed);
    QList<MediaItem *> m_data;
};

#endif // DOCUMENTLISTMODEL_H
