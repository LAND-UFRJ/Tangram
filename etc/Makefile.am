XDEFAULTS1 = "Tgif*DomainPath1:   TANGRAM2_FLUID_OBJECTS:$(prefix)/Domain/TANGRAM2_FLUID_OBJECTS/:."
XDEFAULTS2 = "Tgif*DomainPath2:   TANGRAM2_OBJECTS:$(prefix)/Domain/TANGRAM2_OBJECTS/:."

INSTALL_DIR = $(DESTDIR)/$(prefix)/etc

install-data-local: font.properties tangram2.conf
	mkdir -p $(INSTALL_DIR)
	$(INSTALL_DATA) $(srcdir)/font.properties $(INSTALL_DIR)/font.properties
	$(INSTALL_DATA) $(srcdir)/tangram2.conf $(INSTALL_DIR)/tangram2.conf
	cat Xdefaults.base > Xdefaults
	echo $(XDEFAULTS1) >> Xdefaults
	echo $(XDEFAULTS2) >> Xdefaults
	$(INSTALL_DATA) $(srcdir)/Xdefaults $(INSTALL_DIR)/Xdefaults

EXTRA_DIST = font.properties tangram2.conf Xdefaults.base

CLEANFILES = Xdefaults
