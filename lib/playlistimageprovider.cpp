#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QSize>
#include "mediaitem.h"
#include "playlistimageprovider.h"

PlaylistImageProvider::PlaylistImageProvider() :
        QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{
}

PlaylistImageProvider::~PlaylistImageProvider()
{
}

QPixmap PlaylistImageProvider::requestPixmap(const QString &id, QSize *size,
                                           const QSize &requestedSize)
{
    QPixmap pixmap;
    int width = requestedSize.width();
    int height = requestedSize.height();

    QString title = id.split("?")[0];
    QString imagepath = MediaItem::thumbPlaylist(title);

    //  If we have a custom icon then use it
    if (QFile::exists(imagepath))
    {
        pixmap.load(imagepath);

        if (width > 0 && height > 0)
        {
            pixmap = pixmap.scaled(QSize(width, height));
        }
        else if (width > 0)
        {
            pixmap = pixmap.scaledToWidth(width);
        }
        else if (height > 0)
        {
            pixmap = pixmap.scaledToHeight(height);
        }
    }
    else
    {
        qDebug() << QString("Failed to find theme image requested: %1").arg(id);
        // Return a red pixmap to assist in finding missing images
        if (width <= 0)
            width = 100;
        if (height <= 0)
            height = 100;
        pixmap = QPixmap(QSize(width, height));
        pixmap.fill(Qt::red);
    }

    if (size)
        *size = pixmap.size();

    return pixmap;
}
