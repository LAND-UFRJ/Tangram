#ifndef FLOATVALUEEDIT_H
#define FLOATVALUEEDIT_H

#include "IntValueEdit.h"

class FloatValueEdit: public IntValueEdit
{
    Q_OBJECT

    private:
        QDoubleValidator *double_validator;
        void              resetValidators( void );

    public:
        FloatValueEdit( Object *, bool = false );
};

#endif // FLOATVALUEEDIT_H
