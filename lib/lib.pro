include(../common.pri)
TEMPLATE = lib
TARGET = meegouxmedia

QT += declarative \
    network \
    dbus \
    sql
CONFIG += \
    qt \
    dbus \
    link_pkgconfig \
    mobility
PKGCONFIG += \
    gconf-2.0 \
    libresourceqt1 \
    mlite \
    libexif
MOBILITY = publishsubscribe
TARGET = $$qtLibraryTarget($$TARGET)

OBJECTS_DIR = .obj
MOC_DIR = .moc

# Input
SOURCES += \
    documentdatabase.cpp \
    documentlistmodel.cpp \
    mediadatabase.cpp \
    mediainfodownloader.cpp \
    mediaitem.cpp \
    medialistmodel.cpp \
    musicdatabase.cpp \
    musiclistmodel.cpp \
    photodatabase.cpp \
    photolistmodel.cpp \
    thumbnailer.cpp \
    videodatabase.cpp \
    videolistmodel.cpp \
    playlistimageprovider.cpp \
    resourcemanager.cpp

# these are the external interfaces exposed to clients in /usr/include
INSTALL_HEADERS += \
    mediaitem.h \
    documentlistmodel.h \
    medialistmodel.h \
    musiclistmodel.h \
    photolistmodel.h \
    videolistmodel.h \
    mediainfoplugininterface.h

# these are implementation details
HEADERS += \
    $$INSTALL_HEADERS \
    documentdatabase.h \
    mediadatabase.h \
    mediainfodownloader.h \
    musicdatabase.h \
    photodatabase.h \
    thumbnailer.h \
    videodatabase.h \
    mediainfoplugininterface.h \
    playlistimageprovider.h \
    resourcemanager.h

OTHER_FILES += \
    meego-ux-media.pc

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

headers.files += $$INSTALL_HEADERS
headers.path += $$INSTALL_ROOT/usr/include/meegouxmedia
INSTALLS += headers

pkgconfig.files += meego-ux-media.pc
pkgconfig.path += $$[QT_INSTALL_LIBS]/pkgconfig
INSTALLS += pkgconfig

PKG_NAME = meego-ux-media
TRANSLATIONS += $${HEADERS} $${SOURCES}
dist.commands += lupdate $${TRANSLATIONS} -ts ../$${TSSUBDIR}/$${PKG_NAME}.ts

QMAKE_EXTRA_TARGETS += dist
