TARGET = clock
TEMPLATE = app
CONFIG += c++17

QT += core gui

# Разделение по версиям
equals(QT_MAJOR_VERSION, 5) {
    QT += widgets
    # Включаем поддержку High DPI для Qt 5
    CONFIG += hi-res
}

equals(QT_MAJOR_VERSION, 6) {
    QT += widgets
    # В Qt 6 некоторые модули требуют явного подключения Core5Compat
    # QT += core5compat 
}

INCLUDEPATH += include/

SOURCES += \
    src/main.cpp \
    src/window.cpp

HEADERS += \
    include/window.h

MOC_DIR = build_tmp/moc
OBJECTS_DIR = build_tmp/obj
RCC_DIR = build_tmp/rcc
UI_DIR = build_tmp/ui
DESTDIR = .
