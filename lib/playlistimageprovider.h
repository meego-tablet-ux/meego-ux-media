#ifndef PLAYLISTIMAGEPROVIDER_H
#define PLAYLISTIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class PlaylistImageProvider : public QDeclarativeImageProvider
{
public:
    PlaylistImageProvider();
    ~PlaylistImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // PLAYLISTIMAGEPROVIDER_H
