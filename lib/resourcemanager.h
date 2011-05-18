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

signals:
    void typeChanged(const int type);
    void nameChanged(const QString &name);
    void acquired();
    void lost();

public slots:
    void resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& /*grantedOptionalResList*/);
    void resourceReleasedHandler();
    void resourceLostHandler();
    void acquire();
    void release();

private:
    ResourcePolicy::ResourceSet *resourceSet;
    ResourcePolicy::AudioResource *audioResource;
    void startup();

    QString m_name;
    int m_type;
    bool ready;
};

#endif // RESOURCEMANAGER_H
