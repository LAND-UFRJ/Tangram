#ifndef HMMBATCHVARIABLEPARAMETERS_H
#define HMMBATCHVARIABLEPARAMETERS_H

#include "MainWindow.h"
#include "HMMBatchParameters.h"

class HMMBatchVariableParameters: public HMMBatchParameters
{
    Q_OBJECT

    private:
        void buildLayout( void );    

    public:
        void buildInterface( void );

        HMMBatchVariableParameters( QWidget *, MainWindow * );
};

#endif // HMMBATCHVARIABLEPARAMETERS_H
