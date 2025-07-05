#include "GHMMParameters.h"
#include "MatrixEditor.h"

#include <qmessagebox.h>

#include <stdlib.h>

GHMMParameters::GHMMParameters( QWidget *parent, MainWindow *main_window )
               :HMMParameters( parent, main_window )
{
    r = NULL;
    p = NULL;
    q = NULL;        
}

void GHMMParameters::buildInterface( void )
{
    fm_parameters = new QFrame( this );

    buildN();
    buildBatchSize();
    buildPi();
    buildA();
    buildObs();    
    
    buildLayout();
    
    hide();    
}

void GHMMParameters::buildBatchSize( void )
{
    lbl_batch_size = new QLabel( "Batch size:", fm_parameters );
    sb_batch_size  = new QSpinBox( fm_parameters );
    sb_batch_size->setMinValue( 1 );
}

void GHMMParameters::buildObs( void )
{
    gb_obs   = new QGroupBox( 3, Qt::Vertical,
                              "Observation parameters", fm_parameters );

    gb_obs_r = new QButtonGroup( 2, Qt::Horizontal,
                                 "Initial probabilities of symbol 1",
                                 gb_obs );
    rb_r_random = new QRadioButton( "Random", gb_obs_r );
    rb_r_custom = new QRadioButton( "Custom", gb_obs_r );
    setDefaultR();
    connect( rb_r_custom, SIGNAL( clicked() ), 
             this, SLOT( openREditor() ) );    

    gb_obs_p = new QButtonGroup( 2, Qt::Horizontal,
                                 "Symbol 0 to 1 transition probabilities",
                                 gb_obs );
    rb_p_random = new QRadioButton( "Random", gb_obs_p );
    rb_p_custom = new QRadioButton( "Custom", gb_obs_p );    
    setDefaultP();
    connect( rb_p_custom, SIGNAL( clicked() ), 
             this, SLOT( openPEditor() ) );    
    
    gb_obs_q = new QButtonGroup( 2, Qt::Horizontal,
                                 "Symbol 1 to 0 transition probabilities",
                                 gb_obs );
    rb_q_random = new QRadioButton( "Random", gb_obs_q );
    rb_q_custom = new QRadioButton( "Custom", gb_obs_q );    
    setDefaultQ();
    connect( rb_q_custom, SIGNAL( clicked() ), 
             this, SLOT( openQEditor() ) );        
}

void GHMMParameters::buildLayout( void )
{
    // Build layout             
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 2 );

    parameters_layout->addWidget( lbl_N,           0, 0 );
    parameters_layout->addWidget( sb_N,            0, 1 );
    parameters_layout->addWidget( lbl_batch_size,  1, 0 );
    parameters_layout->addWidget( sb_batch_size,   1, 1 );           
    parameters_layout->addMultiCellWidget( gb_pi,  2, 2, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,   3, 3, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_obs, 4, 4, 0, 1 );      
}

void GHMMParameters::resetStateStructures( int )
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
        message += "Initial probabilities of symbol 1\n";
    }
    
    if( gb_obs_p->selectedId() == p_custom )
    {
        setDefaultP();
        message += "Symbol 0 to 1 transition probabilities\n";
    }
    
    if( gb_obs_q->selectedId() == q_custom )
    {
        setDefaultQ();
        message += "Symbol 1 to 0 transition probabilities\n";
    }
    
    if( message != "" )
        QMessageBox::warning( 0, "Number of states changed",
                                 "Will reset the following structures "
                                 "to their default values:\n" + message );    
}

void GHMMParameters::setDefaultR( void )
{
    rb_r_random->setChecked( true );  
}

void GHMMParameters::setDefaultP( void )
{
    rb_p_random->setChecked( true );  
}

void GHMMParameters::setDefaultQ( void )
{
    rb_q_random->setChecked( true );  
}

void GHMMParameters::openREditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    r,
                                                    "Editing initial "
                                                    "probabilities of "
                                                    "symbol 1",
                                                    MatrixEditor::independent_prob );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeVector( r, sb_N->value() );
        matrix_editor->getVector( r );
    }
    else
        setDefaultR();

    delete matrix_editor;
}

void GHMMParameters::openPEditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    p,
                                                    "Editing symbol 0 to 1 "
                                                    "transition "
                                                    "probabilities",
                                                    MatrixEditor::independent_prob );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeVector( p, sb_N->value() );
        matrix_editor->getVector( p );
    }
    else
        setDefaultP();

    delete matrix_editor;
}

void GHMMParameters::openQEditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    q,
                                                    "Editing symbol 1 to 0 "
                                                    "transition "
                                                    "probabilities",
                                                    MatrixEditor::independent_prob );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeVector( q, sb_N->value() );
        matrix_editor->getVector( q );
    }
    else
        setDefaultQ();

    delete matrix_editor;
}

int GHMMParameters::getBatchSize( void )
{
    return sb_batch_size->value();
}

GHMMParameters::RVectorType GHMMParameters::getRVectorType( void )
{
    return (GHMMParameters::RVectorType)gb_obs_r->selectedId();
}

double *GHMMParameters::getRVector( void )
{
    return r;
}

GHMMParameters::PVectorType GHMMParameters::getPVectorType( void )
{
    return (GHMMParameters::PVectorType)gb_obs_p->selectedId();
}

double *GHMMParameters::getPVector( void )
{
    return p;
}

GHMMParameters::QVectorType GHMMParameters::getQVectorType( void )
{
    return (GHMMParameters::QVectorType)gb_obs_q->selectedId();
}

double *GHMMParameters::getQVector( void )
{
    return q;
}
