#ifndef HMMBATCHVARIABLEACTIONS_H
#define HMMBATCHVARIABLEACTIONS_H

#include "HMMBatchActions.h"

class HMMBatchVariableActions: public HMMBatchActions
{
    Q_OBJECT

    public:
        /* Note: buildInterface() is public because virtual function cannot be
           called inside constructor */    
        void buildInterface( void );

        HMMBatchVariableActions( QWidget *, Object * );
};

#endif // HMMBATCHVARIABLEACTIONS_H
