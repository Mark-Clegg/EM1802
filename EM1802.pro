QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    console.cpp \
    externalflag.cpp \
    indicatorled.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_assembler.cpp \
    memory.cpp \
    processor.cpp \
    processor_bcd.cpp \
    processor_disassembler.cpp \
    register.cpp \
    uart.cpp

HEADERS += \
    console.h \
    externalflag.h \
    indicatorled.h \
    mainwindow.h \
    memory.h \
    processor.h \
    register.h \
    uart.h

FORMS += \
    console.ui \
    mainwindow.ui \
    memory.ui \
    processor.ui \
    uart.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

DISTFILES += \
    LICENSE \
    README.md \
    idiot4.hex
