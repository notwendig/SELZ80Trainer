#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T19:23:27
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = SELZ80Trainer
QMAKE_EXTRA_TARGETS += SELROM
TEMPLATE = app

PRE_TARGETDEPS += ../../ROMS/SELRom.bin #../../ROMS/camel80.bin

QMAKE_CXXFLAGS +=-std=c++2a -I../../../qvintage
QMAKE_CXXFLAGS_WARN_ON+=-Wno-unused-parameter -Wno-unused-const-variable
QMAKE_LFLAGS += -L ~/lib/qt6/designer -L ~/lib

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/SELZ80Trainer.cpp \
        src/Manual.cpp

HEADERS  += src/mainwindow.h \
        src/SELZ80Trainer.h \
        ROMS/SELRom.h \
        src/Manual.h

FORMS    += \
        src/mainwindow.ui \
        src/Manual.ui

unix: LIBS +=-L~/lib -lZ80 -lpoppler-qt6 -L~/lib/qt6/designer -ldevicesplugin

INCLUDEPATH += $$PWD/../qviPWntage/LED \
                $$PWD/../qvintage/SWITCH \
                $$PWD/../qvintage/SWITCH2 \
                $$PWD/../qvintage/SEG7 $$PWD/ \
                $$PWD/../Z80/src \
                $$PWD/ROMS

DEPENDPATH += /home/juergen/lib/qt6/designer

OTHER_FILES += \
        ROMS/SELRom.z80


all: $(TARGET)


SELROM.target   = ../../Resources/SELRom.bin
SELROM.depends  = ../../ROMS/SELRom.z80
SELROM.commands = zmac -czfm $$SELROM.depends;\
        echo "SELNOTUSED  equ  4000h" >  $$replace(SELROM.target,".bin",".inc") ;\
        echo "SELRAMSTART equ  1000h" >> $$replace(SELROM.target,".bin",".inc") ;\
        echo "SELRAMEND   equ 0FFFFh" >> $$replace(SELROM.target,".bin",".inc") ;\
        cat $$SELROM.depends | sed -ne $$shell_quote('s/^\\([^ \t]\+\W\+equ\W\+\\)/\1/p') >> $$replace(SELROM.target,".bin",".inc") ;\
        cat zout/SELRom.lst | sed -ne $$shell_quote('s/^\\([^ \t]\+\\)\W\+\\([0-9A-Fa-f]\+\\)\W\+(public)/\1\tequ\t0\2h/p') >> $$replace(SELROM.target,".bin",".inc") ;\
        cp zout/SELRom.cim $$SELROM.target

QMAKE_CLEAN    += $$SELROM.target $$replace(SELROM.target,".bin",".inc")

CAMELFORTH.target	= ../../Resources/camel80.bin
CAMELFORTH.depends	= ../../ROMS/camel80.z80 $$SELROM.depends
CAMELFORTH.commands= zmac -czfm --zmac $$first(CAMELFORTH.depends) ;\
   cp zout/camel80.cim $$CAMELFORTH.target

QMAKE_CLEAN += $$CAMELFORTH.target

RESOURCES += \
        Resources/resources.qrc

DISTFILES += \
        camel80.z80 \
        camel80d.azm \
        camel80h.azm \
        SELRom.inc \
        camelSEL.azm \
        SELRom.z80

unix {
        target.path = $(HOME)/bin
        INSTALLS += target
}

DESTDIR =~/bin
