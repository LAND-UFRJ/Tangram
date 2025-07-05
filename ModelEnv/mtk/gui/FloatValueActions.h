#ifndef FLOATVALUEACTIONS_H
#define FLOATVALUEACTIONS_H

#include "IntValueActions.h"

class FloatValueActions: public IntValueActions
{
    Q_OBJECT

    private:
        QDoubleValidator *double_validator; 

        void              resetValidators( void );

    private slots:
        void truncate( void );

    public:
        FloatValueActions( QWidget *, Object * );
};

#endif // FLOATVALUEACTIONS_H
