TARGET = vfmd
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
include(../src/vfmd.pri)
include(../3rdparty/pcre.pri)
include(../3rdparty/redblacktree.pri)
include(../3rdparty/streamhtmlparser.pri)

SOURCES += main.cpp
