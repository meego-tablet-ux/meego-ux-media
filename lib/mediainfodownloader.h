/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MEDIAINFODOWNLOADER_H
#define MEDIAINFODOWNLOADER_H

#include <QtCore>
#include <QtCore/QtGlobal>
#include <QHash>
#include <mediainfoplugininterface.h>

class MGConfItem;

struct RequestInfo
{
    QString type;
    QString info;
    QString plugin;
};

class MediaInfoDownloader : public QObject
{
    Q_OBJECT;

public:
    MediaInfoDownloader();
    ~MediaInfoDownloader();

    bool request(QString id, QString type, QString info);
    bool supported(QString type);

signals:
    void error(QString id, QString type, QString info, QString errorString);
    void ready(QString id, QString type, QString info, QString data);

private slots:
    void error(QString id, QString errorString);
    void ready(QString id, QString data);
    void enabledChanged();
    void disabledChanged();
    void priorityChanged();

private:
    QList<QPluginLoader *> m_plugins;
    QHash<QString, RequestInfo> m_requests;
    MGConfItem *enabledItem;
    bool enabled;
    MGConfItem *disabledItem;
    QStringList disabled;
    MGConfItem *priorityItem;
    QStringList priority;
};

#endif // MEDIAINFODOWNLOADER_H
