QT += widgets serialport
QT += printsupport
requires(qtConfig(combobox))
CONFIG += static
TARGET = terminal
TEMPLATE = app

SOURCES += \
    ../serialframedecoder.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    stepperparameters.cpp

HEADERS += \
    ../serialframedecoder.h \
    mainwindow.h \
    settingsdialog.h \
    stepperparameters.h

INCLUDEPATH +=../
FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    stepperparameters.ui

RESOURCES += \
    resources.qrc


