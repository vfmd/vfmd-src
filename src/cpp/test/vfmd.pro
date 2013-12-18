TARGET = vfmd
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
SOURCES += main.cpp \
           ../vfmd/vfmdpreprocessor.cpp
HEADERS += ../vfmd/vfmdpreprocessor.h
INCLUDEPATH += ../vfmd

