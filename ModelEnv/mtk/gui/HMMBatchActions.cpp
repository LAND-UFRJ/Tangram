#include "HMMBatchActions.h"

HMMBatchActions::HMMBatchActions( QWidget *parent, Object *object )
                :GHMMActions( parent, object )
{
}

void HMMBatchActions::buildInterface( void )
{
    setTitle( "HMM Batch object actions" );

    tb_actions = new QTabWidget( this );
    
    buildViterbiFrame();
    buildLikelihoodFrame();
    buildTrainFrame();    
    buildSimulateFrame();       
    buildStateProbabilityFrame();    
    buildSaveFrame();    

    hide();
}
