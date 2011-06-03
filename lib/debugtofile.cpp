#include "debugtofile.h"

DebugToFile::DebugToFile(QObject *parent) :
    QObject(parent)
{
    ready = false;
}

void DebugToFile::setFile(const QString &file)
{
    if(m_file == file)
        return;

    if(m_file.isEmpty())
        return;

    QString filePath = QDir::toNativeSeparators(QDir::homePath()) + QDir::separator() + m_file;
    fp.setFileName(filePath);
    ready = true;
}

void DebugToFile::print(const QString &text)
{
    if(!ready)
        return;

    fp.write(text.toAscii());
}
