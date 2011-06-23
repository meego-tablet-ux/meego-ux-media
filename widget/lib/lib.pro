TEMPLATE = lib
CONFIG += qt meegotouch
TARGET = meegomediawidget
TARGET = $$qtLibraryTarget($$TARGET)

DEPENDPATH += .
INCLUDEPATH += .

OBJECTS_DIR = .obj
MOC_DIR = .moc

INSTALL_HEADERS += \
    meegomediawidget.h

HEADERS += \
    $$INSTALL_HEADERS

SOURCES += \
    meegomediawidget.cpp

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

headers.files += $$INSTALL_HEADERS
headers.path += $$INSTALL_ROOT/usr/include/meegomediawidget/
INSTALLS += headers

pkgconfig.files += meegomediawidget.pc
pkgconfig.path += $$[QT_INSTALL_LIBS]/pkgconfig
INSTALLS += pkgconfig

PKG_NAME = meegomediawidget
