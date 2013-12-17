TARGET = vfmd
OBJECTS_DIR = .obj
mac: CONFIG -= app_bundle
SOURCES += main.cpp \
           VfmdPreprocessor.cpp
HEADERS += VfmdPreprocessor.h
INCLUDEPATH += .

