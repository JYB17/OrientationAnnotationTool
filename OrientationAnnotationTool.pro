QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataLoader.cpp \
    FrontView.cpp \
    LabelManager.cpp \
    OrientationGraphicsItem.cpp \
    TwoWheelerGraphicsItem.cpp \
    json/json.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DataLoader.h \
    FrontView.h \
    InfoStructure.h \
    LabelManager.h \
    OrientationGraphicsItem.h \
    TwoWheelerGraphicsItem.h \
    json/json.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += ..\3rdparty\opencv-3.4.0\include

LIBS += -L..\3rdparty\opencv-3.4.0\x64\vc15\lib \
    -lopencv_world340 \
    -lopencv_world340d

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
