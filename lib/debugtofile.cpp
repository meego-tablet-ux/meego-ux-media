#include "debugtofile.h"

DebugToFile::DebugToFile(QObject *parent) :
    QObject(parent)
{
    ready = false;
}

void DebugToFile::setFile(const QString &file)
{
    if((m_file == file)||file.isEmpty())
        return;

    if(!m_file.isEmpty())
        fp.close();

    m_file = file;
    QString filePath = QDir::toNativeSeparators(QDir::homePath()) + QDir::separator() + m_file;
    fp.setFileName(filePath);
    fp.open(QIODevice::WriteOnly | QIODevice::Text);
    ready = true;
}

void DebugToFile::print(const QString &text)
{
    if(!ready)
        return;

    fp.write(text.toAscii());
    fp.flush();
}
