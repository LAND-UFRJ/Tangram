SUBDIRS = Tangram2_Interface libTangram Input_Parser

EXTRA_DIST = end_user_needed

# Creates $(prefix)/class and install .jar's
INSTALL_DIR = $(DESTDIR)/$(prefix)/class

install-data-hook:
	mkdir -p $(INSTALL_DIR)
	$(INSTALL_DATA) end_user_needed/*.jar $(INSTALL_DIR)/
	$(INSTALL_DATA) Tangram2_Interface/Tangram2.jar $(INSTALL_DIR)/Tangram2.jar

uninstall-local:
	rm -rf $(INSTALL_DIR)
