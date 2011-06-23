#include "meegomediawidget.h"

WidgetAddress::WidgetAddress(QObject *parent) :
    QObject(parent)
{
    m_localPort = "11111";
    m_uid = "";
    m_name = "";
    m_type = "";
    m_widgetId = "";
    m_sdm = "0";
}

void WidgetAddress::setUid(QString uid)
{
    if(uid != m_uid)
        m_uid = uid;
}

void WidgetAddress::setName(QString name)
{
    if(name != m_name)
        m_name = name;
}

void WidgetAddress::setType(QString type)
{
    if(type != m_type)
        m_type = type;
}

void WidgetAddress::setWidgetId(QString widgetId)
{
    if(widgetId != m_widgetId)
        m_widgetId = widgetId;
}

void WidgetAddress::setSendDataMode(QString sdm)
{
    if(sdm == "force")
        m_sdm = "1";
    else
        m_sdm = "0";
}

QString WidgetAddress::getAddress(QString command, QString data)
{
    if(m_uid != "")
    {
        if(m_widgetId == "")
            return "http://localhost:"+m_localPort+"/?ft="+m_sdm+"&name="+m_name+"&type="+m_type+"&uid="+m_uid+"&command="+command+"&data="+data;
        else
            return "http://localhost:"+m_localPort+"/?ft="+m_sdm+"&name="+m_name+"&type="+m_type+"&uid="+m_uid+"&command="+command+"&id="+m_widgetId+"&data="+data;
    }
    else
    {
        if(m_widgetId == "")
            return "http://localhost:"+m_localPort+"/?ft="+m_sdm+"&name="+m_name+"&type="+m_type+"&command="+command+"&data="+data;
        else
            return "http://localhost:"+m_localPort+"/?ft="+m_sdm+"&name="+m_name+"&type="+m_type+"&command="+command+"&id="+m_widgetId+"&data="+data;
    }
}
