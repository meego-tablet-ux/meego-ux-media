#include "debugtofile.h"

DebugToFile::DebugToFile(QObject *parent) :
    QObject(parent)
{
    ready = false;
}

bool DebugToFile::fileReady()
{
    if(ready)
        return true;

    if(m_file.isEmpty())
        return false;

    QString filePath = QDir::toNativeSeparators(QDir::homePath()) + QDir::separator() + m_file;
    fp.setFileName(filePath);
    ready = true;
}

void DebugToFile::print(const QString &text)
{
    if(!fileReady())
        return;

    fp.write(text.toAscii());
}
