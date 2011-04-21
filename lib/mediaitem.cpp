/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "mediaitem.h"
#include <inttypes.h>

MediaItem::MediaItem(int type, QDateTime recenttime, QStringList args, QObject *parent) :
    QObject(parent), m_type(type)
{
    m_favorite = false;
    m_recentlyadded = false;
    m_recentlyviewed = false;
    m_id = QString().sprintf("0x%08lX", (uintptr_t)this);
    m_thumburi_exists = false;
    m_thumburi_ignore = true;
    m_thumbtype = NoThumb;
    artistitem = NULL;
    albumitem = NULL;
    m_tracknum = 0;
    m_length = 0;
    m_isvirtual = false;
    m_playstatus = 0;
    m_sid = 0;
    m_width = 0;
    m_height = 0;
    m_isusercontent = false;
    changeData(recenttime, args);
}

MediaItem::MediaItem(int type, QString uri, QObject *parent) :
        QObject(parent), m_type(type)
{
    m_favorite = false;
    m_recentlyadded = false;
    m_recentlyviewed = false;
    m_id = QString().sprintf("0x%08lX", (uintptr_t)this);
    m_thumburi_exists = false;
    m_thumburi_ignore = true;
    m_thumbtype = NoThumb;
    artistitem = NULL;
    albumitem = NULL;
    m_tracknum = 0;
    m_length = 0;
    m_isvirtual = true;
    m_playstatus = 0;
    m_sid = 0;
    m_width = 0;
    m_height = 0;
    m_isusercontent = false;

    if(isAnyVideoType())
    {
        m_uri = (QUrl::fromEncoded(fileFormatted(uri).toAscii())).toString();
        QStringList temp = m_uri.split("/", QString::SkipEmptyParts);
        m_title = temp.last();
        m_thumbtype = VideoThumb;
        m_thumburi = thumbPhoto(m_uri);
        m_thumburi_ignore = false;
        if(thumbExists())
            m_thumburi_exists = true;
    }
    else if(isSong())
    {
        m_uri = (QUrl::fromEncoded(fileFormatted(uri).toAscii())).toString();
        QStringList temp = m_uri.split("/", QString::SkipEmptyParts);
        m_title = temp.last();
        m_thumburi_exists = false;
        m_thumburi_ignore = true;
    }
    else if(isPhoto())
    {
        m_uri = (QUrl::fromEncoded(fileFormatted(uri).toAscii())).toString();
        QStringList temp = m_uri.split("/", QString::SkipEmptyParts);
        m_title = temp.last();
        m_thumbtype = PhotoThumb;
        m_thumburi = thumbPhoto(m_uri);
        m_thumburi_ignore = false;
        if(thumbExists())
            m_thumburi_exists = true;
    }
}

MediaItem::MediaItem(int type, QObject *parent) :
    QObject(parent), m_type(type)
{
    m_favorite = false;
    m_recentlyadded = false;
    m_recentlyviewed = false;
    m_id = QString().sprintf("0x%08lX", (uintptr_t)this);
    m_thumburi_exists = false;
    m_thumburi_ignore = true;
    m_thumbtype = NoThumb;
    artistitem = NULL;
    albumitem = NULL;
    m_tracknum = 0;
    m_length = 0;
    m_isvirtual = true;
    m_playstatus = 0;
    m_sid = 0;
    m_width = 0;
    m_height = 0;
    m_isusercontent = false;
}

MediaItem::MediaItem(QObject *parent) :
    QObject(parent)
{
    m_type = PhotoItem;
    m_favorite = false;
    m_recentlyadded = false;
    m_recentlyviewed = false;
    m_id = QString().sprintf("0x%08lX", (uintptr_t)this);
    m_thumburi_exists = false;
    m_thumburi_ignore = true;
    m_thumbtype = NoThumb;
    artistitem = NULL;
    albumitem = NULL;
    m_tracknum = 0;
    m_length = 0;
    m_isvirtual = true;
    m_playstatus = 0;
    m_sid = 0;
    m_width = 0;
    m_height = 0;
    m_isusercontent = false;
}

void MediaItem::changeData(QDateTime recenttime, QStringList args)
{
    m_urn = args.at(IDX_URN);
    if(!args.at(IDX_SUBJECTID).isEmpty())
        m_sid = args.at(IDX_SUBJECTID).toInt();
    m_addedtime = args.at(IDX_ADDTIME);
    m_lastplayedtime = args.at(IDX_VIEWED);
    m_favorite = (args.at(IDX_FAVORITE) == "favorite");

    if(isSong() && (args.count() == SNG_ARGS))
    {
        m_title = args.at(IDX_SNG_TITLE);
        m_uri = (QUrl::fromEncoded(args.at(IDX_SNG_URI).toAscii())).toString();
        m_creationtime = args.at(IDX_SNG_CTIME);
        if(!args.at(IDX_SNG_TRACK).isEmpty())
            m_tracknum = args.at(IDX_SNG_TRACK).toInt();
        if(!args.at(IDX_SNG_DURATION).isEmpty())
            m_length = args.at(IDX_SNG_DURATION).toInt();
        if(!args.at(IDX_SNG_ARTISTNAME).isEmpty())
            m_artist << args.at(IDX_SNG_ARTISTNAME);
        if(!args.at(IDX_SNG_ARTISTURN).isEmpty())
            m_artist_urn << args.at(IDX_SNG_ARTISTURN);
        m_album = args.at(IDX_SNG_ALBUMNAME);
        m_album_urn = args.at(IDX_SNG_ALBUMURN);
        m_mimetype = args.at(IDX_SNG_MIME);
        if(args.at(IDX_SNG_USER) == USER_CONTENT_STRING)
            m_isusercontent = true;

        if(!m_artist.isEmpty()&&!m_album.isEmpty())
        {
            m_thumbtype = AlbumThumb;
            m_thumburi = thumbMusicAlbum(m_artist.first(), m_album);
            m_thumburi_ignore = false;
            if(thumbExists())
                m_thumburi_exists = true;
        }
        if(!m_artist.isEmpty()&&!m_thumburi_exists)
        {
            QString temp = thumbMusicArtist(m_artist.first());
            if(m_thumbtype == NoThumb)
            {
                m_thumbtype = ArtistThumb;
                m_thumburi = temp;
                m_thumburi_ignore = false;
            }
            if(thumbExists(temp))
            {
                m_thumbtype = ArtistThumb;
                m_thumburi = temp;
                m_thumburi_ignore = false;
                m_thumburi_exists = true;
            }
        }
        if(m_title.isEmpty()&&!m_uri.isEmpty())
        {
            QStringList temp = m_uri.split("/", QString::SkipEmptyParts);
            m_title = temp.last();
        }
    }
    else if(isMusicAlbum() && (args.count() == MAL_ARGS))
    {
        m_title = args.at(IDX_MAL_TITLE);
        if(!args.at(IDX_MAL_ARTISTNAME).isEmpty())
            m_artist << args.at(IDX_MAL_ARTISTNAME);
        if(!args.at(IDX_MAL_ARTISTURN).isEmpty())
            m_artist_urn << args.at(IDX_MAL_ARTISTURN);
        if(!m_artist.isEmpty()&&!m_title.isEmpty())
        {
            m_thumbtype = AlbumThumb;
            m_thumburi = thumbMusicAlbum(m_artist.first(), m_title);
            m_thumburi_ignore = false;
            if(thumbExists())
                m_thumburi_exists = true;
        }
        if(!m_artist.isEmpty()&&!m_thumburi_exists)
        {
            QString temp = thumbMusicArtist(m_artist.first());
            if(thumbExists(temp))
            {
                m_thumbtype = ArtistThumb;
                m_thumburi = temp;
                m_thumburi_ignore = false;
                m_thumburi_exists = true;
            }
        }
    }
    else if(isMusicArtist() && (args.count() == ART_ARGS))
    {
        m_title = args.at(IDX_ART_NAME);
        if(!m_title.isEmpty())
        {
            m_thumbtype = ArtistThumb;
            m_thumburi = thumbMusicArtist(m_title);
            m_thumburi_ignore = false;
            if(thumbExists())
                m_thumburi_exists = true;
        }
    }
    else if(isMusicPlaylist() && (args.count() == MPL_ARGS))
    {
        m_title = args.at(IDX_MPL_TITLE);
        if(!args.at(IDX_MPL_COUNT).isEmpty())
            m_tracknum = args.at(IDX_MPL_COUNT).toInt();
        if(!m_title.isEmpty())
        {
            m_thumbtype = PlaylistThumb;
            m_thumburi = thumbPlaylistImageProvider(m_title);
            m_thumburi_ignore = false;
            if(thumbExists(thumbPlaylist(m_title)))
                m_thumburi_exists = true;
        }
    }
    else if(isPhoto() && (args.count() == PHO_ARGS))
    {
        m_uri = (QUrl::fromEncoded(args.at(IDX_PHO_URI).toAscii())).toString();
        m_creationtime = args.at(IDX_PHO_CTIME);
        m_mimetype = args.at(IDX_PHO_MIME);
        m_title = args.at(IDX_PHO_TITLE);
        if(!args.at(IDX_PHO_WIDTH).isEmpty())
            m_width = args.at(IDX_PHO_WIDTH).toInt();
        if(!args.at(IDX_PHO_HEIGHT).isEmpty())
            m_height = args.at(IDX_PHO_HEIGHT).toInt();
        if(args.at(IDX_PHO_USER) == USER_CONTENT_STRING)
            m_isusercontent = true;
        if(!m_uri.isEmpty())
        {
            if(m_title.isEmpty())
            {
                QStringList temp = m_uri.split("/", QString::SkipEmptyParts);
                m_title = temp.at(temp.count()-1);
            }
            m_thumbtype = PhotoThumb;
            m_thumburi = thumbPhoto(m_uri);
            m_thumburi_ignore = false;
            if(thumbExists())
                m_thumburi_exists = true;
        }
    }
    else if(isPhotoAlbum() && (args.count() == PAL_ARGS))
    {
        m_uri = args.at(IDX_PAL_COVER);
        m_title = args.at(IDX_PAL_TITLE);
        if(!m_uri.isEmpty())
        {
            /* cover art tag */
            m_thumbtype = PhotoThumb;
            m_thumburi = m_uri;
            m_thumburi_ignore = false;
            if(thumbExists())
                m_thumburi_exists = true;
        }
    }
    else if(isAnyVideoType() && (args.count() == VID_ARGS))
    {
        m_uri = (QUrl::fromEncoded(args.at(IDX_VID_URI).toAscii())).toString();
        m_title = (QUrl::fromEncoded(args.at(IDX_VID_TITLE).toAscii())).toString();
        if(!args.at(IDX_VID_DURATION).isEmpty())
            m_length = args.at(IDX_VID_DURATION).toInt();
        m_mimetype = args.at(IDX_VID_MIME);
        if(args.at(IDX_VID_USER) == USER_CONTENT_STRING)
            m_isusercontent = true;

        if(!m_uri.isEmpty())
        {
            m_thumbtype = VideoThumb;
            m_thumburi = thumbPhoto(m_uri);
            m_thumburi_ignore = false;
            if(thumbExists())
                m_thumburi_exists = true;
        }
        if(m_title.isEmpty()&&!m_uri.isEmpty())
        {
            QStringList temp = m_uri.split("/", QString::SkipEmptyParts);
            m_title = temp.last();
        }
    }

    /* calculate recently added and recently viewed */
    QDateTime ctime = QDateTime::currentDateTime();
    if(!m_addedtime.isEmpty())
    {
        /* if item was added within X days ago it's recent */
        QDateTime atime = QDateTime::fromString(m_addedtime, Qt::ISODate);
        if(ctime.daysTo(atime) > DAYSRECENT)
            m_recentlyadded = true;
    }
    if(!m_lastplayedtime.isEmpty())
    {
        /* if item was viewed before recenttime it's recent */
        QDateTime ptime = QDateTime::fromString(m_lastplayedtime, Qt::ISODate);
        if(ptime > recenttime)
            m_recentlyviewed = true;
    }

    if(!m_creationtime.isEmpty())
        m_timestamp = QDateTime::fromString(m_creationtime, Qt::ISODate);
    else if(!m_addedtime.isEmpty())
        m_timestamp = QDateTime::fromString(m_addedtime, Qt::ISODate);
}

QString MediaItem::stripInvalidEntities(const QString &src)
{
    QString source(src);

    // http://live.gnome.org/MediaArtStorageSpec
    // remove blocks and special characters
    QRegExp removeblocks( "(\\([^\\)]*\\))"
                          "|(\\[[^\\]]*\\])"
                          "|(\\{[^\\}]*\\})"
                          "|(\\<[^\\>]*\\>)");
    source.replace(removeblocks, "");

    // remove special characters
    source.remove(QRegExp("[\\(\\)\\_\\{\\}\\[\\]\\!\\@\\#\\$\\^\\&\\*\\+\\=\\|\\\\\\/\\\"\\'\\?\\<\\>\\~\\`]+"));

    // replace tabs with spaces
    source.replace('\t', " ");

    // replace space sequences with single spaces
    source.replace(QRegExp("\\s+"), " ");

    // remove leading and trailing spaces
    source.remove(QRegExp("(^\\s+)|(\\s+$)"));

    if(source.isEmpty())
        return "";

    return source.toLower();
}

QString MediaItem::thumbMusicAlbum(const QString &artist, const QString &album)
{
    if(artist.isEmpty()||album.isEmpty())
        return "";

    QString albumInfo = stripInvalidEntities(album);
    QString artistInfo = stripInvalidEntities(artist);

    if(albumInfo.isEmpty()||artistInfo.isEmpty())
        return "";

    QByteArray album_md5 =
        QCryptographicHash::hash(albumInfo.toAscii(),
                                 QCryptographicHash::Md5);
    QByteArray artist_md5 =
        QCryptographicHash::hash(artistInfo.toAscii(),
                                 QCryptographicHash::Md5);

    QString thumb = QDir::toNativeSeparators(QDir::homePath()) +
        QDir::separator() + QString(".cache/media-art/album-") +
        artist_md5.toHex() + "-" + album_md5.toHex() + ".jpeg";
    return thumb;
}

QString MediaItem::thumbMusicArtist(const QString &artist)
{
    if(artist.isEmpty())
        return "";

    QString artistInfo = stripInvalidEntities(artist);

    if(artistInfo.isEmpty())
        return "";

    QByteArray artist_md5 =
        QCryptographicHash::hash(artistInfo.toAscii(),
                                 QCryptographicHash::Md5);

    QString thumb = QDir::toNativeSeparators(QDir::homePath()) +
        QDir::separator() + QString(".cache/media-art/artist-") +
        artist_md5.toHex() + ".jpeg";
    return thumb;
}

QString MediaItem::thumbPhoto(const QString &uri)
{
    QString homePath = QDir::toNativeSeparators(QDir::homePath());
    QString thumbnail_folder = QDir::separator() + QString(".thumbnails") + QDir::separator() + QString(DEFAULT_FLAVOR);
    md5Result = QCryptographicHash::hash(uri.toUtf8(), QCryptographicHash::Md5);
    QString thumburi = QString("file://") + homePath + thumbnail_folder +
        QDir::separator() + md5Result.toHex() + ".png";
    return thumburi;
}

QString MediaItem::thumbPlaylistDir()
{
    QString homePath = QDir::toNativeSeparators(QDir::homePath());
    QString thumbnail_folder = homePath + QDir::separator() + QString(".thumbnails") + QDir::separator() + QString("playlists");
    return thumbnail_folder;
}

QString MediaItem::thumbPlaylist(const QString &title)
{
    if(title.isEmpty())
        return "";

    return (thumbPlaylistDir() + QDir::separator() + title + ".png");
}

QString MediaItem::thumbPlaylistImageProvider(const QString &title)
{
    if(title.isEmpty())
        return "";

    return (QString("image://playlist/") + title);
}

void MediaItem::setFavorite(const bool &favorite)
{
    m_favorite = favorite;
}

void MediaItem::setRecentlyViewed(const QString &timestamp)
{
    m_recentlyviewed = true;
    m_lastplayedtime = timestamp;
}

void MediaItem::setCoverArt(const QString &thumburi)
{
    m_uri = thumburi;
    m_thumburi = thumburi;
    m_thumbtype = PhotoThumb;
    m_thumburi_ignore = false;
    if(thumbExists())
        m_thumburi_exists = true;
}

bool MediaItem::thumbExists(const QString &name)
{
    QString temp;
    if(name.isEmpty())
        temp = m_thumburi;
    else
        temp = name;
    temp.replace("file://", "");
    return QFile::exists(temp);
}

QString MediaItem::fileFormatted(const QString &file)
{
    QString newfile = (QUrl::fromEncoded(file.toAscii())).toString();
    if(!newfile.startsWith("file://"))
        newfile.prepend("file://");
    return newfile;
}
