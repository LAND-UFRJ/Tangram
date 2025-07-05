#ifndef HMMBatch_H
#define HMMBatch_H

#include "types.h"
#include "object_base.h"
#include "param.h"

#include <vector>

class HMMBatch : public ObjectBase<HMMBatch>
{
public:
    HMMBatch( const Name &, const HMMBatch & );

protected:
    friend void initHMMBatch( void );

    HMMBatch( const Name &, unsigned int, unsigned int );
    HMMBatch( const Name & );
    virtual ~HMMBatch( void );

    Parameter parameter;
    double epsilon;
    
    // Results array
    std::vector<double> result;
    void saveResult( int, double );    

    // constructors
    static Object * createEmpty( const Name &, Arguments & );
    static Object * createSized( const Name &, Arguments & );

    // options
    // Number of hidden states
    bool set_N ( const std::string &, const Index & );
    bool get_N ( std::string &, const Index & );
    // Number of observations
    bool set_M ( const std::string &, const Index & );
    bool get_M ( std::string &, const Index & );
    // PI Matrix 
    bool set_pi( const std::string &, const Index & );
    bool get_pi( std::string &, const Index & );
    // A Matrix
    bool set_A ( const std::string &, const Index & );
    bool get_A ( std::string &, const Index & );
    // Markov Model (inside the hidden states)
    bool set_p ( const std::string &, const Index & );
    bool get_p ( std::string &, const Index & );
    // Probability of the initial state  
    bool set_r ( const std::string &, const Index & );
    bool get_r ( std::string &, const Index & );
    // Result array functions
    bool set_result( const std::string &, const Index & );
    bool get_result( std::string &,       const Index & );    

    // displays
    bool display_all( Terminal & );
    bool display_pi ( Terminal & );
    bool display_A  ( Terminal & );
    bool display_obs( Terminal & );

    // loads
    bool load_all( const std::string & );
    bool load_pi ( const std::string & );
    bool load_A  ( const std::string & );
    bool load_obs( const std::string & );
    
    // saves
    bool save_all( const std::string & );
    bool save_pi ( const std::string & );
    bool save_A  ( const std::string & );
    bool save_obs( const std::string & ); 

    // commands
    bool load                   ( Arguments & );
    bool save                   ( Arguments & );

    bool exec_normalize         ( Arguments & );

    bool exec_train             ( Arguments & );
    bool exec_train_fast        ( Arguments & );
    bool exec_train_threshold   ( Arguments & );
    bool exec_train_fast_threshold( Arguments & );
    bool exec_train_complete    ( Arguments & );

    bool exec_simulate          ( Arguments & );
    bool exec_viterbi           ( Arguments & );
    bool exec_likelihood        ( Arguments & );
    bool exec_likelihood_fast   ( Arguments & );

    // Tangram related functions
    bool import_from_tangram       ( Arguments & );
    bool import_sparse_A           ( char *      );
    bool import_sparse_initial_prob( int, char * );    
    bool import_sparse_transitions ( int, char * );  

    bool set_full               ( Arguments & );
    bool set_coxian             ( Arguments & );
    bool set_qbd                ( Arguments & );

    bool fix_full               ( Arguments & );
    bool fix_coxian             ( Arguments & );
    bool fix_qbd                ( Arguments & );

    bool set_epsilon            ( Arguments & );

    void fix_obs                ( void );
    void fix_qbd_matrix         ( int, int, int, int );

    bool state_prob( Arguments & );
};

#define HMMBatch_HELP_MESSAGE \
    "This plugin defines a discrete-time, 1-M observations Batch HMM.  \n"\
    "Hidden states are non-negative 32-bit integers.                   \n"\
    "Batch size is variable.                                           \n"\
    "    Constructors: hmm_batch_variable( )                           \n"\
    "    Constructors: hmm_batch_variable( <N>, <M> )                  \n"\
    "    Where:                                                        \n"\
    "        <N> - number of hidden states.                            \n"\
    "        <M> - number of observations.                             \n"

#define LOAD_HELP_MESSAGE \
    "Loads a model description from a file.                            \n"\
    "    Usage: load( <file>, [<parameter>] )                          \n"\
    "    Where <file> is the name of the file to open.                 \n"\
    "          <parameter> is the name of the parameter to load.       \n"\
    "                      Available parameters are: {all, pi, A, obs}.\n"\
    "    Warning: Both <file> and <parameter> should be written between\n"\
    "             quote marks.                                         \n"

#define SAVE_HELP_MESSAGE \
    "Saves a model description to a file.                              \n"\
    "    Usage: save( <file>, [<parameter>] )                          \n"\
    "    Where <file> is the name of the file to write.                \n"\
    "          <parameter> is the name of the parameter to save.       \n"\
    "                      Available parameters are: {all, pi, A, obs}.\n"\
    "    Warning: Both <file> and <parameter> should be written between\n"\
    "             quote marks.                                         \n"

#define NORMALIZE_HELP_MESSAGE \
    "Normalizes the model's parameters to satisfy the stochastic       \n"\
    "constraints.                                                      \n"\
    "    Usage: normalize( )                                           \n"\
    "                                                                  \n"\
    "Note: the normalization of an array V is done by dividing every   \n"\
    "      element of V by the sum of ithe elements of the array, i.e.,\n"\
    "      normalized V[i] = V[i]/sum(V[i's]).                         \n"

#define TRAIN_HELP_MESSAGE \
    "Estimates the model parameters by maximizing the likelihood of a  \n"\
    "given observation sample, using the Baum-Welch algorithm.         \n"\
    "    Usage: training( <iterations>, [<threshold>], <object> )         \n"\
    "    Where:                                                        \n"\
    "        <iterations> - number of iterations to perform in the     \n"\
    "                       Baum-Welch algorithm.                      \n"\
    "        <threshold>  - log-likelihood threshold to stop training. \n"\
    "        <object>     - the object containing the observations.    \n"

#define TRAIN_FAST_HELP_MESSAGE \
    "This is an optimized version of the train() function.             \n"\
    "    Usage: training_fast( <iterations>, [<threshold>], <object> ) \n"\
    "    Where:                                                        \n"\
    "        <iterations> - number of iterations to perform in the     \n"\
    "                       Baum-Welch algorithm.                      \n"\
    "        <threshold>  - log-likelihood threshold to stop training. \n"\
    "        <object>     - the object containing the observations.    \n"

#define SIMULATE_HELP_MESSAGE \
    "Generates a random sample path from the model.                    \n"\
    "    Usage: simulate( <sample size>, <object1> [, <object2> ] )    \n"\
    "    Where:                                                        \n"\
    "        <sample size> - the number of observation time steps to   \n"\
    "                        simulate.                                 \n"\
    "        <object1>     - object used to store the observations.    \n"\
    "        <object2>     - object used to store the hidden states.   \n"

#define VITERBI_HELP_MESSAGE \
    "Evaluates the most likely sequence of states to have produced a   \n"\
    "given observation sample.                                         \n"\
    "    Usage: viterbi( <object1>, <object2> )                        \n"\
    "    Where:                                                        \n"\
    "        <object1> - the object containing the observations.       \n"\
    "        <object2> - the object used to store the hidden states.   \n"

#define LIKELIHOOD_HELP_MESSAGE \
    "Calculates and displays the probability that a given sample was   \n"\
    "generated by the current model parameters.                        \n"\
    "    Usage: likelihood( <object> )                                 \n"\
    "    Where <object> is the object containing the sample.           \n"

#define LIKELIHOOD_FAST_HELP_MESSAGE \
    "This is an optimizaed version of the likelihood method.           \n"\
    "    Usage: likelihood( <object> )                                 \n"\
    "    Where <object> is the object containing the sample.           \n"

#define IMPORT_FROM_TANGRAM_HELP_MESSAGE \
    "Import structures from a Tangram2 model.                          \n"\
    "    Usage: import_tangram( <obj_name>,                            \n"\
    "                           <num_hidden_vars>, <num_internal_vars>,\n"\
    "                           <system_variable_list> )               \n"\
    "    Where:                                                        \n"\
    "        <obj_name>          - model description                   \n"\
    "        <num_hidden_vars>   - number of system state variable     \n"\
    "                              which represent hidden states       \n"\
    "        <num_internal_vars> - number of system state variable     \n"\
    "                              which represent internal states     \n"\
    "        <state_variable_list> - comma separated list with         \n"\
    "                               variable names in the format       \n"\
    "                               'Object.StateVariable'.            \n"\
    "                               The list must have                 \n"\
    "                               num_hidden_vars +                  \n"\
    "                               num_internal_vars elements         \n"

#define SET_EPSILON_HELP_MESSAGE \
    "Sets model error tolerance. Default value is 0.00001.             \n"\
    "    Usage: set_epsilon( float_value )                             \n"    

#define STATE_PROB_HELP_MESSAGE \
    "Evaluates the probability of each hidden state. Can be based on   \n"\
    "observations.                                                     \n"\
    "    Usage: state_prob( )                                          \n"\
    "           state_prob( <object_src> )                             \n"\
    "    Where:                                                        \n"\
    "        <object_src> - the object containing the observations.    \n"

#define SET_FULL_HELP_MESSAGE \
    "Fills matrices PI, A, p and r with random values, obeying the     \n"\
    "stochastic constrains.                                            \n"\
    "    Usage: set_full( <N>, <M> )                                   \n"\
    "    Where:                                                        \n"\
    "        <N> - number of states.                                   \n"\
    "        <M> - number of symbols.                                  \n"

#define SET_COXIAN_HELP_MESSAGE \
    "Fills matrices PI, A, p and r with random values, obeying the     \n"\
    "stochastic constrains and the coxian structure.                   \n"\
    "    Usage: set_coxian( <N>, <M> )                                 \n"\
    "    Where:                                                        \n"\
    "        <N> - number of states.                                   \n"\
    "        <M> - number of symbols.                                  \n"

#define SET_QBD_HELP_MESSAGE \
    "Fills matrices PI, A, p and r with random values, obeying the     \n"\
    "stochastic constrains and the quasi birth-death structure.        \n"\
    "    Usage: set_qbd( <G>, <E>, <M> [, <BACK> [, <FWD> ] ] )        \n"\
    "    Where:                                                        \n"\
    "        <G>    - number of group of states.                       \n"\
    "        <E>    - number of elements (states) per group.           \n"\
    "                 The number of states are G times E.              \n"\
    "        <M>    - number of symbols.                               \n"\
    "        <BACK> - number of reachable back groups.                 \n"\
    "        <FWD>  - number of reachable forward groups.              \n"

#define FIX_FULL_HELP_MESSAGE \
    "Adds epsilon to each value of PI, A, p and r matrices, and        \n"\
    "normalizes them.                                                  \n"\
    "    Usage: fix_full( <N>, <M> )                                   \n"\
    "    Where:                                                        \n"\
    "        <N> - number of states.                                   \n"\
    "        <M> - number of symbols.                                  \n"

#define FIX_COXIAN_HELP_MESSAGE \
    "Obeying the coxian structure, adds epsilon to each value of PI, A,\n"\
    "p and r matrices, and normalizes them.                            \n"\
    "    Usage: fix_coxian( <N>, <M> )                                 \n"\
    "    Where:                                                        \n"\
    "        <N> - number of states.                                   \n"\
    "        <M> - number of symbols.                                  \n"

#define FIX_QBD_HELP_MESSAGE \
    "Obeying the quasi birth-death structure, adds epsilon to each     \n"\
    "value of PI, A, p and r matrices, and normalizes them.            \n"\
    "    Usage: fix_qbd( <G>, <E>, <M> [, <BACK> [, <FWD> ] ] )        \n"\
    "    Where:                                                        \n"\
    "        <G>    - number of group of states.                       \n"\
    "        <E>    - number of elements (states) per group.           \n"\
    "                 The number of states are G times E.              \n"\
    "        <M>    - number of symbols.                               \n"\
    "        <BACK> - number of reachable back groups.                 \n"\
    "        <FWD>  - number of reachable forward groups.              \n"

#endif /* HMMBatch_H */
