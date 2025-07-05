include_HEADERS = tangram_types.h tangram_macros.h user_code.h

install-data-hook: 
	$(INSTALL_DATA) $(srcdir)/Makefile.mark_find $(DESTDIR)/$(prefix)/lib/Makefile.mark_find

uninstall-local: 
	rm -f $(DESTDIR)/$(prefix)/lib/Makefile.mark_find
