#include <QtDeclarative>

#include "../lib/meegomediawidget.h"

QT_BEGIN_NAMESPACE

class MediaWidgetInterfaceDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        qmlRegisterType<WidgetAddress>(uri, 0, 1, "WidgetAddress");
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri)
    {
        Q_UNUSED(uri);
        Q_UNUSED(engine);
    }
};

QT_END_NAMESPACE

#include "widgetclientqml.moc"

Q_EXPORT_PLUGIN2(mediawidgetinterfacedeclarativemodule, QT_PREPEND_NAMESPACE(MediaWidgetInterfaceDeclarativeModule));

