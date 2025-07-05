#include "HMMParameters.h"
#include "MatrixEditor.h"
#include "QBDParameters.h"

#include <qmessagebox.h>

#include <stdlib.h>

HMMParameters::HMMParameters( QWidget *parent, MainWindow *main_window )
              :QGroupBox( 1, Qt::Vertical, "Parameters", parent )
{
    this->main_window = main_window;
    pi                = NULL;
    A                 = NULL; 
    B                 = NULL; 
    qbd_parameters    = new QBDParameterValues();
}

void HMMParameters::buildInterface( void )
{
    fm_parameters = new QFrame( this );
    
    ////////// HMM elements //////////
    buildN();
    buildM();
    buildPi();
    buildA();
    buildB();                
    
    buildLayout();     

    hide();
}
        
void HMMParameters::buildN( void )
{
    // Number of states
    lbl_N  = new QLabel( "Number of states:", fm_parameters, "lbl_N"  );
    sb_N   = new QSpinBox( fm_parameters, "sb_N" );
    sb_N->setMinValue( 1 );
    connect( sb_N, SIGNAL( valueChanged ( int ) ), 
             this, SLOT( resetStateStructures( int ) ) );
}

void HMMParameters::buildM( void )
{
    // Number of observations
    lbl_M  = new QLabel( "Number of symbols:", fm_parameters, "lbl_M"  );
    sb_M   = new QSpinBox( fm_parameters, "sb_M" );
    sb_M->setMinValue( 1 );
    connect( sb_M, SIGNAL( valueChanged ( int ) ), 
             this, SLOT( resetSymbolStructures( int ) ) );    

}

void HMMParameters::buildPi( void )
{
    // Initial distribution vector
    gb_pi        = new QButtonGroup( 2, Qt::Horizontal,
                                     "Initial state distribution",
                                     fm_parameters );
    rb_pi_random = new QRadioButton( "Random", gb_pi, "rb_pi_random" );
    rb_pi_custom = new QRadioButton( "Custom", gb_pi, "rb_pi_custom" );
    setDefaultPi();
    connect( rb_pi_custom, SIGNAL( clicked() ), 
             this, SLOT( openPiEditor() ) );
}

void HMMParameters::buildA( void )
{
    // State transition matrix
    gb_A         = new QButtonGroup( 2, Qt::Horizontal,
                                     "State transition matrix", fm_parameters );
    rb_A_full    = new QRadioButton( "Full",              gb_A, "rb_A_full"   );
    rb_A_qbd     = new QRadioButton( "Quasi-birth-death", gb_A, "rb_A_qbd"    );
    rb_A_coxian  = new QRadioButton( "Coxian",            gb_A, "rb_A_coxian" );
    rb_A_custom  = new QRadioButton( "Custom",            gb_A, "rb_A_custom" );
    setDefaultA();
    connect( rb_A_custom, SIGNAL( clicked() ), 
             this, SLOT( openAEditor() ) );
    connect( rb_A_qbd, SIGNAL( clicked() ), 
             this, SLOT( openQBDParameters() ) );
}
 
void HMMParameters::buildB( void )
{
    // Symbol emition matrix
    gb_B        = new QButtonGroup( 2, Qt::Horizontal,
                                    "Observation symbol matrix",
                                    fm_parameters );    
    rb_B_random = new QRadioButton( "Random", gb_B, "rb_B_random" );
    rb_B_custom = new QRadioButton( "Custom", gb_B, "rb_B_custom" );
    setDefaultB();
    connect( rb_B_custom, SIGNAL( clicked() ), 
             this, SLOT( openBEditor() ) );
}

void HMMParameters::buildLayout( void )
{
    // Build layout             
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 2 );

    parameters_layout->addWidget( lbl_N, 0, 0 );
    parameters_layout->addWidget( sb_N,  0, 1 );
    parameters_layout->addWidget( lbl_M, 1, 0 );
    parameters_layout->addWidget( sb_M,  1, 1 );            
    parameters_layout->addMultiCellWidget( gb_pi, 2, 2, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,  3, 3, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_B,  4, 4, 0, 1 );      
}

/**
 * When user changes the number of states, unselect user defined pi, A and B
 */
void HMMParameters::resetStateStructures( int )
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

    if( gb_B->selectedId() == B_custom )
    {
        setDefaultB();
        message += "Observation symbol matrix\n";
    }

    if( message != "" )
        QMessageBox::warning( 0, "Number of states changed",
                                 "Will reset the following structures "
                                 "to their default values:\n" + message );
}

/**
 * When user changes the number of symbolss, unselect user B
 */              
void HMMParameters::resetSymbolStructures( int )
{
    if( gb_B->selectedId() == B_custom )
    {
        setDefaultB();
        QMessageBox::warning( 0, "Number of symbols changed",
                              "Will reset the observation symbol matrix to its "
                              "default value" );    
    }
}

/**
 * Open vector pi editor
 */
void HMMParameters::openPiEditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    pi,
                                                    "Editing initial state "
                                                    "distribution vector" );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeVector( pi, sb_N->value() );
        matrix_editor->getVector( pi );
    }
    else
        setDefaultPi();

    delete matrix_editor;
}

/**
 * Open matrix A editor
 */
void HMMParameters::openAEditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    sb_N->value(),
                                                    A,
                                                    "Editing state transition "
                                                    "matrix" );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeMatrix( A, sb_N->value(), sb_N->value() );
        matrix_editor->getMatrix( A );
    }
    else
        setDefaultA();

    delete matrix_editor;
}

void HMMParameters::openQBDParameters( void )
{
    QBDParameters *parameters = new QBDParameters( qbd_parameters,
                                                   sb_N->value() );
    
    if( parameters->exec() != QDialog::Accepted )
    {
        setDefaultA();
    }

    delete parameters;    
}

/**
 * Open matrix B editor
 */
void HMMParameters::openBEditor( void )
{
    MatrixEditor *matrix_editor = new MatrixEditor( main_window,
                                                    sb_N->value(), 
                                                    sb_M->value(),
                                                    B,
                                                    "Editing observation symbol"
                                                    " matrix" );
    if( matrix_editor->exec() == QDialog::Accepted )
    {
        resizeMatrix( B, sb_N->value(), sb_M->value() );
        matrix_editor->getMatrix( B );
    }
    else
        setDefaultB();

    delete matrix_editor;
}

void HMMParameters::setDefaultPi( void )
{
    rb_pi_random->setChecked( true );
}

void HMMParameters::setDefaultA( void )
{
    rb_A_full->setChecked( true );
}

void HMMParameters::setDefaultB( void )
{
    rb_B_random->setChecked( true );
}

QBDParameterValues *HMMParameters::getQBDParameter( void )
{
    return qbd_parameters;
}

int HMMParameters::getNumberOfStates( void )
{
    return sb_N->value();
}

int HMMParameters::getNumberOfSymbols( void )
{
    return sb_M->value();
}

HMMParameters::InitialDistributionType
    HMMParameters::getInitialDistributionType( void )
{
    return (HMMParameters::InitialDistributionType)gb_pi->selectedId();
}

double *HMMParameters::getInitialDistribution( void )
{
    return pi;
}

HMMParameters::TransitionMatrixType
    HMMParameters::getTransitionMatrixType( void )
{
    return (HMMParameters::TransitionMatrixType)gb_A->selectedId();
}

double **HMMParameters::getTransitionMatrix( void )
{
    return A;
}

HMMParameters::ObservationMatrixType
    HMMParameters::getObservationMatrixType( void )
{
    return (HMMParameters::ObservationMatrixType)gb_B->selectedId();
}

double **HMMParameters::getObservationMatrix( void )
{
    return B;
}

/****************************
 * Class QBDParameterValues *
 ****************************/
QBDParameterValues::QBDParameterValues( void )
{
    null = true;
}

bool QBDParameterValues::isNull( void )
{
    return null;
}
        
void QBDParameterValues::setParameters( int groups,
                                        bool has_back, int back,
                                        bool has_fwd,  int fwd )
{
    null = false;

    this->groups   = groups;
    this->has_back = has_back;     
    this->back     = back;         
    this->has_fwd  = has_fwd;
    this->fwd      = fwd;
}
