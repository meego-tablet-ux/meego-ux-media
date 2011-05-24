#include "resourcemanager.h"

using namespace ResourcePolicy;

ResourceManager::ResourceManager(QObject *parent) :
    QObject(parent)
{
    m_type = -1;
    m_ready = false;
    m_userwantsplayback = false;
    m_acquired = false;
}

ResourceManager::~ResourceManager()
{
    if(m_ready)
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

    m_ready = true;
}

void ResourceManager::setType(const int type)
{
    if(m_type >= 0)
        return;

    m_type = type;
    emit typeChanged(m_type);

    if(!m_ready&&!m_name.isEmpty())
        startup();
}

void ResourceManager::setName(const QString &name)
{
    if(!m_name.isEmpty())
        return;

    m_name = name;
    emit nameChanged(m_name);

    if(!m_ready&&(m_type >= 0))
        startup();
}

void ResourceManager::setUserwantsplayback(const bool userwantsplayback)
{
    if(!m_ready)
        return;

    m_userwantsplayback = userwantsplayback;
    emit userwantsplaybackChanged(m_userwantsplayback);

    if(userwantsplayback)
    {
        if(m_acquired)
            emit startPlaying();
        else
            acquire();
    }
    else
    {
        if(m_acquired)
            release();
    }
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
    m_acquired = false;
}

void ResourceManager::resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&)
{
    qDebug() << "ACQUIRED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    m_acquired = true;
    if(m_userwantsplayback)
        emit startPlaying();
}

void ResourceManager::resourceLostHandler()
{
    qDebug() << "LOST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    m_acquired = false;
    if(m_userwantsplayback)
        emit stopPlaying();
}
