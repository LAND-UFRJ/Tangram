#ifndef FLOATVALUEPARAMETERS_H
#define FLOATVALUEPARAMETERS_H

#include "IntValueParameters.h"

class FloatValueParameters: public IntValueParameters
{
    Q_OBJECT

    public:
        FloatValueParameters( QWidget *, MainWindow * );
};

#endif // FLOATVALUEPARAMETERS_H
