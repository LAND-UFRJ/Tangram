INSTALL_DIR = $(DESTDIR)/$(prefix)/examples

# Creating dirs with examples
install-data-local:
	mkdir -p $(INSTALL_DIR);
	for dir in `ls $(srcdir)`; do \
        if test -d $(srcdir)/$$dir; then \
            mkdir -p $(INSTALL_DIR)/$$dir; \
            for example_dir in `ls $(srcdir)/$$dir`; do \
                example_path=$$dir/$$example_dir; \
                if test -d $(srcdir)/$$example_path; then \
                    mkdir -p $(INSTALL_DIR)/$$example_path; \
                    for example_file in `ls $(srcdir)/$$example_path`; do \
                        if test ! -d $(srcdir)/$$example_path/$$example_file; then \
                            $(INSTALL_DATA) $(srcdir)/$$example_path/$$example_file $(INSTALL_DIR)/$$example_path/; \
                        fi \
                    done \
                fi \
            done \
        fi \
    done

uninstall-local:
	rm -rf $(INSTALL_DIR)

EXTRA_DIST = ModelEnv

