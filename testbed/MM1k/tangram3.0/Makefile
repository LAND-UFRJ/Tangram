################################################################################
# File: Makefile                                                               #
# Authors: {gdjaime,allyson,bnetto}@land.ufrj.br                               #
# Copyright 2004 LAND/UFRJ                                                     #
# http://www.land.ufrj.br                                                      #
#                                                                              #
# Description: This makefile was created to allow users to run TANGRAM-II      #
#              without its graphical interface                                 #
#                                                                              #
################################################################################

TGIF= tgif
GRAMATICA= gramatica
GEN_CHAIN= generate_chain
SOLV= solv
GETNAMES= getNames
GETMAXVALUES= getMaxValues.sh
MARKFIND_MAKEFILE = $TANGRAM2_HOME/lib/Makefile.mark_find
METHOD= 1
PRECISION= 1e-6
ITERATIONS= 10000
MAX_STATES= 0
REWARD_OPTION= 1
SUFFIX = result
DEBUG_LEVEL = 0
RUNS = 1
CLEANFILES=*.absorb_st *.generator_mtx* *.maxvalues *.parameter *.rate_reward.* *.impulse_reward.* *.SS.* *.TS.* *.states *.trans_same_st *.uniform_rate *.parser *.user_code.c *.user_code.o *.tables_dump mark_find *.vstat *.events *.trace *state_variable *.INTSIMUL *.reward *.config *.out

.SUFFIXES:

.SUFFIXES: .obj .parser .user_code.c

.obj.parser:
	@echo "Generating $(BASE).parser from $(BASE).obj file..."
	$(TGIF) -print -tg2genparser="0;0;teste" $(BASE).obj

.parser.user_code.c:
	@echo "Generating $(BASE).user_code.c from $(BASE).parser file..."
	$(GRAMATICA) -G $(BASE).parser $(BASE) $(DEBUG_LEVEL)

all:  compile run

getnames: $(BASE).parser
	@echo "Getting names ..."
	$(GETNAMES) -v $(BASE)
	$(GETMAXVALUES) $(BASE)

chain: getnames $(BASE).maxvalues $(BASE).user_code.c
	@echo "Generating chain ..."
	$(GEN_CHAIN) $(BASE) $(MAX_STATES) $(DEBUG_LEVEL) $(REWARD_OPTION)
    
iterative-solv: chain
	@echo "Solving model using $(METHOD)"
	$(SOLV) $(BASE) $(METHOD) $(PRECISION) $(ITERATIONS)

gth-solv: chain
	@echo "Solving model using $(METHOD)"
	$(SOLV) $(BASE) $(METHOD)
    
gthblock-solv: chain
	@echo "Solving model using $(METHOD)"
	$(SOLV) $(BASE) $(METHOD)
    
compile: $(BASE).user_code.c
	@echo "Generating new mark_find from $(BASE).obj file"
	make -f $(MARKFIND_MAKEFILE) BASE=$(BASE)

run:
	rm -f mark_find
	make -f $(MARKFIND_MAKEFILE) BASE=$(BASE) 
	./mark_find -f $(BASE) -d $(DEBUG_LEVEL) -S -B -o SIMUL.$(SUFFIX) -u $(RUNS) -w 4 -y -c 1 -t $(TIME)  # sem trace :-w 4 -c 1 -t  #
	@echo
	@echo "*************************************************************************"
	@echo "Simulation results stored in $(BASE).SIMUL.$(SUFFIX) file"
	@echo "To change the result file suffix set the SUFFIX variable (default=result)"
	@echo "*************************************************************************"
	@echo

clean:
	rm -f $(CLEANFILES)

help:
	@echo
	@echo "Usage: "
	@echo "  make clean"
	@echo "  or"
	@echo "  make compile <BASE=model_base_name>"
	@echo "  or"
	@echo "  make [all|run] <BASE=model_base_name> <TIME=simul_time> [RUNS=num_of_runs]"
	@echo
	@echo "where:"
	@echo "  compile: generates mark_find from <model_base_name>.obj (compiles the model)"
	@echo "  run: generates a new mark_find from an existing $(BASE).user_code.c and runs "
	@echo "       and runs the simulation"
	@echo "  all: generates mark_find file from <model_base_name>.obj and runs a simulation"
	@echo "********************************************************************************"
	@echo
