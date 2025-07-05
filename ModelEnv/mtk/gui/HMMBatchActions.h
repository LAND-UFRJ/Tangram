#ifndef HMMBATCHACTIONS_H
#define HMMBATCHACTIONS_H

#include "GHMMActions.h"

class HMMBatchActions: public GHMMActions
{
    Q_OBJECT

    public:
        /* Note: buildInterface() is public because virtual function cannot be
           called inside constructor */    
        void buildInterface( void );

        HMMBatchActions( QWidget *, Object * );
};

#endif // HMMBATCHACTIONS_H
