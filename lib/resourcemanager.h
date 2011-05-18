#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <policy/resource-set.h>

class ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject *parent = 0);

signals:
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

};

#endif // RESOURCEMANAGER_H
