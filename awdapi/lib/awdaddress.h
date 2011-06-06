#ifndef AWDADDRESS_H
#define AWDADDRESS_H

#include <QObject>

class AwdAddress : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString localPort READ localPort CONSTANT)
    Q_PROPERTY(QString uid READ uid WRITE setUid NOTIFY uidChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString widgetId READ widgetId WRITE setWidgetId NOTIFY widgetIdChanged)
    Q_PROPERTY(QString sendDataMode READ sendDataMode WRITE setSendDataMode NOTIFY sendDataModeChanged)
public:
    explicit AwdAddress(QObject *parent = 0);

    QString localPort() const { return m_localPort; }

    QString uid() const { return m_uid; }
    void setUid(QString uid);

    QString name() const { return m_name; }
    void setName(QString name);

    QString type() const { return m_type; }
    void setType(QString type);

    QString widgetId() const { return m_widgetId; }
    void setWidgetId(QString widgetId);

    QString sendDataMode() const { return m_sdm; }
    void setSendDataMode(QString sdm);

Q_SIGNALS:
    void uidChanged();
    void nameChanged();
    void typeChanged();
    void widgetIdChanged();
    void sendDataModeChanged();

public slots:
    QString getAddress(QString command, QString data="");

private:
    QString m_localPort;
    QString m_uid;
    QString m_name;
    QString m_type;
    QString m_widgetId;
    QString m_sdm;
};

#endif // AWDADDRESS_H
