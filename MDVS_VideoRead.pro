TARGET = MDVS_VideoRead
TEMPLATE = app

CONFIG =  warn_off release

QMAKE_CXXFLAGS += -O3

MOC_DIR = .moc
OBJECTS_DIR = .obj


CONFIG += qt
#QMAKE_CXXFLAGS = -std = c++11

INCLUDEPATH += ./sdk/include/
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += ./src


LIBS += ../sdk/libMVSDK.so
LIBS +=
LIBS += /usr/local/lib/libopencv_highgui.so.2.4.13
LIBS += /usr/local/lib/libopencv_imgproc.so.2.4.13
LIBS += /usr/local/lib/libopencv_core.so.2.4.13

SOURCES += ./src/detect.cpp \
main.cpp \
#            test.cpp \
#        videocapture.cpp \

HEADERS += ./sdk/include/CameraApi.h \
           ./src/detect.h \
            #videocapture.h
