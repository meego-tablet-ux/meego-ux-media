#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <policy/resource-set.h>

class ResourceManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(AppType)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(int type READ getType WRITE setType NOTIFY typeChanged);
    Q_PROPERTY(bool userwantsplayback READ getUserwantsplayback WRITE setUserwantsplayback NOTIFY userwantsplaybackChanged);

public:
    ResourceManager(QObject *parent = 0);
    ~ResourceManager();

    enum AppType {
        MusicApp = 0,
        VideoApp = 1
    };

    QString getName() const
        { return m_name; }
    void setName(const QString &name);
    int getType() const
        { return m_type; }
    void setType(const int type);
    bool getUserwantsplayback() const
        { return m_userwantsplayback; }
    void setUserwantsplayback(const bool userwantsplayback);

signals:
    void typeChanged(const int type);
    void nameChanged(const QString &name);
    void userwantsplaybackChanged(const bool userwantsplayback);
    void startPlaying();
    void stopPlaying();

private slots:
    void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&);
    void resourceLostHandler();

private:
    ResourcePolicy::ResourceSet *resourceSet;
    ResourcePolicy::AudioResource *audioResource;
    void startup();
    void acquire();
    void release();

    QString m_name;
    int m_type;
    bool m_userwantsplayback;
    bool m_acquired;
    bool m_ready;
};

#endif // RESOURCEMANAGER_H
