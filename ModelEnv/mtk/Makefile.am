# libMatrix
LIB_MATRIX_DIR   = $(top_srcdir)/ModelEnv/libMatrix
LIB_MATRIX_FLAGS = -lMatrix -L${LIB_MATRIX_DIR}

# libTangramMTK
LIB_TANGRAM_MTK_DIR   = plugins/libTangramMTK
LIB_TANGRAM_MTK_FLAGS = -lTangramMTK -L${LIB_TANGRAM_MTK_DIR}

SUBDIRS = libmtk plugins

bin_PROGRAMS = mtk

mtk_HEADER =  action.h         \
              command.h        \
              core_interface.h \
              user_interface.h \
              help_messages.h

mtk_SOURCES = $(mtk_HEADER) action.cpp \
              command.cpp              \
              core_interface.cpp       \
              mtk.cpp                  \
              user_interface.cpp

              
mtk_CPPFLAGS =  -Ilibmtk -I.
#mtk_CPPFLAGS +=  -g -DDEBUG_LEVEL=2

mtk_LDFLAGS = -rdynamic -ldl -lreadline -lcurses \
              -lmtk -Llibmtk                     \
              ${LIB_MATRIX_FLAGS}                \
              ${LIB_TANGRAM_MTK_FLAGS}

if HAVE_MTKGUI
    SUBDIRS      += gui
    mtk_LDADD     = gui/libmtkgui.a
    mtk_CPPFLAGS += -DMTKGUI -I$(QTDIR)/include -Igui
    mtk_LDFLAGS  += -lqt-mt -L$(QTDIR)/lib
endif
