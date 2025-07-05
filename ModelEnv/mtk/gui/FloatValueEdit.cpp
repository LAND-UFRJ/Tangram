#include "FloatValueEdit.h"

FloatValueEdit::FloatValueEdit( Object *object, bool read_only )
               :IntValueEdit( object, read_only )
{
    // Invoke IntValueEdit constructor's
    
    resetValidators();
}

void FloatValueEdit::resetValidators( void )
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
