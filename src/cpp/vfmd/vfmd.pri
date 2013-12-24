
SOURCES += $$PWD/vfmdpreprocessor.cpp \
           $$PWD/vfmdbytearray.cpp \
           $$PWD/vfmdinputlinesequence.cpp \
           $$PWD/vfmdline.cpp \
           $$PWD/vfmddocument.cpp

HEADERS += $$PWD/vfmdpreprocessor.h \
           $$PWD/vfmdbytearray.h \
           $$PWD/vfmdinputlinesequence.h \
           $$PWD/vfmdline.h \
           $$PWD/vfmddocument.h

BLK = $$PWD/blockelements

SOURCES += $$BLK/vfmdblockelement.cpp \
           $$BLK/paragraph.cpp \
           $$BLK/blockquote.cpp

HEADERS += $$BLK/vfmdblockelement.h \
           $$BLK/paragraph.h \
           $$BLK/blockquote.h

INCLUDEPATH += $$PWD
