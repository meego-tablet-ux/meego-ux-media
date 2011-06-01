include(common.pri)
CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS += lib libqml Media content awdapi

QML_FILES = *.qml

OTHER_FILES = \
    $${QML_FILES}

TRANSLATIONS += $${OTHER_FILES}

dist.commands += rm -fR $${DISTSUBDIR} &&
dist.commands += git clone . $${DISTSUBDIR} &&
dist.commands += rm -fR $${DISTSUBDIR}/.git &&
dist.commands += mkdir -p $${DISTSUBDIR}/ts &&
dist.commands += cd lib && make dist && cd .. &&
dist.commands += cd Media && make dist && cd .. &&
dist.commands += tar jcpvf $${DISTSUBDIR}.tar.bz2 $${DISTSUBDIR} &&
dist.commands += rm -fR $${DISTSUBDIR} &&
dist.commands += echo && echo Created $${DISTSUBDIR}.tar.bz2
QMAKE_EXTRA_TARGETS += dist
