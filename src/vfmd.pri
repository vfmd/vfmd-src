
CONFIG += c++11

SOURCES += $$PWD/vfmdbytearray.cpp \
           $$PWD/vfmdline.cpp \
           $$PWD/vfmdspantagstack.cpp \
           $$PWD/vfmdlinearray.cpp \
           $$PWD/vfmdlinearrayiterator.cpp \
           $$PWD/vfmddocument.cpp \
           $$PWD/vfmdelementregistry.cpp \
           $$PWD/vfmdinputlinesequence.cpp \
           $$PWD/vfmdregexp.cpp \
           $$PWD/core/vfmdpreprocessor.cpp \
           $$PWD/core/vfmdspanelementsprocessor.cpp \
           $$PWD/core/vfmddefaultelementregistry.cpp

HEADERS += $$PWD/vfmdbytearray.h \
           $$PWD/vfmdline.h \
           $$PWD/vfmdspantagstack.h \
           $$PWD/vfmdlinearray.h \
           $$PWD/vfmdlinearrayiterator.h \
           $$PWD/vfmddocument.h \
           $$PWD/vfmdelementregistry.h \
           $$PWD/vfmdinputlinesequence.h \
           $$PWD/vfmdregexp.h \
           $$PWD/vfmdconstants.h \
           $$PWD/core/vfmdpreprocessor.h \
           $$PWD/core/vfmdspanelementsprocessor.h \
           $$PWD/core/vfmdpointerarray.h

BLK = $$PWD/blockelements

SOURCES += $$BLK/vfmdblockelementhandler.cpp \
           $$BLK/paragraphhandler.cpp \
           $$BLK/blockquotehandler.cpp

HEADERS += $$BLK/vfmdblockelementhandler.h \
           $$BLK/paragraphhandler.h \
           $$BLK/blockquotehandler.h

SPN = $$PWD/spanelements

SOURCES += $$SPN/vfmdspanelementhandler.cpp \
           $$SPN/emphasishandler.cpp

HEADERS += $$SPN/vfmdspanelementhandler.h \
           $$SPN/emphasishandler.h

INCLUDEPATH += $$PWD $$PWD/core

DEFINES += VFMD_DEBUG
