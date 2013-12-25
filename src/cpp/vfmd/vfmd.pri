
CONFIG += c++11

SOURCES += $$PWD/vfmdpreprocessor.cpp \
           $$PWD/vfmdbytearray.cpp \
           $$PWD/vfmdinputlinesequence.cpp \
           $$PWD/vfmdline.cpp \
           $$PWD/vfmddocument.cpp \
           $$PWD/vfmdelementregistry.cpp \
           $$PWD/vfmdspantagstack.cpp

HEADERS += $$PWD/vfmdpreprocessor.h \
           $$PWD/vfmdbytearray.h \
           $$PWD/vfmdinputlinesequence.h \
           $$PWD/vfmdline.h \
           $$PWD/vfmddocument.h \
           $$PWD/vfmdpointerarray.h \
           $$PWD/vfmdelementregistry.h \
           $$PWD/vfmdspantagstack.h

BLK = $$PWD/blockelements

SOURCES += $$BLK/vfmdblockelement.cpp \
           $$BLK/paragraph.cpp \
           $$BLK/blockquote.cpp

HEADERS += $$BLK/vfmdblockelement.h \
           $$BLK/paragraph.h \
           $$BLK/blockquote.h

SPN = $$PWD/spanelements

SOURCES += $$SPN/vfmdspanelementhandler.cpp

HEADERS += $$SPN/vfmdspanelementhandler.h

INCLUDEPATH += $$PWD
