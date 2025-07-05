#ifndef FLOATVALUE_H
#define FLOATVALUE_H

#include "types.h"
#include "object_base.h"

#include "sample.h"

class FloatValue : public ObjectBase<FloatValue>
{
public:
    FloatValue( const Name &, const FloatValue & );

protected:
    friend void initFloatValue( void );

    FloatValue( const Name & );
    virtual ~FloatValue( void );

    FloatSample sample;
    double lower, upper;
    static const double DEFAULT_BOUND;

    bool truncate_data( double, double );

    // constructors
    static Object * createEmpty( const std::string &, Arguments & );
    static Object * createLoad ( const std::string &, Arguments & );

    // options
    bool set_lower( const std::string &, const Index & );
    bool get_lower( std::string &, const Index & );
    bool set_upper( const std::string &, const Index & );
    bool get_upper( std::string &, const Index & );
    bool get_max_bound( std::string &, const Index & );

    bool set_size( const std::string &, const Index & );
    bool get_size( std::string &, const Index & );
    bool set_data( const std::string &, const Index & );
    bool get_data( std::string &, const Index & );

    // displays
    bool display_sample( Terminal & );
    bool display_bounds( Terminal & );
    bool display_stats ( Terminal & );

    // commands
    bool load       ( Arguments & );
    bool load_offset( Arguments & );
    bool save       ( Arguments & );
    bool save_offset( Arguments & );

    bool autocorrelation( Arguments & );

    bool truncate( Arguments & );

    // functions
    void * get_sample( void * );
    void * set_sample( void * );
};

#define FLOATVALUE_HELP_MESSAGE \
    "This plugin defines a sample of float values                      \n"\
    "    Constructors: floatvalue( )                                   \n"\
    "                  floatvalue( <file> )                            \n"\
    "    Where:                                                        \n"\
    "        <file>   - is the name of the file to open.               \n"

#define LOAD_HELP_MESSAGE \
    "Loads a sample from a file.                                       \n"\
    "    Usages: load( <file> )                                        \n"\
    "            load( <file>, <offset> [, <size> ] )                  \n"\
    "    Where:                                                        \n"\
    "        <file>   - is the name of the file to open.               \n"\
    "        <offset> - number of initial samples to skip inside file. \n"\
    "        <size>   - number of samples to load from file.           \n"

#define SAVE_HELP_MESSAGE \
    "Stores a sample in a file.                                        \n"\
    "    Usages: save( <file> )                                        \n"\
    "            save( <file>, <offset> [, <size> ] )                  \n"\
    "    Where:                                                        \n"\
    "        <file>   - is the name of the file to open.               \n"\
    "        <offset> - number of initial samples to skip before save. \n"\
    "        <size>   - number of samples to save to the file.         \n"

#define TRUNCATE_HELP_MESSAGE \
    "Used to truncate sample values between two values.                \n"\
    "    Usage: train( <lower>, <upper> )                              \n"\
    "    Where:                                                        \n"\
    "        <lower> - the lower bound to truncate values.             \n"\
    "        <upper> - the upper bound to truncate values.             \n"

#define AUTOCORRELATION_HELP_MESSAGE \
   "Calculates the sample autocorrelation function for a stationary   \n"\
   "process.                                                          \n"\
   "    Usage: autocorrelation( <lag> [, <start>] )                   \n"\
   "    Where:                                                        \n"\
   "        <lag>   - the desired autocorrelation lag                 \n"\
   "        <start> - position, in the array, from which to start     \n"\
   "                  evaluating acf (will start from the beginning if\n"\
   "                  <start> is not specified)                       \n"

#endif /* FLOATVALUE_H */
