#include <QtDeclarative>

#include "../lib/awdaddress.h"

QT_BEGIN_NAMESPACE

class AcerWidgetsDaemonDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        qmlRegisterType<AwdAddress>(uri, 0, 1, "AwdAddress");
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri)
    {
        Q_UNUSED(uri);
        Q_UNUSED(engine);
    }
};

QT_END_NAMESPACE

#include "awdplugin.moc"

Q_EXPORT_PLUGIN2(acerwidgetsdaemondeclarativemodule, QT_PREPEND_NAMESPACE(AcerWidgetsDaemonDeclarativeModule));

