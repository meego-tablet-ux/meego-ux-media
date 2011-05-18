#include "resourcemanager.h"

using namespace ResourcePolicy;

ResourceManager::ResourceManager(QObject *parent) :
    QObject(parent)
{
    m_type = -1;
    ready = false;
}

ResourceManager::~ResourceManager()
{
    if(ready)
        resourceSet->release();
}

void ResourceManager::startup()
{
    resourceSet = new ResourceSet(m_name, this);
    resourceSet->setAlwaysReply();

    switch(m_type) {
    case MusicApp:
    case VideoApp:
        audioResource = new ResourcePolicy::AudioResource(m_name);
        audioResource->setProcessID(QCoreApplication::applicationPid());
        audioResource->setStreamTag("media.name", "*");
        resourceSet->addResourceObject(audioResource);
        break;
    }

    connect(resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)),
            this,        SLOT(resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&)));
    connect(resourceSet, SIGNAL(lostResources()),     this, SLOT(resourceLostHandler()));
    connect(resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));

    ready = true;
}

void ResourceManager::setType(const int type)
{
    if(m_type >= 0)
        return;

    m_type = type;
    emit typeChanged(m_type);

    if(!ready&&!m_name.isEmpty())
        startup();
}

void ResourceManager::setName(const QString &name)
{
    if(!m_name.isEmpty())
        return;

    m_name = name;
    emit nameChanged(m_name);

    if(!ready&&(m_type >= 0))
        startup();
}

void ResourceManager::acquire()
{
    if (m_type == VideoApp) {
      resourceSet->addResource(ResourcePolicy::VideoPlaybackType);
    } else {
      resourceSet->deleteResource(ResourcePolicy::VideoPlaybackType);
    }

    resourceSet->acquire();
}

void ResourceManager::release()
{
    resourceSet->release();
}

void ResourceManager::resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>& /*grantedOptionalResList*/)
{
    qDebug("PlayerWidget::resourceAcquiredHandler()");
    emit acquired();
}

void ResourceManager::resourceReleasedHandler()
{
    qDebug("PlayerWidget::resourceReleasedHandler()");
}

void ResourceManager::resourceLostHandler()
{
    qDebug("PlayerWidget::resourceLostHandler()");
    emit lost();
}
