#include "HMMBatchVariableParameters.h"

HMMBatchVariableParameters::HMMBatchVariableParameters( QWidget *parent, 
                                                        MainWindow *main_window )
                           :HMMBatchParameters( parent, main_window )
{
}

void HMMBatchVariableParameters::buildInterface( void )
{
    fm_parameters = new QFrame( this );

    buildN();
    buildM();    
    buildPi();
    buildA();
    buildObs();
    
    buildLayout();    
        
    hide();    
}

void HMMBatchVariableParameters::buildLayout( void )
{
    // Build layout             
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 2 );

    parameters_layout->addWidget( lbl_N,           0, 0 );
    parameters_layout->addWidget( sb_N,            0, 1 );
    parameters_layout->addWidget( lbl_M,           1, 0 );
    parameters_layout->addWidget( sb_M,            1, 1 );
    parameters_layout->addMultiCellWidget( gb_pi,  2, 2, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,   3, 3, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_obs, 4, 4, 0, 1 );      
}
