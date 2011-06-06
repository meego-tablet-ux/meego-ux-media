TEMPLATE = lib
CONFIG += qt plugin meegotouch
TARGET  = $$qtLibraryTarget(awd_plugin)
TARGETPATH = AcerWidgetsDaemonInterface
PLUGIN_TYPE = declarative

QT += declarative

OBJECTS_DIR = .obj
MOC_DIR = .moc

HEADERS += \
    ../lib/awdaddress.h

SOURCES += \
    awdplugin.cpp \
    ../lib/awdaddress.cpp

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH
qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH
INSTALLS += qmldir target
