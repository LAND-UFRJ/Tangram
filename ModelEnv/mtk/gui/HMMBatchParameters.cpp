#include "HMMBatchParameters.h"
#include "MatrixEditor.h"

#include <qmessagebox.h>

#include <stdlib.h>

HMMBatchParameters::HMMBatchParameters( QWidget *parent, 
                                        MainWindow *main_window )
                   :GHMMParameters( parent, main_window )
{
    r = NULL;
    p = NULL;
}

void HMMBatchParameters::buildInterface( void )
{
    fm_parameters = new QFrame( this );

    buildN();
    buildM();    
    buildBatchSize();
    buildPi();
    buildA();
    buildObs();
    
    buildLayout();    
        
    hide();    
}

void HMMBatchParameters::buildLayout( void )
{
    // Build layout             
    parameters_layout = new QGridLayout( fm_parameters, 6, 2, 2 );

    parameters_layout->addWidget( lbl_N,           0, 0 );
    parameters_layout->addWidget( sb_N,            0, 1 );
    parameters_layout->addWidget( lbl_M,           1, 0 );
    parameters_layout->addWidget( sb_M,            1, 1 );
    parameters_layout->addWidget( lbl_batch_size,  2, 0 );
    parameters_layout->addWidget( sb_batch_size,   2, 1 );           
    parameters_layout->addMultiCellWidget( gb_pi,  3, 3, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,   4, 4, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_obs, 5, 5, 0, 1 );      
}

void HMMBatchParameters::buildObs( void )
{
    gb_obs   = new QGroupBox( 3, Qt::Vertical,
                              "Observation parameters", fm_parameters );

    gb_obs_r = new QButtonGroup( 2, Qt::Horizontal,
                                 "Initial probabilities of symbols at "
                                 "each state",
                                 gb_obs );
    rb_r_random = new QRadioButton( "Random", gb_obs_r );
    rb_r_custom = new QRadioButton( "Custom", gb_obs_r );
    setDefaultR();
    connect( rb_r_custom, SIGNAL( clicked() ), 
             this, SLOT( openREditor() ) );    

    gb_obs_p = new QButtonGroup( 2, Qt::Horizontal,
                                 "Symbol transition matrices at each state",
                                 gb_obs );
    rb_p_random = new QRadioButton( "Random", gb_obs_p );
    rb_p_custom = new QRadioButton( "Custom", gb_obs_p );    
    setDefaultP();
    connect( rb_p_custom, SIGNAL( clicked() ), 
             this, SLOT( openPEditor() ) );    
}

void HMMBatchParameters::setDefaultR( void )
{
    rb_r_random->setChecked( true );  
}

void HMMBatchParameters::setDefaultP( void )
{
    rb_p_random->setChecked( true );  
}

void HMMBatchParameters::openREditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    sb_M->value(),
                                                    r,
                                                    "Editing initial "
                                                    "probabilities of symbols "
                                                    "at each state" );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeMatrix( r, sb_N->value(), sb_M->value() );
        matrix_editor->getMatrix( r );
    }
    else
        setDefaultR();

    delete matrix_editor;
}

void HMMBatchParameters::openPEditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    sb_M->value(),
                                                    sb_M->value(),
                                                    p,
                                                    "Editing symbol "
                                                    "transition matrices "
                                                    "at each state" );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resize3DMatrix( p, sb_N->value(), sb_M->value(), sb_M->value() );
        matrix_editor->get3DMatrix( p );
    }
    else
        setDefaultP();

    delete matrix_editor;
}

void HMMBatchParameters::resetStateStructures( int )
{
    QString message;
    
    if( gb_pi->selectedId() == pi_custom )
    {
        setDefaultPi();
        message += "Initial state distribution\n";
    }
    
    // reset A only if QBD or custom is selected
    if( ( gb_A->selectedId() == A_qdb ) || ( gb_A->selectedId() == A_custom ) )
    {
        setDefaultA();
        message += "State transition matrix\n";
    }
    
    if( gb_obs_r->selectedId() == r_custom )
    {
        setDefaultR();
        message += "Initial probabilities of symbols at each state\n";
    }
    
    if( gb_obs_p->selectedId() == p_custom )
    {
        setDefaultP();
        message += "Symbol transition matrices at each state\n";
    }
    
    if( message != "" )
        QMessageBox::warning( 0, "Number of states changed",
                                 "Will reset the following structures "
                                 "to their default values:\n" + message );  
}

void HMMBatchParameters::resetSymbolStructures( int )
{
    QString message;
    
    if( gb_obs_r->selectedId() == r_custom )
    {
        setDefaultR();
        message += "Initial probabilities of symbols at each state\n";
    }
    
    if( gb_obs_p->selectedId() == p_custom )
    {
        setDefaultP();
        message += "Symbol transition matrices at each state\n";
    }
    
    if( message != "" )
        QMessageBox::warning( 0, "Number of symbols changed",
                                 "Will reset the following structures "
                                 "to their default values:\n" + message );
}

HMMBatchParameters::RMatrixType HMMBatchParameters::getRMatrixType( void )
{
    return (HMMBatchParameters::RMatrixType)gb_obs_r->selectedId();
}

double **HMMBatchParameters::getRMatrix( void )
{
    return r;
}

HMMBatchParameters::PMatrixType HMMBatchParameters::getPMatrixType( void )
{
    return (HMMBatchParameters::PMatrixType)gb_obs_p->selectedId();
}

double ***HMMBatchParameters::getPMatrix( void )
{
    return p;
}
