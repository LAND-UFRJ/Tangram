#include "QBDParameters.h"

#include <qmessagebox.h>

QBDParameters::QBDParameters( QBDParameterValues *values, int n )
             :QDialog( 0, 0, true, 0 )
{
    this->values = values;
    this->n      = n;

    buildInterface();
    
    // If old values were specified, fill them into the form
    if( !values->isNull() )
    {
        sb_groups->setValue( values->groups   );
        gb_back->setChecked( values->has_back );
        sb_back->setValue  ( values->back     );
        gb_fwd->setChecked ( values->has_fwd  );
        sb_fwd->setValue   ( values->fwd      );
    }
}

void QBDParameters::buildInterface( void )
{
    setCaption( "Quasi-birth-death parameters" );

    window_layout = new QVBoxLayout( this, 5 );
 
    fm_parameters = new QFrame( this );
    parameters_layout = new QGridLayout( fm_parameters, 2, 2, 5 );    

    ////////// Input //////////
    lbl_groups = new QLabel( "Number of groups of states:", fm_parameters, 
                             "lbl_groups" );
    sb_groups  = new QSpinBox( fm_parameters, "sb_groups" );
    sb_groups->setMinValue( 1 );

    gb_back = new QGroupBox( 2, Qt::Vertical, 
                             "Number of reachable back groups", 
                             fm_parameters, "gb_back" );
    gb_back->setCheckable( true );
    gb_back->setChecked( false );
    sb_back = new QSpinBox( gb_back, "sb_back" );
    sb_back->setMinValue( 1 );                                      

    gb_fwd = new QGroupBox( 1, Qt::Vertical, 
                            "Number of reachable forward groups", 
                            gb_back, "gb_fwd" );
    gb_fwd->setCheckable( true );
    gb_fwd->setChecked( false );
    sb_fwd = new QSpinBox( gb_fwd, "sb_fwd" );
    sb_fwd->setMinValue( 1 );

    parameters_layout->addWidget( lbl_groups, 0, 0 );
    parameters_layout->addWidget( sb_groups,  0, 1 );
    parameters_layout->addMultiCellWidget( gb_back, 1, 1, 0, 1 );

    window_layout->addWidget( fm_parameters );
   
    ////////// Buttons //////////    
    buttons_layout = new QHBoxLayout( window_layout, 5 );
    
    bt_ok     = new QPushButton( "Ok",     this, "bt_save"   );
    bt_cancel = new QPushButton( "Cancel", this, "bt_cancel" );
    
    connect( bt_ok, SIGNAL( clicked() ), 
             this, SLOT( setParameters() ) );
    connect( bt_cancel, SIGNAL( clicked() ), 
             this, SLOT( reject() ) );
    
    buttons_layout->addWidget( bt_ok     );
    buttons_layout->addWidget( bt_cancel );
//    buttons_layout->setAlignment( Qt::AlignHCenter );

//    window_layout->setAlignment( Qt::AlignHCenter );

    adjustSize();    
}

void QBDParameters::setParameters( void )
{
    bool status   = true;
    int  groups   = sb_groups->value();
    bool has_back = gb_back->isChecked();
    int  back     = sb_back->value();
    bool has_fwd  = gb_fwd->isChecked();
    int  fwd      = sb_fwd->value();
    
    // Number of groups must divide n
    if( n % groups )
    {
        QMessageBox::warning( 0, "Invalid number of groups",
                                 "The number of groups must divide the number "
                                 "of states." );
        sb_groups->setFocus();
        status = false;                               
    }
    
    // Back and fwd must be <= groups
    if( status && has_back && ( back > groups ) )
    {
        QMessageBox::warning( 0, "Invalid number of back groups",
                                 "The number of back groups must be less or "
                                 "equal the number of groups." );
        sb_back->setFocus();
        status = false;
    }

    if( status && has_fwd && ( fwd > groups ) )
    {
        QMessageBox::warning( 0, "Invalid number of forward groups",
                                 "The number of forward groups must be less or "
                                 "equal the number of groups." );
        sb_fwd->setFocus();
        status = false;
    }

    if( status )
    {
        values->setParameters( groups,
                               gb_back->isChecked(), back,
                               gb_fwd->isChecked(),  fwd );
        accept();
    }
}

int QBDParameters::getGroups( void )
{
    return sb_groups->value();
}

bool QBDParameters::hasBack( void )
{
    return gb_back->isChecked();
}

int QBDParameters::getBack( void )
{
    return sb_back->value();
}

bool QBDParameters::hasForward( void )
{
    return gb_fwd->isChecked();
}

int QBDParameters::getForward( void ) 
{
    return sb_fwd->value();
}
