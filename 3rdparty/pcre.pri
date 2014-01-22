
SOURCES += $$PWD/pcre/pcre_byte_order.c \
           $$PWD/pcre/pcre_chartables.c \
           $$PWD/pcre/pcre_compile.c \
           $$PWD/pcre/pcre_config.c \
           $$PWD/pcre/pcre_dfa_exec.c \
           $$PWD/pcre/pcre_exec.c \
           $$PWD/pcre/pcre_fullinfo.c \
           $$PWD/pcre/pcre_get.c \
           $$PWD/pcre/pcre_globals.c \
           $$PWD/pcre/pcre_jit_compile.c \
           $$PWD/pcre/pcre_maketables.c \
           $$PWD/pcre/pcre_newline.c \
           $$PWD/pcre/pcre_ord2utf8.c \
           $$PWD/pcre/pcre_refcount.c \
           $$PWD/pcre/pcre_string_utils.c \
           $$PWD/pcre/pcre_study.c \
           $$PWD/pcre/pcre_tables.c \
           $$PWD/pcre/pcre_ucd.c \
           $$PWD/pcre/pcre_valid_utf8.c \
           $$PWD/pcre/pcre_version.c \
           $$PWD/pcre/pcre_xclass.c

INCLUDEPATH += $$PWD/pcre

QMAKE_CFLAGS += -DHAVE_CONFIG_H

