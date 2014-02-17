
CONFIG += c++11

SOURCES += $$PWD/vfmdbytearray.cpp \
           $$PWD/vfmdline.cpp \
           $$PWD/vfmdspantagstack.cpp \
           $$PWD/vfmdlinearray.cpp \
           $$PWD/vfmdlinearrayiterator.cpp \
           $$PWD/vfmddocument.cpp \
           $$PWD/vfmdelementregistry.cpp \
           $$PWD/vfmdinputlinesequence.cpp \
           $$PWD/vfmdblockelementhandler.cpp \
           $$PWD/vfmdspanelementhandler.cpp \
           $$PWD/vfmdregexp.cpp \
           $$PWD/vfmdelementtreenode.cpp \
           $$PWD/textspantreenode.cpp \
           $$PWD/vfmdoutputdevice.cpp \
           $$PWD/vfmdelementtreenodestack.cpp \
           $$PWD/core/vfmdpreprocessor.cpp \
           $$PWD/core/vfmdspanelementsprocessor.cpp \
           $$PWD/core/vfmddefaultelementregistry.cpp \
           $$PWD/core/vfmdcommonregexps.cpp

HEADERS += $$PWD/vfmdbytearray.h \
           $$PWD/vfmdline.h \
           $$PWD/vfmdspantagstack.h \
           $$PWD/vfmdlinearray.h \
           $$PWD/vfmdlinearrayiterator.h \
           $$PWD/vfmddocument.h \
           $$PWD/vfmdelementregistry.h \
           $$PWD/vfmdinputlinesequence.h \
           $$PWD/vfmdblockelementhandler.h \
           $$PWD/vfmdspanelementhandler.h \
           $$PWD/vfmdregexp.h \
           $$PWD/vfmdconstants.h \
           $$PWD/vfmdelementtreenode.h \
           $$PWD/textspantreenode.h \
           $$PWD/vfmdoutputdevice.h \
           $$PWD/vfmdelementtreenodestack.h \
           $$PWD/vfmdunicodeproperties.h \
           $$PWD/core/vfmdpreprocessor.h \
           $$PWD/core/vfmdspanelementsprocessor.h \
           $$PWD/core/vfmdpointerarray.h \
           $$PWD/core/vfmdscopedpointer.h \
           $$PWD/core/vfmddictionary.h \
           $$PWD/core/vfmdcommonregexps.h

BLK = $$PWD/blockelements

SOURCES += \
           $$BLK/refresolutionblockhandler.cpp \
           $$BLK/setextheaderhandler.cpp \
           $$BLK/blockquotehandler.cpp \
           $$BLK/paragraphhandler.cpp

HEADERS += \
           $$BLK/nullblockhandler.h \
           $$BLK/setextheaderhandler.h \
           $$BLK/refresolutionblockhandler.h \
           $$BLK/blockquotehandler.h \
           $$BLK/paragraphhandler.h

SPN = $$PWD/spanelements

SOURCES += \
           $$SPN/emphasishandler.cpp

HEADERS += \
           $$SPN/emphasishandler.h

INCLUDEPATH += $$PWD $$PWD/core

DEFINES += VFMD_DEBUG
