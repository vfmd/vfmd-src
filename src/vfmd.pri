
CONFIG += c++11

SOURCES += $$PWD/vfmdbytearray.cpp \
           $$PWD/vfmdline.cpp \
           $$PWD/vfmdspantagstack.cpp \
           $$PWD/vfmddocument.cpp \
           $$PWD/vfmdelementregistry.cpp \
           $$PWD/vfmddefaultelementregistry.cpp \
           $$PWD/vfmdinputlinesequence.cpp \
           $$PWD/vfmdblockelementhandler.cpp \
           $$PWD/vfmdspanelementhandler.cpp \
           $$PWD/vfmdregexp.cpp \
           $$PWD/vfmdelementtreenode.cpp \
           $$PWD/textspantreenode.cpp \
           $$PWD/vfmdoutputdevice.cpp \
           $$PWD/vfmdelementtreenodestack.cpp \
           $$PWD/core/vfmdpreprocessor.cpp \
           $$PWD/core/scanline.c \
           $$PWD/core/vfmdspanelementsprocessor.cpp \
           $$PWD/core/vfmdcommonregexps.cpp \
           $$PWD/core/vfmdcodespanfilter.cpp \
           $$PWD/core/htmlstatewatcher.cpp \
           $$PWD/core/vfmdlinkrefmap.cpp \
           $$PWD/core/htmltextrenderer.cpp \
           $$PWD/core/vfmdutils.cpp

HEADERS += $$PWD/vfmdbytearray.h \
           $$PWD/vfmdline.h \
           $$PWD/vfmdspantagstack.h \
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
           $$PWD/core/vfmdcommonregexps.h \
           $$PWD/core/vfmdcodespanfilter.h \
           $$PWD/core/htmlstatewatcher.h \
           $$PWD/core/vfmdlinkrefmap.h \
           $$PWD/core/htmltextrenderer.h \
           $$PWD/core/vfmdutils.h \
           $$PWD/core/vfmdlinkandimageutils.h

BLK = $$PWD/blockelements

SOURCES += \
           $$BLK/nullblockhandler.cpp \
           $$BLK/refresolutionblockhandler.cpp \
           $$BLK/setextheaderhandler.cpp \
           $$BLK/codeblockhandler.cpp \
           $$BLK/atxheaderhandler.cpp \
           $$BLK/blockquotehandler.cpp \
           $$BLK/horizontalrulehandler.cpp \
           $$BLK/unorderedlisthandler.cpp \
           $$BLK/orderedlisthandler.cpp \
           $$BLK/paragraphhandler.cpp

HEADERS += \
           $$BLK/nullblockhandler.h \
           $$BLK/setextheaderhandler.h \
           $$BLK/refresolutionblockhandler.h \
           $$BLK/codeblockhandler.h \
           $$BLK/atxheaderhandler.h \
           $$BLK/blockquotehandler.h \
           $$BLK/horizontalrulehandler.h \
           $$BLK/unorderedlisthandler.h \
           $$BLK/orderedlisthandler.h \
           $$BLK/paragraphhandler.h

SPN = $$PWD/spanelements

SOURCES += \
           $$SPN/linkhandler.cpp \
           $$SPN/emphasishandler.cpp \
           $$SPN/codespanhandler.cpp \
           $$SPN/imagehandler.cpp \
           $$SPN/htmltaghandler.cpp

HEADERS += \
           $$SPN/linkhandler.h \
           $$SPN/emphasishandler.h \
           $$SPN/codespanhandler.h \
           $$SPN/imagehandler.h \
           $$SPN/htmltaghandler.h

INCLUDEPATH += $$PWD $$PWD/core

contains(CONFIG, debug) {
    DEFINES += VFMD_DEBUG
}

contains(CONFIG, release) {
    DEFINES += NDEBUG
}
