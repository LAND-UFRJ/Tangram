MTK_HEADER = core_controller.h \
	     object.h          \
	     object_manager.h  \
	     plugin.h          \
	     plugin_manager.h  \
	     types.h           \
	     terminal.h        \
	     token.h           \
	     prototype.h       \
         signal_manager.h

MTK_HEADER_DIR = $(DESTDIR)/$(prefix)/include/mtk/

lib_LTLIBRARIES = libmtk.la

SOURCES = $(MTK_HEADER) core_controller.cpp \
          object.cpp          \
          object_manager.cpp  \
          plugin.cpp          \
          plugin_manager.cpp  \
          prototype.cpp       \
          signal_manager.cpp  \
          terminal.cpp        \
          token.cpp

libmtk_la_SOURCES = $(SOURCES)

libmtk_la_CPPFLAGS = -fPIC -shared

install-data-hook:
	mkdir -p $(MTK_HEADER_DIR)
	$(INSTALL_DATA) $(MTK_HEADER) $(MTK_HEADER_DIR)
