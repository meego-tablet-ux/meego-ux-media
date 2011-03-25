/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QtCore/QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <QString>
#include <MGConfItem>
#include "mediainfodownloader.h"

#define GCONFKEY_SWITCH "/MeeGo/Media/music-metadata-download-enabled"
#define GCONFKEY_DISABLED "/MeeGo/Media/music-metadata-plugin-disabled"
#define GCONFKEY_PRIORITY "/MeeGo/Media/music-metadata-plugin-priority"
#define PLUGIN_RELPATH "/MeeGo/Media/metadata"

MediaInfoDownloader::MediaInfoDownloader()
{
    enabled = true;
    QPluginLoader *plugin = NULL;
    MediaInfoPluginInterface *intf = NULL;

    QString homePath = QDir::toNativeSeparators(QDir::homePath());
    QString thumbnail_folder = homePath + QDir::separator() + QString(".cache") + QDir::separator() + QString("media-art");

    QDir thumbdir(thumbnail_folder);
    if(!thumbdir.exists())
        thumbdir.mkpath(thumbnail_folder);

    foreach (QString path, QCoreApplication::libraryPaths()) {
        QDir dir = QDir(path + PLUGIN_RELPATH);
        foreach (QString file, dir.entryList(QStringList() << QString("*.so"))) {
            plugin = new QPluginLoader(dir.absoluteFilePath(file), this);
            if (!plugin->load()) {
                qDebug() << QString("Couldn't load plugin %1: %2").arg(file, plugin->errorString());
                continue;
            }
            intf = qobject_cast<MediaInfoPluginInterface *>(plugin->instance());
            if(intf == NULL) {
                qDebug() << "Failed to get the instance from the plugin";
                continue;
            }
            qDebug() << "Loaded media info plugin: " << intf->pluginName();
            m_plugins << plugin;
            connect(intf,SIGNAL(error(QString, QString)),this,SLOT(error(QString, QString)));
            connect(intf,SIGNAL(ready(QString, QString)),this,SLOT(ready(QString, QString)));
        }
    }

    /* this key is used to completely shut down the interface from settings */
    enabledItem = new MGConfItem(GCONFKEY_SWITCH);
    connect(enabledItem,SIGNAL(valueChanged()),this,SLOT(enabledChanged()));
    enabledChanged();

    /* this key is used to disable a list of plugins */
    disabledItem = new MGConfItem(GCONFKEY_DISABLED);
    connect(disabledItem,SIGNAL(valueChanged()),this,SLOT(disabledChanged()));
    disabledChanged();

    /* this key is used to set the priority order of the plugins */
    priorityItem = new MGConfItem(GCONFKEY_PRIORITY);
    connect(priorityItem,SIGNAL(valueChanged()),this,SLOT(priorityChanged()));
    priorityChanged();
}

MediaInfoDownloader::~MediaInfoDownloader()
{
    for(int i = 0; i < m_plugins.count(); i++)
    {
        m_plugins[i]->unload();
        m_plugins[i]->deleteLater();
    }
    m_plugins.clear();
}

void MediaInfoDownloader::enabledChanged()
{
    enabled = enabledItem->value(true).toBool();
}

void MediaInfoDownloader::disabledChanged()
{
    disabled = disabledItem->value("").toStringList();
}

void MediaInfoDownloader::priorityChanged()
{
    priority = priorityItem->value("").toStringList();
    int i, j1, j2;
    for(i = 0, j1 = 0; (i < priority.count())&&(j1 < m_plugins.count()); i++)
    {
        for(j2 = j1; j2 < m_plugins.count(); j2++)
        {
            MediaInfoPluginInterface *plugin = qobject_cast<MediaInfoPluginInterface *>(m_plugins[j2]->instance());
            if(priority[i] == plugin->pluginName())
            {
                if(j1 != j2)
                {
                    m_plugins.swap(j1, j2);
                }
                j1++;
                break;
            }
        }
    }
}

bool MediaInfoDownloader::supported(QString type)
{
    bool result = false;
    for(int i = 0; i < m_plugins.count(); i++)
    {
        MediaInfoPluginInterface *plugin = qobject_cast<MediaInfoPluginInterface *>(m_plugins[i]->instance());
        if(disabled.contains(plugin->pluginName()))
            continue;
        if(plugin->supported(type))
            result = true;
    }
    return result;
}

bool MediaInfoDownloader::request(QString id, QString type, QString info)
{
    if(!enabled)
    {
        emit error(id, type, info, "media info download is currently disabled");
        return false;
    }

    bool result = false;
    bool attempted = false;
    for(int i = 0; i < m_plugins.count(); i++)
    {
        MediaInfoPluginInterface *plugin = qobject_cast<MediaInfoPluginInterface *>(m_plugins[i]->instance());
        if(disabled.contains(plugin->pluginName()))
            continue;
        if(plugin->supported(type))
        {
            result = plugin->request(id, type, info);
            if(result)
            {
                RequestInfo data;
                data.info = info;
                data.type = type;
                data.plugin = plugin->pluginName();
                m_requests.insert(id, data);
            }
            attempted = true;
            break;
        }
    }

    if(!attempted)
        emit error(id, type, info, "No plugin found which supports this action");
    else if(!result)
        emit error(id, type, info, "Command failed");

    return result;
}

void MediaInfoDownloader::error(QString id, QString errorString)
{
    if(m_requests.contains(id))
    {
        emit error(id, m_requests[id].type, m_requests[id].info, errorString);
        m_requests.remove(id);
    }
}

void MediaInfoDownloader::ready(QString id, QString data)
{
    if(m_requests.contains(id))
    {
        emit ready(id, m_requests[id].type, m_requests[id].info, data);
        m_requests.remove(id);
    }
}
