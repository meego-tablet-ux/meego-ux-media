TEMPLATE = lib
CONFIG += qt plugin meegotouch
TARGET  = $$qtLibraryTarget(awd_plugin)
TARGETPATH = AcerWidgetsDaemonInterface
PLUGIN_TYPE = declarative

QT += declarative

OBJECTS_DIR = .obj
MOC_DIR = .moc

QML_FILES = *.qml *.js

HEADERS += \
    ../lib/awdaddress.h

SOURCES += \
    awdplugin.cpp \
    ../lib/awdaddress.cpp

OTHER_FILES += $${QML_FILES}

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qml.files += $${QML_FILES}
qml.path += $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qml qmldir target
