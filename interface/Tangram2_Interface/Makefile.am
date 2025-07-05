tangram2SOURCES = Entrada.java ExampleFileFilterEx.java ExampleFileFilter.java\
                  JMultiFileChooser.java myCombo.java Processo.java\
                  SwingWorker.java T2about.java\
                  T2Analytical_Iterative_Methods.java\
                  T2analytical.java T2Analytical_Stationary_Exact.java\
                  T2Analytical_Stationary_Iterative.java\
                  T2Analytical_Stationary_Non_markovian.java\
                  T2Analytical_Transient_Dist_Cumulative_Reward.java\
                  T2Analytical_Transient_Dist_Operational_Time.java\
                  T2Analytical_Transient_EV_ECR_ESRA_Direct.java\
                  T2Analytical_Transient_EV_ECR_ESRA_Iterative.java\
                  T2Analytical_Transient_EV_ExpCumImpRew.java\
                  T2Analytical_Transient_EV_ExpCumRew.java\
                  T2Analytical_Transient_EV_FracTime_CR_AbLev.java\
                  T2Analytical_Transient_PP_ESPA_Direct.java\
                  T2Analytical_Transient_PP_ESPA_Iterative.java\
                  T2Analytical_Transient_PP_Uniformization.java\
                  T2Analytical_Transient_Probability_and_Intervals.java\
                  T2animplot.java T2cacalg.java T2command.java T2config.java\
                  T2descriptors.java T2enable_plot.java T2functions.java\
                  T2histogram_mse.java T2inputdialog.java T2intsimulation.java\
                  T2literal.java T2main_app.java T2Matrix_Blocks_Manager.java\
                  T2matrixperm.java T2measures.java T2modelenv.java T2modelgen.java\
                  T2output.java T2plot.java T2plotprop.java T2progressDialog.java\
                  T2reward_data.java T2simulation.java T2simulation_reward.java\
                  T2simulation_options.java\
                  T2status.java T2traffic.java T2traffic_measures.java\
                  TangramFilter.java threadCapOut.java threadWaitFor.java\
                  T2MTKHmm.java T2MTKCommon.java  T2MTKHmmMethods.java\
                  T2MTKHmmReadForm.java T2MTKHmmIO.java T2MTKMain.java\
		  T2MTKMessages.java T2MTKHmmResults.java T2MTKHmmImportFromTangram.java\
		  T2MTKHmmView.java T2MTKHmmReadFormSymbol.java\
		  T2MTKHierarchicalHmmImportFromTangram.java\
		  T2MTKHierarchicalHmmMethods.java T2MTKHierarchicalHmmReadForm.java\
		  T2MTKHmmMatrixperm.java T2MTKHierarchicalHmmMatrixperm.java\
                  T2MTKGhmm.java T2MTKGhmmIO.java T2MTKHierarchicalHmm.java T2MTKHierarchicalHmmIO.java


CLASSPATH = ${JAVA_HOME}:${top_srcdir}/interface/Tangram2_Interface:${top_srcdir}/interface/end_user_needed/TimerBean.jar:${top_srcdir}/interface/end_user_needed/AbsoluteLayout.jar

JAVACFLAGS = -classpath $(CLASSPATH) -d ${srcdir}

BUILT_SOURCES = Tangram2.jar

CLEANFILES = Tangram2/imgs/* Tangram2/*.class Tangram2.jar

clean-local:
	rm -rf $(srcdir)/Tangram2

TEMPDIR = /tmp

Tangram2.jar: $(tangram2SOURCES)
	$(JAVAC) $(JAVACFLAGS) $(tangram2SOURCES)
	mkdir -p Tangram2/imgs
	cp -p imgs/*jpg imgs/*gif imgs/*pl Tangram2/imgs/
	$(JAR) cv0f ${srcdir}/Tangram2.jar Tangram2

EXTRA_DIST = imgs $(tangram2SOURCES) *.form

install-data-hook: Tangram2.jar
