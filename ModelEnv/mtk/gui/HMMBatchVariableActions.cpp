#include "HMMBatchVariableActions.h"

HMMBatchVariableActions::HMMBatchVariableActions( QWidget *parent,
                                                  Object *object )
                        :HMMBatchActions( parent, object )
{
}

void HMMBatchVariableActions::buildInterface( void )
{
    // Call parent's method, since the actions are the same
    HMMBatchActions::buildInterface();

    setTitle( "HMM Batch Variable object actions" );
}
