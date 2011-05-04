/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MEDIALISTMODEL_H
#define MEDIALISTMODEL_H

#include <QAbstractListModel>
#include "mediaitem.h"

class MediaDatabase;

class MediaListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(FilterType)
    Q_ENUMS(SortType)
    Q_ENUMS(PlayType)
    Q_PROPERTY(int filter READ getFilter WRITE setFilter NOTIFY filterChanged);
    Q_PROPERTY(int limit READ getLimit WRITE setLimit NOTIFY limitChanged);
    Q_PROPERTY(int sort READ getSort WRITE setSort NOTIFY sortChanged);
    /* count is the number of items currently displayed */
    Q_PROPERTY(int count READ getCount NOTIFY countChanged);
    /* total is the total number of items displayed or not */
    Q_PROPERTY(int total READ getTotal NOTIFY totalChanged);
    Q_PROPERTY(QString search READ getSearch WRITE setSearch NOTIFY searchChanged);

public:
    MediaListModel(QObject *parent = 0);
    ~MediaListModel();

    enum FilterType { FilterAll = 0,
                      FilterFavorite = 1,
                      FilterViewed = 2,
                      FilterAdded = 3,
                      FilterUnwatched = 4,
                      FilterSearch = 5 };

    enum SortType { SortByDefault = 0,
                    SortByTitle = 1,
                    SortByAddedTime = 2,
                    SortByCreationTime = 3,
                    SortByAccessTime = 4,
                    SortByUnwatched = 5,
                    SortByFavorite = 6,
                    SortByTrackNum = 7,
                    SortByURNList = 8,
                    SortAsIs = 9 };

    enum PlayType  {  Stopped = 0,
                      Paused = 1,
                      Playing = 2
                   };

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QVariant data(int index) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void insertRow(int row, MediaItem *item);
    void moveRow(int rowsrc, int rowdst);

    int getFilter() const
        { return m_filter; }
    virtual void setFilter(const int filter);
    QString getSearch() const
        { return m_search; }
    virtual void setSearch(const QString search);
    int getLimit() const
        { return m_limit; }
    virtual void setLimit(const int limit);
    int getSort() const
        { return m_sort; }
    virtual void setSort(const int sort);
    int getCount() const
        { return mediaItemsDisplay.count(); }
    int getTotal() const
        { return mediaItemsList.count(); }

public slots:
    int itemIndex(const QString &id);
    QVariant datafromURN(const QString &urn, int role);
    QVariant datafromID(const QString &id, int role);
    QVariant datafromIndex(const int index, int role);
    QString getIDfromURN(const QString &urn);
    QString getTitlefromURN(const QString &urn);
    QString getURIfromURN(const QString &urn);
    QString getURIfromIndex(const int index);
    QString getTitlefromID(const QString &id);
    QString getURIfromID(const QString &id);
    int getIndexfromURN(const QString &urn);
    int getTypefromURN(const QString &urn);
    int getTypefromID(const QString &id);
    bool isFavorite(const QString &id);
    void setSelected(const QString &id, bool selected);
    bool isSelected(const QString &id);
    int selectionCount();
    void clearSelected();
    QStringList getSelectedURIs();
    QStringList getSelectedIDs();
    QStringList getAllIDs();
    void hideItemsByURN(const QStringList &urns);
    void hideItemByURN(const QString &urn);
    bool isURN(const QString &val);

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void filterChanged(const int filter);
    void limitChanged(const int limit);
    void sortChanged(const int sort);
    void searchChanged(const QString search);
    void totalChanged(const int total);
    void countChanged(const int count);
    void itemAvailable(const QString identifier);
    void songItemAvailable(const QString identifier);

protected slots:
    void itemsRemoved(const QStringList &ids);

protected:
    /* private filter functions which respond to setting the filter property */
    int m_filter;
    QString m_search;
    bool datesearch;
    QDateTime rangeStart;
    QDateTime rangeEnd;
    int m_sort;
    int m_limit;
    int m_default_sort;
    bool firstsort;
    MediaDatabase *database;
    QList<MediaItem *> selectedItemsList;
    QStringList hiddenURNsList;
    QStringList urnSortList;
    bool disable_filter;

    void notifyChanged(const QStringList &ids);
    void redisplay(const QModelIndex &parent = QModelIndex());
    void displayNewItems(QList<MediaItem *> &list, const QModelIndex &parent = QModelIndex());
    void resortItems(QList<MediaItem *> &list, const QModelIndex &parent = QModelIndex());
    bool isItemDisplayed(MediaItem *item);
    void sortItems(QList<MediaItem *> &list, int sort);
    bool isYbeforeX(MediaItem *x, MediaItem *y);
    QList<MediaItem *> filterItems(const QList<MediaItem *> list);
    void hideItems(QList<MediaItem *> &list);
    void filterDuplicates(QList<MediaItem *> &currentList, QList<MediaItem *> &additionList);
    QVariant datafromItem(MediaItem *item, int role);

    /* the master list contains all the photos found through tracker */
    QList<MediaItem *> mediaItemsList;

    /* the display list contains just the items that are to be displayed */
    QList<MediaItem *> mediaItemsDisplay;

private:
    void displayItems(const QList<MediaItem *> &list);
    void checkForTimeString(const QString search);
};

#endif // MEDIALISTMODEL_H
