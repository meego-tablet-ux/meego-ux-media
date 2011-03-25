include(../common.pri)
TARGET = mediaplugin
TEMPLATE = lib

#QT += dbus network

#CONFIG += plugin link_pkgconfig dbus
CONFIG += plugin link_pkgconfig

PKGCONFIG += meego-ux-content meego-ux-media

# use pkg-config paths for include in both g++ and moc
INCLUDEPATH += $$system(pkg-config --cflags meego-ux-content \
    | tr \' \' \'\\n\' | grep ^-I | cut -d 'I' -f 2-)
INCLUDEPATH += ../lib

# this is just for building within the tree
LIBS += -L../lib -lmeegouxmedia

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    mediafeedmodel.cpp \
    mediaplugin.cpp \
    mediaservicemodel.cpp

HEADERS += \
    mediafeedmodel.h \
    mediaplugin.h \
    mediaservicemodel.h

target.path = $$[QT_INSTALL_PLUGINS]/MeeGo/Content
INSTALLS += target
