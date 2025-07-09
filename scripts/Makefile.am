# Scripts that need to be installed
bin_SCRIPTS = tangram2 traffgen generate_chain modelenv

EXTRA_DIST = tangram2.base traffgen generate_chain modelenv

DEFINES =
if HAVE_PVM
    DEFINES += -DHAVE_PVM 
endif

if HAVE_ATM
    DEFINES += -DHAVE_ATM
endif

tangram2:
	cp tangram2.base tangram2
	chmod u+w tangram2
	echo '$$JAVA_HOME/bin/java -classpath $$TANGRAM2_HOME/class/TimerBean.jar:$$TANGRAM2_HOME/class/Tangram2.jar:$$TANGRAM2_HOME/class/AbsoluteLayout.jar -Djava.library.path=$$TANGRAM2_HOME/lib  -DSTDOUT=$$STDOUT -DTANGRAM2_HOME=$$TANGRAM2_HOME $(DEFINES) Tangram2.T2main_app' >> tangram2        

CLEANFILES = tangram2
