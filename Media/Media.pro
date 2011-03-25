include(../common.pri)
TEMPLATE = subdirs

QML_FILES = *.qml

OTHER_FILES += $${QML_FILES}

qml.files += $${QML_FILES}
qml.path += $$[QT_INSTALL_IMPORTS]/MeeGo/Media
INSTALLS += qml

PKG_NAME = meego-ux-media-qml
TRANSLATIONS += $${QML_FILES}

dist.commands += lupdate $${TRANSLATIONS} -ts ../$${TSSUBDIR}/$${PKG_NAME}.ts
QMAKE_EXTRA_TARGETS += dist
