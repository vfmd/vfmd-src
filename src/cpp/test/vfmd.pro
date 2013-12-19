TARGET = vfmd
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
SOURCES += main.cpp \
           ../vfmd/vfmdpreprocessor.cpp \
           ../vfmd/vfmdbytearray.cpp
HEADERS += ../vfmd/vfmdpreprocessor.h \
           ../vfmd/vfmdbytearray.h
INCLUDEPATH += ../vfmd

