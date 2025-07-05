bin_PROGRAMS = mtx_vis-st_permut

mtx_vis_st_permut_SOURCES = matrix.c

mtx_vis_st_permut_CPPFLAGS = -Wall -O -g -I$(top_srcdir)/ModelEnv/libMatrix

mtx_vis_st_permut_LDADD = -L$(top_srcdir)/ModelEnv/libMatrix -lMatrix $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)

EXTRA_DIST = stack.c llist.c MtxVisStPermut

install-data-local:
	$(INSTALL_DATA) $(srcdir)/MtxVisStPermut $(DESTDIR)/$(prefix)/bin/

uninstall-local: 
	rm -f $(prefix)/bin/MtxVisStPermut
