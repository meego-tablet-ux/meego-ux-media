/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MEDIADATABASE_H
#define MEDIADATABASE_H

#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <QDBusPendingCallWatcher>
#include "mediaitem.h"

struct Quad
{
    int graph;
    int subject;
    int predicate;
    int object;
};

struct TrackerIDs
{
    bool valid;
    int rdf_type;
    int nie_title;
    int nmm_Photo;
    int nmm_ImageList;
    int nmm_MusicPiece;
    int nmm_MusicAlbum;
    int nmm_Artist;
    int nmm_Playlist;
    int nmm_Video;
    int nao_Tag;
};

QDebug operator<<(QDebug d, Quad const &t);
QDBusArgument& operator<<(QDBusArgument& argument, const Quad &t);
const QDBusArgument& operator>>(const QDBusArgument& argument, Quad &t);

Q_DECLARE_METATYPE(QVector<QStringList>)
Q_DECLARE_METATYPE(Quad)
Q_DECLARE_METATYPE(QVector<Quad>)

class MGConfItem;

/* base class for music, photo, video databases. defines */
/* common functionality and shouldn't be instantiated directly */
class MediaDatabase : public QObject {
    Q_OBJECT

public:
    MediaDatabase(QString recentRangeKey, QObject *parent = 0);
    ~MediaDatabase();

    enum ChangeReason { Viewed = 0,
                        Unviewed = 1,
                        Favorited = 2,
                        Thumbnail = 3,
                        PlayStatus = 4,
                        Contents = 5,
                        Title = 6,
                        Other = 7
                      };

    enum PrioritySignal { None = 0,
                          MainItem = 1,
                          AssociatedSongs = 2,
                        };

    QList<MediaItem *> getSnapshot();
    QList<MediaItem *> getItemsByID(const QStringList &ids);
    QList<MediaItem *> getItemsByURN(const QStringList &urns);
    virtual void destroyItem(MediaItem *item);
    void broadcastMyChanges(const QStringList &ids, int reason);
    void broadcastMyChanges(const QString &id, int reason);
    void setRecentlyViewed(const QString &urn, const QString &timestamp);
    void setFavorite(const QString &urn, const bool &favorite);
    void setFavoriteMulti(const QStringList &ids, bool val);
    void setViewedMulti(const QStringList &ids);
    void setPlayStatus(const QString &id, int playstatus);
    bool trackerCall(QVector<QStringList> &out, const QString &cmd);
    void trackerCall(const QString &cmd);
    void trackerCallAsync(const QString &cmd);
    void changeTitle(QString uri, QString title);
    void changeTitleByURN(QString urn, QString title);
    void clearHistory();

public slots:
    void trackerUpdates(QString classname, QVector<Quad> deletes, QVector<Quad> inserts);
    void recentkeyChanged();

signals:
    void itemsChanged(const QStringList &ids, int reason);
    void itemsAdded(const QList<MediaItem *> *list);
    void itemsRemoved(const QStringList &ids);
    void photoItemAdded(int subjectid);
    void photoAlbumItemAdded(int subjectid);
    void songItemAdded(int subjectid);
    void musicAlbumItemAdded(int subjectid);
    void musicArtistItemAdded(int subjectid);
    void playlistItemAdded(int subjectid);
    void videoItemAdded(int subjectid);
    void itemAvailable(const QString identifier);
    void songItemAvailable(const QString identifier);

protected:
    /* the master list contains all music items found through tracker */
    QList<MediaItem *> mediaItemsList;
    QHash<QString, MediaItem *> mediaItemsUrnHash;
    QHash<QString, MediaItem *> mediaItemsIdHash;
    QHash<int, MediaItem *> mediaItemsSidHash;
    QList<MediaItem *> needThumbList;
    void itemAdded(MediaItem *item);
    void itemChanged(MediaItem *item, int reason);

    /* tracker calls */
    QDBusInterface *trackerinterface;
    bool fileExists(const QString &name);

    /* recently viewed functionality */
    void clearRecentlyViewedTags();
    void getRecentKeyVal();
    MGConfItem *recentkey;
    QDateTime recenttime;

    /* variables used in populating the list */
    int trackeritems;
    int trackerindex;
    int targetitemtype;

    /* tracker helper functions */
    bool classNameIsMediaItem(QString);
    bool getTrackerIDs();
    TrackerIDs tid;

    /* variables used in thumbnail loops */
    QTimer thumbnailerTimer;
};

#endif // MEDIADATABASE_H
