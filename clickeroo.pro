CONFIG += c++17
QT += core gui widgets
TEMPLATE = app

win32: LIBS += -luser32 # For global hotkey support on Windows

LIBS += -L"C:\\Users\\User\\AppData\\Local\\Programs\\Python\\Python310\libs" -lpython310
INCLUDEPATH += "C:\\Users\\User\\AppData\\Local\\Programs\\Python\\Python310\include"
DEPENDPATH += "C:\\Users\\User\\AppData\\Local\\Programs\\Python\\Python310\include"

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # Disables all the APIs deprecated before Qt 6.0.0

include($$PWD/src/sources.pri)
