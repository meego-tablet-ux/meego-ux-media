TEMPLATE = lib
CONFIG += qt meegotouch
TARGET = awdapi
TARGET = $$qtLibraryTarget($$TARGET)

DEPENDPATH += .
INCLUDEPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

INSTALL_HEADERS += \
    awdaddress.h

HEADERS += \
    $$INSTALL_HEADERS

SOURCES += \
    awdaddress.cpp

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

headers.files += $$INSTALL_HEADERS
headers.path += $$INSTALL_ROOT/usr/include/awdapi/
INSTALLS += headers

pkgconfig.files += awdapi.pc
pkgconfig.path += $$[QT_INSTALL_LIBS]/pkgconfig
INSTALLS += pkgconfig

PKG_NAME = awdapi
