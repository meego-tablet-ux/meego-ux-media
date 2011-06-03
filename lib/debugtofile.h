#ifndef DEBUGTOFILE_H
#define DEBUGTOFILE_H

#include <QObject>
#include <QtCore/QtCore>

class DebugToFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString file READ getFile WRITE setFile NOTIFY fileChanged)

public:
    explicit DebugToFile(QObject *parent = 0);

    QString getFile() const
        { return m_file; }
    void setFile(const QString &file);

signals:
    void fileChanged(const QString &file);

public slots:
    void print(const QString &text);

private:
    QString m_file;
    QFile fp;
    bool ready;
};

#endif // DEBUGTOFILE_H
