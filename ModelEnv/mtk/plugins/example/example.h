#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "types.h"
#include "object_base.h"

class Example : public ObjectBase<Example>
{
public:
    Example( const Name &, const Example & );

protected:
    // libmtk communication methods
    friend void initExample( void );

    Example( const Name & );
    virtual ~Example( void );

    ////////////////////////////////////////
    // Plugin programmer code starts here //
    ////////////////////////////////////////
    
    // Plugin attributes
    int a, b, result;

    // Plugin onstructors
    static Object *createEmpty( const std::string &, Arguments & );
    static Object *createWithOperators( const std::string &, Arguments & );

    // Plugin attribute get/set
    bool set_a( const std::string &, const Index & );
    bool get_a( std::string &, const Index &       );
    bool set_b( const std::string &, const Index & );
    bool get_b( std::string &, const Index &       );
    bool set_result( const std::string &, const Index & );
    bool get_result( std::string &, const Index &       );

    // Plugin displays
    bool display_a( Terminal & );
    bool display_b( Terminal & );
    bool display_result( Terminal & );

    // Plugin commands
    bool add( Arguments & );
};

#define EXAMPLE_HELP_MESSAGE \
    "This plugin defines a two operators basic calculator              \n"\
    "    Constructors: example( )                                      \n"\
    "                  example( <op1>, <op2> )                         \n"\
    "    Where:                                                        \n"\
    "        <op1>, <op2> - are the operators                          \n"

#define ADD_HELP_MESSAGE \
    "Add the two operators and stores the result                       \n"\
    "    Usages: add( )                                                \n"\
    "            add( <int>, <int> )                                   \n"\
    "    Where:                                                        \n"\
    "        <int> - an interger constant                              \n"
#endif /* EXAMPLE_H */
