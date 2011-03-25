/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef MEDIAINFOPLUGININTERFACE_H
#define MEDIAINFOPLUGININTERFACE_H

#include <QtCore>

class MediaInfoPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit MediaInfoPluginInterface(QObject *parent = 0): QObject(parent) {}
    virtual ~MediaInfoPluginInterface() { }

    /* request: retrieve metadata from the network
     * arguments:
     *      id: the unique id for this transaction, returned with error/ready
     *      type: the type of data to gather
     *      info: the info needed to gather the data requested
     * return:
     *      bool: true if request was successfully issued
     *            false on syntax error, unsupported info type, etc
     *
     * commands:
     *      type: "artistimage"
     *      info: "artistname|imagefilename"
     *      data (ready signal): ignored
     *      description: requests an artist image of artistname and stores
     *            it as imagefilename. All images must be converted to
     *            jpeg format, it's up to the caller to provide the
     *            .jpeg extention.
     *
     *      type: "albumimage"
     *      info: "artistname|albumname|imagefilename"
     *      data (ready signal): ignored
     *      description: requests an album image of albumname with artist artistname,
     *            and stores it as imagefilename. All images must be converted to
     *            jpeg format, it's up to the caller to provide the
     *            .jpeg extention.
     *
     *      type: "dvdimage"
     *      info: "dvdtitle|imagefilename"
     *      data (ready signal): ignored
     *      description: requests an image of dvdtitle given,
     *            and stores it as imagefilename. All images must be converted to
     *            jpeg format, it's up to the caller to provide the
     *            .jpeg extention.
     */
    virtual bool request(QString id, QString type, QString info) = 0;

    /* supported: tests if a command is supported by this plugin
     * arguments:
     *      type: the request type
     * return:
     *      bool: true if supported, false otherwise
     */
    virtual bool supported(QString type) = 0;

    /* pluginName: returns the simple name of the plugin
     * return:
     *      QString: a simple name
     */
    virtual QString pluginName() = 0;

signals:
    /* error: signals an error for a transaction
     * arguments:
     *      id: the id given in the request call
     *      errorString: the error description, should also be triggered
     *            on bad request calls
     */
    void error(QString id, QString errorString);

    /* ready: returns the request data
     * arguments:
     *      id: the id given in the request call
     *      data: the requested data
     */
    void ready(QString id, QString data);
};
Q_DECLARE_INTERFACE(MediaInfoPluginInterface, "com.meego.MediaInfo.PluginInterface/0.1")

#endif // MEDIAINFOPLUGININTERFACE_H
