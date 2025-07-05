lib_LIBRARIES = libmark_find.a

if HAVE_PVM
    lib_LIBRARIES += libdistributed_mark_find.a
endif

SOURCES = action_desc.cpp expression.cpp rate_reward.cpp rate_reward_desc.cpp \
          string_list.cpp chained_list.cpp expr_val.cpp mark_find.cpp \
          rew_measure.cpp string_list.h chained_list.h mem_debug.h rew_sum.cpp \
          symbol_desc.cpp general.cpp simulator.cpp system_desc.cpp clone_ev.cpp \
          general.h simulator.h system_desc.h object_desc.cpp simul_control.cpp \
          system_state.cpp $(top_srcdir)/ModelEnv/grammar/gramatica.h object_state.cpp  simul_event.cpp \
          system_state.h output.cpp simul_interactive.cpp T2String.cpp debug.h \
          output.h simul_level.cpp T2String.h imp_reward.cpp pending_message.cpp \
          simul_result.cpp $(top_srcdir)/ModelEnv/grammar/tangram_types.h distribution.cpp imp_reward_desc.cpp \
          prototypes.h simul_run.cpp transition.cpp distribution.h indep_chain.cpp \
          queue_type.cpp simul_sched.cpp queue_type.h simul_split.cpp \
          event_msg_desc.cpp known_state.cpp random.cpp simul_split.h random.h \
          state_var.cpp seeds_table.cpp seeds_table.h

libmark_find_a_SOURCES = $(SOURCES)

libmark_find_a_CPPFLAGS = -Wall -I$(top_srcdir)/ModelEnv/grammar -I$(top_srcdir)/ModelEnv/hash_table -DHANDLESIGNAL

if HAVE_PVM
    libdistributed_mark_find_a_SOURCES = $(SOURCES)
    libdistributed_mark_find_a_CPPFLAGS = -Wall -I$(top_srcdir)/ModelEnv/hash_table \
                                          -DTG_PARALLEL -I$(PVM_ROOT)/include
endif
