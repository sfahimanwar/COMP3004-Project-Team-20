QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aed.cpp \
    main.cpp \
    mainwindow.cpp \
    patient.cpp

HEADERS += \
    aed.h \
    mainwindow.h \
    patient.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Selection_003.png \
    Selection_005.png \
    Selection_006.png \
    Selection_007.png \
    Selection_008.png \
    Selection_009.png \
    Selection_010.png \
    Selection_011.png

RESOURCES += \
    images.qrc
