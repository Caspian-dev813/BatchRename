QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    file_scanner.cpp \
    name_generator.cpp \
    backup_manager.cpp \
    rename_engine.cpp

HEADERS += \
    mainwindow.h \
    models.h \
    file_scanner.h \
    name_generator.h \
    backup_manager.h \
    rename_engine.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
