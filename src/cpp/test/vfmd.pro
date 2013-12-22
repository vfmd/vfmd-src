TARGET = vfmd
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
include(../vfmd/vfmd.pri)

SOURCES += main.cpp
