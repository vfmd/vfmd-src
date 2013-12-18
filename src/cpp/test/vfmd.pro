TARGET = vfmd
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
SOURCES += main.cpp \
           ../vfmd/VfmdPreprocessor.cpp
HEADERS += ../vfmd/VfmdPreprocessor.h
INCLUDEPATH += ../vfmd

