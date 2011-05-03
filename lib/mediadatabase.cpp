/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QtDBus/QtDBus>
#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include "mediadatabase.h"
#include <MGConfItem>

// D-Bus marshalling
QDBusArgument &operator<<(QDBusArgument &argument, const Quad &t)
{
    argument.beginStructure();
    argument << t.graph << t.subject << t.predicate << t.object;
    argument.endStructure();
    return argument;
}

// D-Bus demarshalling
const QDBusArgument &operator>>(const QDBusArgument &argument, Quad &t)
{
    argument.beginStructure();
    argument >> t.graph >> t.subject >> t.predicate >> t.object;
    argument.endStructure();
    return argument;
}

MediaDatabase::MediaDatabase(QString recentRangeKey, QObject *parent)
    : QObject(parent)
{
    tid.valid = false;
    tid.rdf_type = 11;
    tid.nie_title = 51;
    tid.nmm_Photo = 711;
    tid.nmm_ImageList = 710;
    tid.nmm_MusicPiece = 666;
    tid.nmm_MusicAlbum = 667;
    tid.nmm_Artist = 672;
    tid.nmm_Playlist = 673;
    tid.nmm_Video = 671;
    tid.nao_Tag = 89;

    qRegisterMetaType<QVector<QStringList> >();
    qDBusRegisterMetaType<QVector<QStringList> >();
    qDBusRegisterMetaType<Quad>();
    qDBusRegisterMetaType<QVector<Quad> >();

    recentkey = new MGConfItem(recentRangeKey);
    connect(recentkey,SIGNAL(valueChanged()),this,SLOT(recentkeyChanged()));
    getRecentKeyVal();

    trackerinterface = new QDBusInterface(TRACKER_SERVICE, TRACKER_PATH, TRACKER_INTERFACE);
    QDBusConnection::sessionBus().connect(TRACKER_SERVICE, TRACKER_PATH, TRACKER_INTERFACE,
        "GraphUpdated", this, SLOT(trackerUpdates(QString, QVector<Quad>, QVector<Quad>)));
}

MediaDatabase::~MediaDatabase()
{
    qDebug() << "~MediaDatabase";
//    for (int i = 0; i < mediaItemsList.count(); i++)
//    {
//        delete mediaItemsList.at(i);
//        mediaItemsList.removeAt(i);
//    }
    while (!mediaItemsList.isEmpty()) {
        delete mediaItemsList.takeFirst();
    }
    mediaItemsSidHash.clear();
    mediaItemsUrnHash.clear();
    mediaItemsIdHash.clear();
}

bool MediaDatabase::getTrackerIDs()
{
    if(tid.valid)
        return true;

    QString SqlCmd = "SELECT tracker:id(rdf:type) tracker:id(nie:title) " \
                     "tracker:id(nmm:Photo) tracker:id(nmm:ImageList) " \
                     "tracker:id(nmm:MusicPiece) tracker:id(nmm:MusicAlbum) " \
                     "tracker:id(nmm:Artist) tracker:id(nmm:Playlist) " \
                     "tracker:id(nmm:Video) tracker:id(nao:Tag) {}";

    QVector<QStringList> info;
    if(trackerCall(info, SqlCmd))
    {
        for (QVector<QStringList>::iterator i = info.begin(); i != info.end(); i++)
        {
            TrackerIDs temp;
            QStringList ids = (*i);
            if(ids.count() != 10)
                return false;

            temp.valid = true;
            for(int i = 0; i < 10; i++)
            {
                bool ok;
                int val = ids[i].toInt(&ok);
                if(!ok)
                    return false;

                switch(i) {
                case 0:
                    temp.rdf_type = val;
                    break;
                case 1:
                    temp.nie_title = val;
                    break;
                case 2:
                    temp.nmm_Photo = val;
                    break;
                case 3:
                    temp.nmm_ImageList = val;
                    break;
                case 4:
                    temp.nmm_MusicPiece = val;
                    break;
                case 5:
                    temp.nmm_MusicAlbum = val;
                    break;
                case 6:
                    temp.nmm_Artist = val;
                    break;
                case 7:
                    temp.nmm_Playlist = val;
                    break;
                case 8:
                    temp.nmm_Video = val;
                    break;
                case 9:
                    temp.nao_Tag = val;
                    break;
                }
            }
            tid = temp;
            return true;
        }
    }
    return false;
}

bool MediaDatabase::fileExists(const QString &name)
{
    QString temp = name;
    temp.replace("file://", "");
    return QFile::exists(temp);
}

void MediaDatabase::getRecentKeyVal()
{
    QDateTime rtime = QDateTime::currentDateTime().addDays(DAYSRECENT);
    QString temp = recentkey->value(rtime.toString()).toString();
    recenttime = QDateTime::fromString(temp);
    if(recenttime < rtime)
        recenttime = rtime;
}

void MediaDatabase::recentkeyChanged()
{
    getRecentKeyVal();
    clearRecentlyViewedTags();
}

void MediaDatabase::clearHistory()
{
    QDateTime ctime = QDateTime::currentDateTime();
    recentkey->set(ctime.toString());
    clearRecentlyViewedTags();
}

void MediaDatabase::clearRecentlyViewedTags()
{
    QStringList ids;
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        if(mediaItemsList[i]->m_recentlyviewed)
        {
            ids << mediaItemsList[i]->m_id;
            mediaItemsList[i]->m_recentlyviewed = false;
        }
    }
    if(!ids.isEmpty())
    {
        emit itemsChanged(ids, MediaDatabase::Unviewed);
    }
}

bool MediaDatabase::trackerCall(QVector<QStringList> &out, const QString &cmd)
{
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(cmd);
    QDBusReply<QVector<QStringList> > reply =
            trackerinterface->callWithArgumentList(QDBus::BlockWithGui,
            QLatin1String("SparqlQuery"), argumentList);

    if (!reply.isValid()) {
        qDebug() << "QDBus Error: " << reply.error().message();
        return false;
    }

    /* fill out this album's children list */
    out = reply.value();

    if(out.isEmpty())
        return false;
    return true;
}

void MediaDatabase::trackerCall(const QString &cmd)
{
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(cmd);
    if(cmd.contains("INSERT")||cmd.contains("DELETE"))
    {
        trackerinterface->callWithArgumentList(QDBus::BlockWithGui,
                 QLatin1String("SparqlUpdate"), argumentList);
    }
    else
    {
        trackerinterface->callWithArgumentList(QDBus::BlockWithGui,
                QLatin1String("SparqlQuery"), argumentList);
    }
}

void MediaDatabase::trackerCallAsync(const QString &cmd)
{
    //qDebug() << "Tracker Command: " << cmd;
    QList<QVariant> argumentList;
    argumentList << qVariantFromValue(cmd);
    trackerinterface->asyncCallWithArgumentList(QLatin1String("SparqlUpdate"), argumentList);
}

void MediaDatabase::broadcastMyChanges(const QStringList &ids, int reason)
{
    emit itemsChanged(ids, reason);
}

void MediaDatabase::broadcastMyChanges(const QString &id, int reason)
{
    QStringList ids;
    ids << id;
    emit itemsChanged(ids, reason);
}

QList<MediaItem *> MediaDatabase::getSnapshot()
{
    QList<MediaItem *> snapshot = mediaItemsList;

    return snapshot;
}

QList<MediaItem *> MediaDatabase::getItemsByID(const QStringList &ids)
{
    QHash<QString, MediaItem *> unsorted;
    QList<MediaItem *> sorted;

    for(int i = 0; i < mediaItemsList.count(); i++)
        if(ids.contains(mediaItemsList[i]->m_id))
            unsorted.insert(mediaItemsList[i]->m_id, mediaItemsList[i]);

    for(int i = 0; i < ids.count(); i++)
        sorted << unsorted[ids[i]];

    return sorted;
}

QList<MediaItem *> MediaDatabase::getItemsByURN(const QStringList &urns)
{
    QList<MediaItem *> sorted;

    for(int i = 0; i < urns.count(); i++)
        if(mediaItemsUrnHash.contains(urns[i]))
            sorted << mediaItemsUrnHash[urns[i]];

    return sorted;
}

void MediaDatabase::changeTitleByURN(QString urn, QString title)
{
    if(urn.isEmpty())
        return;

    MediaItem *item = NULL;
    QString oldtitle;
    if(mediaItemsUrnHash.contains(urn))
    {
        item = mediaItemsUrnHash[urn];
        oldtitle = item->m_title;
    }

    QString SqlCmd =
         "DELETE { <%1> nie:title ?title } WHERE { <%1> nie:title ?title } " \
         "INSERT { <%1> nie:title '%2' }";

    QString sql = QString(SqlCmd).arg(urn, title);
    trackerCall(sql);

    if(item != NULL)
    {
        if(item->isMusicPlaylist())
        {
            QString oldthumb = MediaItem::thumbPlaylist(oldtitle);
            QString newthumb = MediaItem::thumbPlaylist(title);
            if(!oldtitle.isEmpty()&&QFile::exists(oldthumb))
                QFile::rename(oldthumb, newthumb);
            item->m_thumburi = MediaItem::thumbPlaylistImageProvider(title);
        }
        item->m_title = title;
        broadcastMyChanges(item->m_id, MediaDatabase::Title);
    }
}

void MediaDatabase::changeTitle(QString uri, QString title)
{
    if(uri.isEmpty())
        return;

    QString SqlCmd =
         "DELETE { ?object nie:title ?unknown } WHERE { ?object nie:url '%1' ; nie:title ?unknown } " \
         "INSERT { ?object nie:title '%2' } WHERE { ?object nie:url '%1' } ";

    QString sql = QString(SqlCmd).arg(uri, title);
    trackerCallAsync(sql);

    for(int i = 0; i < mediaItemsList.count(); i++)
        if(mediaItemsList[i]->m_uri == uri)
        {
            mediaItemsList[i]->m_title = title;
            broadcastMyChanges(mediaItemsList[i]->m_id, MediaDatabase::Title);
        }
}

void MediaDatabase::setFavorite(const QString &urn, const bool &favorite)
{
    if(urn.isEmpty())
        return;

    QString SqlCmd =
        "DELETE {?tag a rdfs:Resource } WHERE {?object nao:hasTag ?tag . ?tag nao:identifier 'favorite' " \
        ". { SELECT ?object WHERE {?object a nie:InformationElement . FILTER (str(?object) = '%1') }}} " \
        "INSERT { _:tag a nao:Tag ; nao:prefLabel '%2' ; nao:identifier 'favorite' . ?object nao:hasTag _:tag }" \
        "WHERE { ?object a nie:InformationElement . FILTER (str(?object) = '%1') }";

    QString favstr = (favorite)?"favorite":"not";
    QString sql = QString(SqlCmd).arg(urn, favstr);
    trackerCallAsync(sql);
}

void MediaDatabase::setRecentlyViewed(const QString &urn, const QString &timestamp)
{
    if(urn.isEmpty())
        return;

    QString SqlCmd =
        "DELETE {?tag a rdfs:Resource } WHERE {?object nao:hasTag ?tag . ?tag nao:identifier 'viewed' " \
        ". { SELECT ?object WHERE {?object a nie:InformationElement . FILTER (str(?object) = '%1') }}} " \
        "INSERT { _:tag a nao:Tag ; nao:prefLabel '%2' ; nao:identifier 'viewed' . ?object nao:hasTag _:tag }" \
        "WHERE { ?object a nie:InformationElement . FILTER (str(?object) = '%1') }";

    QString sql = QString(SqlCmd).arg(urn, timestamp);
    trackerCallAsync(sql);
}

void MediaDatabase::setFavoriteMulti(const QStringList &ids, bool val)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        if(ids.contains(mediaItemsList[i]->getID()))
        {
            mediaItemsList[i]->setFavorite(val);
            if(mediaItemsList[i]->isVirtual())
                continue;
            setFavorite(mediaItemsList[i]->m_urn, val);
        }
    }
    broadcastMyChanges(ids, MediaDatabase::Favorited);
}

void MediaDatabase::setViewedMulti(const QStringList &ids)
{
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        if(ids.contains(mediaItemsList[i]->getID()))
        {
            QDateTime time = QDateTime();
            time.setTimeSpec(Qt::LocalTime);
            QString ctime = time.currentDateTime().toString(Qt::ISODate);
            mediaItemsList[i]->setRecentlyViewed(ctime);
            if(mediaItemsList[i]->isVirtual())
                continue;
            setRecentlyViewed(mediaItemsList[i]->m_urn, ctime);
        }
    }
    broadcastMyChanges(ids, MediaDatabase::Viewed);
}

void MediaDatabase::setPlayStatus(const QString &id, int playstatus)
{
    QList<MediaItem *> removedItemsList;
    MediaItem *item = NULL;
    QStringList ids;
    for(int i = 0; i < mediaItemsList.count(); i++)
    {
        if((mediaItemsList[i]->getID() == id)&&
           (mediaItemsList[i]->isSong()||mediaItemsList[i]->isAnyVideoType()))
        {
            item = mediaItemsList[i];
        }
        else if(mediaItemsList[i]->m_playstatus > 0)
        {
            removedItemsList << mediaItemsList[i];
        }
    }
    if((item == NULL)||item->isVirtual())
        return;

    /* set the target play status */
    item->m_playstatus = playstatus;
    ids << item->m_id;

    /* if target status is pause or play, stop everything else */
    if(playstatus > 0)
    {
        for(int i = 0; i < removedItemsList.count(); i++)
        {
            removedItemsList[i]->m_playstatus = 0;
            ids << removedItemsList[i]->m_id;
        }
    }
    broadcastMyChanges(ids, MediaDatabase::PlayStatus);
}

void MediaDatabase::destroyItem(MediaItem *item)
{
    QList<MediaItem *> removedItemsList;
    QStringList ids;

    if(item->isVirtual())
    {
        mediaItemsList.removeAll(item);
        mediaItemsIdHash.remove(item->m_id);
        ids << item->m_id;
        emit itemsRemoved(ids);
    }

    if(item->m_type == MediaItem::MusicPlaylistItem)
    {
        QString SqlCmd = "DELETE { ?playlist a nmm:Playlist } WHERE { ?playlist a nmm:Playlist . FILTER (str(?playlist) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        qDebug() << sql;
        trackerCallAsync(sql);

        /* remove the thumbnail as well */
        if(!item->m_title.isEmpty()&&QFile::exists(MediaItem::thumbPlaylist(item->m_title)))
        {
            QFile f(MediaItem::thumbPlaylist(item->m_title));
            f.remove();
        }
    }
    else if(item->m_type == MediaItem::SongItem)
    {
        QString SqlCmd = "DELETE { ?item a nmm:MusicPiece } WHERE { ?item a nmm:MusicPiece . FILTER (str(?item) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);
        QUrl url = QUrl::fromEncoded(item->m_uri.toAscii());
        qDebug() << "delete " << url.toLocalFile();
        /* Are you sure? (y/n) */
        QFile f(url.toLocalFile());
        f.remove();
    }
    else if(item->m_type == MediaItem::MusicAlbumItem)
    {
        /* Are you really seriously sure? (y/n) */
        for(int i = 0; i < mediaItemsList.count(); i++)
            if((mediaItemsList[i]->isSong())&&(mediaItemsList[i]->m_album == item->m_title))
            {
                QString SqlCmd = "DELETE { ?item a nmm:MusicPiece } WHERE { ?item a nmm:MusicPiece . FILTER (str(?item) = '%1')} ";
                QString sql = QString(SqlCmd).arg(mediaItemsList[i]->m_urn);
                trackerCallAsync(sql);
                QUrl url = QUrl::fromEncoded(mediaItemsList[i]->m_uri.toAscii());
                qDebug() << "album " << item->m_title << " delete " << url.toLocalFile();
                QFile f(url.toLocalFile());
                f.remove();
                removedItemsList << mediaItemsList[i];
                ids << mediaItemsList[i]->m_id;
            }
        QString SqlCmd = "DELETE { ?item a nmm:MusicAlbum } WHERE { ?item a nmm:MusicAlbum . FILTER (str(?item) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);

        /* remove the thumbnail as well */
        if(!item->m_thumburi.isEmpty()&&QFile::exists(item->m_thumburi)&&(item->m_thumbtype == MediaItem::AlbumThumb))
        {
            QFile f(item->m_thumburi);
            f.remove();
        }
    }
    else if(item->m_type == MediaItem::MusicArtistItem)
    {
        /* Are you really seriously totally freaking completely sure? (y/n) */
        for(int i = 0; i < mediaItemsList.count(); i++)
            if((mediaItemsList[i]->isSong())&&(mediaItemsList[i]->m_artist.contains(item->m_title)))
            {
                QString SqlCmd = "DELETE { ?item a nmm:MusicPiece } WHERE { ?item a nmm:MusicPiece . FILTER (str(?item) = '%1')} ";
                QString sql = QString(SqlCmd).arg(mediaItemsList[i]->m_urn);
                trackerCallAsync(sql);
                QUrl url = QUrl::fromEncoded(mediaItemsList[i]->m_uri.toAscii());
                qDebug() << "artist " << item->m_title << " delete " << url.toLocalFile();
                QFile f(url.toLocalFile());
                f.remove();
                removedItemsList << mediaItemsList[i];
                ids << mediaItemsList[i]->m_id;
            }
        QString SqlCmd = "DELETE { ?item a nmm:Artist } WHERE { ?item a nmm:Artist . FILTER (str(?item) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);

        /* remove the thumbnail as well */
        if(!item->m_thumburi.isEmpty()&&QFile::exists(item->m_thumburi))
        {
            QFile f(item->m_thumburi);
            f.remove();
        }
    }
    else if(item->m_type == MediaItem::PhotoAlbumItem)
    {
        QString SqlCmd = "DELETE { ?album a nmm:ImageList } WHERE { ?album a nmm:ImageList . FILTER (str(?album) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);
    }
    else if(item->m_type == MediaItem::PhotoItem)
    {
        QString SqlCmd = "DELETE { ?item a nmm:Photo } WHERE { ?item a nmm:Photo . FILTER (str(?item) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);
        QUrl url = QUrl::fromEncoded(item->m_uri.toAscii());
        qDebug() << "delete " << url.toLocalFile();
        /* Are you sure? (y/n) */
        QFile f(url.toLocalFile());
        f.remove();

        /* remove the thumbnail as well */
        if(!item->m_thumburi.isEmpty()&&QFile::exists(item->m_thumburi))
        {
            QFile f(item->m_thumburi);
            f.remove();
        }
    }
    else if(item->m_type == MediaItem::VideoItem)
    {
        QString SqlCmd = "DELETE { ?item a nmm:Video } WHERE { ?item a nmm:Video . FILTER (str(?item) = '%1')} ";
        QString sql = QString(SqlCmd).arg(item->m_urn);
        trackerCallAsync(sql);
        QUrl url = QUrl::fromEncoded(item->m_uri.toAscii());
        qDebug() << "delete " << url.toLocalFile();
        /* Are you sure? (y/n) */
        QFile f(url.toLocalFile());
        f.remove();

        /* remove the thumbnail as well */
        if(!item->m_thumburi.isEmpty()&&QFile::exists(item->m_thumburi))
        {
            QFile f(item->m_thumburi);
            f.remove();
        }
    }

    removedItemsList << item;
    ids << item->m_id;

    for(int i = 0; (i < removedItemsList.count())&&(i < ids.count()); i++)
    {
        mediaItemsList.removeAll(removedItemsList[i]);
        mediaItemsUrnHash.remove(removedItemsList[i]->m_urn);
        mediaItemsIdHash.remove(removedItemsList[i]->m_id);
        mediaItemsSidHash.remove(removedItemsList[i]->m_sid);
        /* delete should go here, but it needs to be delayed */
    }

    /* tell the world we have deletions */
    if(!ids.isEmpty())
        emit itemsRemoved(ids);
}

void MediaDatabase::trackerUpdates(QString classname, QVector<Quad> deletes, QVector<Quad> inserts)
{
//    qDebug() << "Classname: " << classname;
//    for(int i = 0; i < inserts.count(); i++)
//        qDebug() << "Inserts: " << inserts[i].graph << " " << inserts[i].subject << " " << inserts[i].predicate << " " << inserts[i].object;
//    for(int i = 0; i < deletes.count(); i++)
//        qDebug() << "Deletes: " << deletes[i].graph << " " << deletes[i].subject << " " << deletes[i].predicate << " " << deletes[i].object;

    /* need to get the tracker ids so we know what this update means */
    if(!getTrackerIDs())
        return;

    for(int i = 0; i < inserts.count(); i++)
    {
        int subject = inserts[i].subject;
        int predicate = inserts[i].predicate;
        int object = inserts[i].object;

        /* check for insertions first */
        if(predicate == tid.rdf_type)
        {
            if(classname.endsWith("nmm#Photo")&&(object == tid.nmm_Photo))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit photoItemAdded(subject);
            }
            else if(classname.endsWith("nmm#ImageList")&&(object == tid.nmm_ImageList))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit photoAlbumItemAdded(subject);
            }
            else if(classname.endsWith("nmm#MusicPiece")&&(object == tid.nmm_MusicPiece))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit songItemAdded(subject);
            }
            else if(classname.endsWith("nmm#MusicAlbum")&&(object == tid.nmm_MusicAlbum))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit musicAlbumItemAdded(subject);
            }
            else if(classname.endsWith("nmm#Artist")&&(object == tid.nmm_Artist))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit musicArtistItemAdded(subject);
            }
            else if(classname.endsWith("nmm#Playlist")&&(object == tid.nmm_Playlist))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit playlistItemAdded(subject);
            }
            else if(classname.endsWith("nmm#Video")&&(object == tid.nmm_Video))
            {
                if(!mediaItemsSidHash.contains(subject))
                    emit videoItemAdded(subject);
            }
            else if(classname.endsWith("nao#Tag")&&(object == tid.nao_Tag))
            {
                QStringList processed;
                int reason = Other;
                QString SqlCmd = "SELECT ?object nao:prefLabel(?tag) nao:identifier(?tag) " \
                                 "WHERE { ?object a nie:InformationElement . ?object nao:hasTag ?tag " \
                                 ". ?tag a nao:Tag . FILTER (tracker:id(?tag) = '%1')}";

                QString sql = QString(SqlCmd).arg(subject);
                QVector<QStringList> info;
                if(trackerCall(info, sql))
                {
                    for (QVector<QStringList>::iterator j = info.begin(); j != info.end(); j++)
                    {
                        QString urn = (*j)[0];
                        if(!urn.isEmpty()&&mediaItemsUrnHash.contains(urn)&&
                           !processed.contains(mediaItemsUrnHash[urn]->m_id))
                        {
                            QString tagvalue = (*j)[1];
                            QString tagname = (*j)[2];
                            MediaItem *item = mediaItemsUrnHash[urn];
                            processed << item->m_id;

                            qDebug() << "TAGCHANGE: " << item->m_title << ", " << tagname << ", " << tagvalue;
                            if(tagname == "favorite")
                            {
                                reason = Favorited;
                                item->setFavorite(tagvalue == "favorite");
                            }
                            else if(tagname == "viewed")
                            {
                                reason = Viewed;
                                item->setRecentlyViewed(tagvalue);
                            }
                            else if((tagname == "coverart")&&item->isPhotoAlbum())
                            {
                                reason = Thumbnail;
                                item->setCoverArt(tagvalue);
                            }
                        }
                    }
                }
                if(!processed.isEmpty())
                    emit itemsChanged(processed, reason);
            }
        }
        /* check for title changes next */
        else if((predicate == tid.nie_title)&&mediaItemsSidHash.contains(subject))
        {
            QStringList processed;
            QString SqlCmd = "SELECT ?urn ?title WHERE { ?urn nie:title ?title . FILTER (tracker:id(?urn) = '%1')}";
            QString sql = QString(SqlCmd).arg(subject);
            QVector<QStringList> info;
            if(trackerCall(info, sql))
            {
                for (QVector<QStringList>::iterator j = info.begin(); j != info.end(); j++)
                {
                    QString urn = (*j)[0];
                    if(!urn.isEmpty()&&mediaItemsUrnHash.contains(urn)&&
                       !processed.contains(mediaItemsUrnHash[urn]->m_id))
                    {
                        QString title = (*j)[1];
                        MediaItem *item = mediaItemsUrnHash[urn];
                        processed << item->m_id;

                        qDebug() << "TITLE CHANGED: " << item->m_title << ", " << title;
                        item->m_title = title;
                    }
                }
            }
            if(!processed.isEmpty())
                emit itemsChanged(processed, MediaDatabase::Title);
        }
    }

    /* check for deletions last */
    for(int i = 0; i < deletes.count(); i++)
    {
        if(deletes[i].predicate != tid.rdf_type)
            continue;

        int subject = deletes[i].subject;
        int object = deletes[i].object;

        if((classname.endsWith("nmm#Photo")&&(object == tid.nmm_Photo))||
           (classname.endsWith("nmm#ImageList")&&(object == tid.nmm_ImageList))||
           (classname.endsWith("nmm#MusicPiece")&&(object == tid.nmm_MusicPiece))||
           (classname.endsWith("nmm#MusicAlbum")&&(object == tid.nmm_MusicAlbum))||
           (classname.endsWith("nmm#Artist")&&(object == tid.nmm_Artist))||
           (classname.endsWith("nmm#Playlist")&&(object == tid.nmm_Playlist))||
           (classname.endsWith("nmm#Video")&&(object == tid.nmm_Video)))
        {
            if(!mediaItemsSidHash.contains(subject))
                continue;

            qDebug() << "trackerRemoved " << subject;
            MediaItem *item = mediaItemsSidHash[subject];
            QStringList ids;

            ids << item->m_id;
            mediaItemsList.removeAll(item);
            mediaItemsIdHash.remove(item->m_id);
            mediaItemsUrnHash.remove(item->m_urn);
            mediaItemsSidHash.remove(item->m_sid);
            /* delete should go here, but it needs to be delayed */

            /* tell the world we have deletions */
            emit itemsRemoved(ids);
        }
    }
}

void MediaDatabase::itemAdded(MediaItem *item)
{
    QList<MediaItem *> newItemsList;
    newItemsList << item;
    emit itemsAdded(&newItemsList);
}

void MediaDatabase::itemChanged(MediaItem *item, int reason)
{
    QStringList newItemsList;
    newItemsList << item->m_id;
    emit itemsChanged(newItemsList, reason);
}
