#ifndef MA_H
#define MA_H

#include "types.h"
#include "object_base.h"

class MA : public ObjectBase<MA>
{
public:
    MA( const Name &, const MA & );

protected:
    // libmtk communication methods
    friend void initMA( void );

    MA( const Name & );
    virtual ~MA( void );

    ////////////////////////////////////////
    // Plugin programmer code starts here //
    ////////////////////////////////////////
    
    // Plugin attributes
    unsigned int q;
    double       variance, *theta;

    // Plugin onstructors
    static Object *createEmpty( const std::string &, Arguments & );
    static Object *createWithOperators( const std::string &, Arguments & );

    // Plugin attribute get/set
    bool set_q(        const std::string &, const Index & );
    bool get_q(        std::string &,       const Index & );
    bool set_variance( const std::string &, const Index & );
    bool get_variance( std::string &,       const Index & );
    bool set_theta(    const std::string &, const Index & );
    bool get_theta(    std::string &,       const Index & );

    // Plugin displays
    bool display_q(        Terminal & );
    bool display_variance( Terminal & );
    bool display_theta(    Terminal & );

    // Plugin commands
    bool simulate( Arguments & );
    
    // Auxiliar functions
    void build_theta( void );
};

#define MA_HELP_MESSAGE \
    "This plugin defines a moving average process of order q, with white    \n"\
    "noise variance specified                                               \n"\
    "    Constructor: ma( q, variance )                                     \n"

#define SIMULATE_HELP_MESSAGE \
    "Simulates the model for a given number of steps                   \n"\
    "    Usage: simulate( steps )                                      \n"
#endif /* MA_H */
