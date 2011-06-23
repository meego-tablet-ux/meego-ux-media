TEMPLATE = lib
CONFIG += qt plugin meegotouch
TARGET  = $$qtLibraryTarget(WidgetInterface)
TARGETPATH = MeeGo/WidgetInterface
PLUGIN_TYPE = declarative

QT += declarative

OBJECTS_DIR = .obj
MOC_DIR = .moc

QML_FILES = *.qml *.js

HEADERS += \
    ../lib/meegomediawidget.h

SOURCES += \
    widgetclientqml.cpp \
    ../lib/meegomediawidget.cpp

OTHER_FILES += $${QML_FILES}

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qml.files += $${QML_FILES}
qml.path += $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qml qmldir target
