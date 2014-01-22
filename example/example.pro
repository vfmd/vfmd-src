TARGET = vfmd-example
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
include(../src/vfmd.pri)
include(../3rdparty/pcre.pri)

SOURCES += main.cpp
