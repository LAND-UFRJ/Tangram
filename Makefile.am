SUBDIRS = ModelEnv TraffGen interface scripts etc examples Domain

EXTRA_DIST = KNOWN_BUGS FAQ LICENSE.QPL bin-installer testbed doc

uninstall-local: 
	rm -rf $(DESTDIR)/$(prefix)

install-exec-local:
	cp -a testbed $(DESTDIR)/$(prefix)/
	@echo "============================================================="
	@echo "ATTENTION: "
	@echo "         1) Do not forget to set your TANGRAM2_HOME"
	@echo "           environment variable to $(prefix)"
	@echo "           * If you are using bash, edit your "
	@echo "             $(HOME)/.bashrc and add the line:"
	@echo "             export TANGRAM2_HOME=$(prefix)"    
	@echo "           * If you are using csh, edit your "
	@echo "             $(HOME)/.cshrc and add the line:"
	@echo "             setenv TANGRAM2_HOME $(prefix)"
	@echo "         2) Do not forget to add \$$TANGRAM2_HOME/bin"
	@echo "           to your PATH environment variable"
	@echo "           * If you are using bash, edit your "
	@echo "             $(HOME)/.bashrc and add the line:"
	@echo "             export PATH=\$$PATH:\$$TANGRAM2_HOME/bin"    
	@echo "           * If you are using csh, edit your "
	@echo "             $(HOME)/.cshrc and add the line:"
	@echo "             setenv PATH \$$PATH:\$$TANGRAM2_HOME/bin"
	@echo "============================================================="

# Bin distribution
TANGRAM2_INST_SCRIPT=tangram2-$(PACKAGE_VERSION)_installer.sh
TANGRAM2_INST_HEADER=bin-installer/script_tangram2_install.sh

TANGRAM2_BIN=Tangram2

dist-bin: all
	@make install DESTDIR=$(PWD)/bin-installer
	@echo "-----------------------------------------------------------"
	@echo "Creating package with TANGRAM-II binaries..."
	@tar czf Tangram2_Temp.tar.gz -C $(PWD)/bin-installer/usr/local/ $(TANGRAM2_FILES)
	@cat $(TANGRAM2_INST_HEADER) Tangram2_Temp.tar.gz > $(TANGRAM2_INST_SCRIPT)
	@chmod +x $(TANGRAM2_INST_SCRIPT)
	@echo "Cleaning temporary files..."
	@rm -rf Tangram2_Temp.tar.gz
	@rm -rf $(PWD)/bin-installer/usr/
	@echo "-----------------------------------------------------------"
	@echo "Distribution $(TANGRAM2_INST_SCRIPT) was generated"
	@echo "-----------------------------------------------------------"
	@echo

# Preparing to install
install-data-hook:
	rm -rf `find $(DESTDIR)/$(prefix) -name CVS`

# Preparing to distribute
dist-hook:
	rm -rf `find $(distdir) -name CVS`
	rm -f `find $(distdir) -name gramatica.lex.c`
	rm -f `find $(distdir) -name gramatica.bison.c`
	rm -f `find $(distdir) -name gramatica.bison.h`
