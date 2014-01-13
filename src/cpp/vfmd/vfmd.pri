
CONFIG += c++11

SOURCES += $$PWD/vfmdpreprocessor.cpp \
           $$PWD/vfmdbytearray.cpp \
           $$PWD/vfmdinputlinesequence.cpp \
           $$PWD/vfmdline.cpp \
           $$PWD/vfmddocument.cpp \
           $$PWD/vfmdelementregistry.cpp \
           $$PWD/vfmdspantagstack.cpp \
           $$PWD/vfmdlinearray.cpp \
           $$PWD/vfmdlinearrayiterator.cpp \
           $$PWD/vfmdspanelementsprocessor.cpp \
           $$PWD/VfmdDefaultElementRegistry.cpp

HEADERS += $$PWD/vfmdpreprocessor.h \
           $$PWD/vfmdbytearray.h \
           $$PWD/vfmdinputlinesequence.h \
           $$PWD/vfmdline.h \
           $$PWD/vfmddocument.h \
           $$PWD/vfmdpointerarray.h \
           $$PWD/vfmdelementregistry.h \
           $$PWD/vfmdspantagstack.h \
           $$PWD/vfmdlinearray.h \
           $$PWD/vfmdlinearrayiterator.h \
           $$PWD/vfmdspanelementsprocessor.h \
           $$PWD/vfmdconstants.h

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

INCLUDEPATH += $$PWD
