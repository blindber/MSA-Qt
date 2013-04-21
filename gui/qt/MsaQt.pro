#-------------------------------------------------
#
# Project created by QtCreator 2013-04-17T06:33:28
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = msa-qt
TEMPLATE = app

QMAKE_CFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-parameter

target.path = /home/pi/msa-qt
INSTALLS += target

INCLUDEPATH += /mnt/rasp-pi-rootfs/usr/local/qt5pi/include/

for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemsources = $${item}.sources
    $$itemsources = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath= $$eval($${deploymentfolder}.target)
    export($$itemsources)
    export($$itempath)
    DEPLOYMENT += $$item
}

installPrefix = /home/pi/$${TARGET}

for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemfiles = $${item}.files
    $$itemfiles = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath = $${installPrefix}/$$eval($${deploymentfolder}.target)
    export($$itemfiles)
    export($$itempath)
    INSTALLS += $$item
}


SOURCES += main.cpp\
        mainwindow.cpp \
    uwork.cpp \
    usbfunctions.cpp \
    touchstone.cpp \
    referencedialog.cpp \
    msautilities.cpp \
    interpolation.cpp \
    graph.cpp \
    globalvars.cpp \
    dialoggridappearance.cpp \
    dialogconfigman.cpp \
    dialogcalmanager.cpp \
    dialogoslcal.cpp \
    coaxanalysis.cpp \
    lptfunctions.cpp \
    dialogvnacal.cpp

HEADERS  += mainwindow.h \
    uwork.h \
    usbfunctions.h \
    touchstone.h \
    referencedialog.h \
    msautilities.h \
    interpolation.h \
    graph.h \
    globalvars.h \
    dialoggridappearance.h \
    dialogconfigman.h \
    dialogcalmanager.h \
    dialogoslcal.h \
    coaxanalysis.h \
    lptfunctions.h \
    dialogvnacal.h

FORMS    += mainwindow.ui \
    referencedialog.ui \
    dialoggridappearance.ui \
    dialogconfigman.ui \
    dialogcalmanager.ui \
    dialogoslcal.ui \
    dialogvnacal.ui

RESOURCES += \
    resource.qrc

win32:RC_FILE = resource.rc

OTHER_FILES += \
    resource.rc \
    readme.txt
