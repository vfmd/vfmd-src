TARGET = vfmd-example
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
include(../src/vfmd.pri)

SOURCES += main.cpp
