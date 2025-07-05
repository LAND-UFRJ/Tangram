#include "FloatValueActions.h"

#include <stdio.h>

FloatValueActions::FloatValueActions( QWidget *parent, Object *object )
                  :IntValueActions( parent, object )
{
    // Invoke IntValueActions constructor's
    
    setTitle( "FloatValue object actions" );
    resetValidators();
}

/**
 * Set LineEdits validator's to Double
 */
void FloatValueActions::resetValidators( void )
{
    // Get bounds
    std::string value;
    Index       index;
    double      bound;

    object->get( "max_bound", value, index );
    sscanf( value.c_str(), "%lf", &bound );
        
    // Set validators
    double_validator = new QDoubleValidator( -bound, bound, 4, this );

    le_lower->clearValidator();
    le_upper->clearValidator();

    le_lower->setValidator( double_validator );
    le_upper->setValidator( double_validator );
}

void FloatValueActions::truncate( void )
{
    Arguments args;
    Token     tk_lower, tk_upper;
    
    tk_lower.type = TK_REAL;
    tk_lower.text = le_lower->text().ascii();
    args.push_back( tk_lower );    

    tk_upper.type = TK_REAL;
    tk_upper.text = le_upper->text().ascii();
    args.push_back( tk_upper );
    
    object->exec( "truncate", args );              
}

