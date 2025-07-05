# libMatrix
LIB_MATRIX_INC   = $(top_srcdir)/ModelEnv/libMatrix
LIB_MATRIX_DIR   = ${LIB_MATRIX_INC}/.libs
LIB_MATRIX_FLAGS = -lMatrix -L${LIB_MATRIX_DIR}

# libTangramMTK
LIB_TANGRAM_MTK_INC   = ../libTangramMTK
LIB_TANGRAM_MTK_DIR   = ${LIB_TANGRAM_MTK_INC}/.libs
LIB_TANGRAM_MTK_FLAGS = -lTangramMTK -L${LIB_TANGRAM_MTK_DIR}

AM_LDFLAGS = ${LIB_MATRIX_FLAGS} ${LIB_TANGRAM_MTK_FLAGS} ${LDFLAGS} 

INCLUDES = -I../include -I../../libmtk -I. ${CFLAGS}

AM_CXXFLAGS = -Wall -g -DDEBUG_LEVEL=2 ${INCLUDES}

INSTALL_DIR = $(DESTDIR)/$(prefix)/lib/mtk_plugins

install-data-hook:
	mkdir -p $(INSTALL_DIR)
	cp -a $(PLUGIN_FILES) $(INSTALL_DIR)/

uninstall-local:
	rm -rf $(INSTALL_DIR)/$(PLUGIN_FILES)
