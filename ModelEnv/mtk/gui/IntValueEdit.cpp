#include "IntValueEdit.h"

#include <qmessagebox.h>

IntValueEdit::IntValueEdit( Object *object, bool read_only )
             :QDialog( 0, 0, true, 0 )
{
    this->object    = object;
    this->read_only = read_only; 
    
    buildInterface();
}

void IntValueEdit::buildInterface( void )
{
    std::string value;
    Index       index;

    // Set caption    
    QString caption;    

    if( read_only )
        caption = "Displaying object '";
    else
        caption = "Editing object '";

    caption += QString( object->getName() ) + "'";
    
    setCaption( caption );

    window_layout = new QVBoxLayout( this, 5 );

    fm_parameters = new QFrame( this );
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 5 );

    // Get bounds
    lbl_lower = new QLabel( "Lower bound:", fm_parameters );
    parameters_layout->addWidget( lbl_lower, 0, 0 );

    object->get( "lower", value, index );

    int_validator = new QIntValidator( INT_MIN, INT_MAX, this );

    le_lower = new QLineEdit( fm_parameters );
    le_lower->setReadOnly( read_only );
    le_lower->setText( QString( value ) );
    le_lower->setValidator( int_validator );
    
    parameters_layout->addWidget( le_lower, 0, 1 );

    lbl_upper = new QLabel( "Upper bound:", fm_parameters );
    parameters_layout->addWidget( lbl_upper, 1, 0 );

    object->get( "upper", value, index );

    le_upper = new QLineEdit( fm_parameters );
    le_upper->setReadOnly( read_only );
    le_upper->setText( QString( value ) );
    le_upper->setValidator( int_validator );
    
    parameters_layout->addWidget( le_upper, 1, 1 );

    // Get size
    lbl_size = new QLabel( "Size:", fm_parameters );
    parameters_layout->addWidget( lbl_size, 2, 0 );
    
    object->get( "size", value, index );
    le_size  = new QLineEdit( fm_parameters );
    le_size->setReadOnly( true );
    le_size->setText( QString( value ) );
    parameters_layout->addWidget( le_size, 2, 1 );
    
    // Get data
    lbl_data = new QLabel( "Data:", fm_parameters );
    parameters_layout->addWidget( lbl_data, 3, 0 );

    te_data  = new QTextEdit( fm_parameters );
    te_data->setReadOnly( read_only );
    te_data->setTextFormat( Qt::PlainText );
    parameters_layout->addMultiCellWidget( te_data, 4, 4, 0, 1 );

    int size = QString( value ).toInt();
    index.clear();
    for( int i = 0; i < size; i++ )
    {
        index.push_back( i );
    
        object->get( "data", value, index );
        te_data->append( QString( value ) );
        
        index.clear();
    }
    
    window_layout->addWidget( fm_parameters );
    
    ////////// Buttons //////////
    buttons_layout = new QHBoxLayout( window_layout, 5 );

    if( read_only )
    {
        bt_close = new QPushButton( "Close", this, "bt_close" );
        bt_close->setDefault( true );
        connect( bt_close, SIGNAL( clicked() ), 
                 this, SLOT( reject() ) );

        buttons_layout->addWidget( bt_close );
    }
    else    
    {
        bt_save   = new QPushButton( "Save",   this, "bt_save"   );
        bt_cancel = new QPushButton( "Cancel", this, "bt_cancel" );
        bt_save->setDefault( true );
        connect( bt_save, SIGNAL( clicked() ), 
                 this, SLOT( saveObject() ) );
        connect( bt_cancel, SIGNAL( clicked() ), 
                 this, SLOT( reject() ) );

        buttons_layout->addWidget( bt_save );
        buttons_layout->addWidget( bt_cancel );
    }                   
}

void IntValueEdit::saveObject( void )
{
    std::string value;
    Index       index;
    bool        status = true;

    // Save data
    int lines    = te_data->lines();
    QString text = te_data->text();
    for( int i = 0; i < lines; i++ )
    {
        index.push_back( i );
        
        value  = text.section( '\n', i, i ).ascii();
        status = object->set( "data", value, index );
       
        index.clear();
    }

    // Save bounds (must be after saving new values, because this operation may
    // truncate them)
    if( status )
    {
        value  = le_lower->text().ascii();
        status = object->set( "lower", value, index );

        value  = le_upper->text().ascii();
        status = object->set( "upper", value, index );
    }
    
    if( status )
        accept();
    else
        QMessageBox::warning( 0, "Invalid data",
                              QString( "The data field must be filled "
                                       "with one interger per line." ) );
}
