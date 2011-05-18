#include "resourcemanager.h"

using namespace ResourcePolicy;

ResourceManager::ResourceManager(QObject *parent) :
    QObject(parent)
{
    resourceSet = new ResourceSet("player", this);
    resourceSet->setAlwaysReply();

    audioResource = new ResourcePolicy::AudioResource("player");
    audioResource->setProcessID(QCoreApplication::applicationPid());
    audioResource->setStreamTag("media.name", "*");
    resourceSet->addResourceObject(audioResource);

    connect(resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)),
            this,        SLOT(resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&)));
    connect(resourceSet, SIGNAL(lostResources()),     this, SLOT(resourceLostHandler()));
    connect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));
}

void ResourceManager::acquire() {
  resourceSet->acquire();
}

void ResourceManager::release() {
  resourceSet->release();
}

void ResourceManager::resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& /*grantedOptionalResList*/) {
  qDebug("PlayerWidget::resourceAcquiredHandler()");
  emit acquired();
}

void ResourceManager::resourceReleasedHandler() {
  qDebug("PlayerWidget::resourceReleasedHandler()");
}

void ResourceManager::resourceLostHandler() {
  qDebug("PlayerWidget::resourceLostHandler()");
  emit lost();
}
