#include "IntValueActions.h"

#include <qmessagebox.h>
#include <qfiledialog.h>

IntValueActions::IntValueActions( QWidget *parent, Object *object )
                :QGroupBox( 1, Qt::Horizontal,
                            "IntValue object actions", parent )
{
    this->object      = object;
    this->main_window = (MainWindow *)parent;

    buildInterface();

    hide();
}

void IntValueActions::buildInterface( void )
{
    tb_actions = new QTabWidget( this );

    buildLoadFrame();
    buildSaveFrame();    
    buildTruncateFrame();
}

void IntValueActions::buildLoadFrame( void )
{
    fm_load         = new QFrame( this );
    load_layout     = new QGridLayout( fm_load, 4, 2, 5 );    
    gb_load_file    = new QGroupBox( 2, Qt::Horizontal,
                                     "File", fm_load );
    le_load_file    = new QLineEdit( gb_load_file );
    bt_load_file    = new QPushButton( "Choose file", gb_load_file );
    connect( bt_load_file, SIGNAL( clicked() ), 
             this, SLOT( chooseLoadFile() ) );     
    lbl_load_offset = new QLabel( "Offset:", fm_load );
    sb_load_offset  = new QSpinBox( 0, INT_MAX, 1, fm_load );
    gb_load_size    = new QGroupBox( 2, Qt::Horizontal, 
                                     "Partial load", fm_load );
    gb_load_size->setCheckable( true );
    gb_load_size->setChecked( false );
    lbl_load_size   = new QLabel( "Size:", gb_load_size );
    sb_load_size    = new QSpinBox( 0, INT_MAX, 1, gb_load_size );
    bt_load         = new QPushButton( "Load", fm_load );
    connect( bt_load, SIGNAL( clicked() ), 
             this, SLOT( load() ) ); 

    load_layout->addMultiCellWidget( gb_load_file, 0, 0, 0, 1 );
    load_layout->addWidget( lbl_load_offset, 1, 0 );    
    load_layout->addWidget( sb_load_offset,  1, 1 );  
    load_layout->addMultiCellWidget( gb_load_size,  2, 2, 0, 1 );
    load_layout->addMultiCellWidget( bt_load,       3, 3, 0, 1 );

    tb_actions->addTab( fm_load, "Load" );
    tb_actions->setTabToolTip( fm_load, "Loads a sample from a file" );
}

void IntValueActions::buildSaveFrame( void )
{
    fm_save         = new QFrame( this );
    save_layout     = new QGridLayout( fm_save, 4, 2, 5 );    
    gb_save_file    = new QGroupBox( 2, Qt::Horizontal,
                                     "File", fm_save );
    le_save_file    = new QLineEdit( gb_save_file );
    bt_save_file    = new QPushButton( "Choose file", gb_save_file );
    connect( bt_save_file, SIGNAL( clicked() ), 
             this, SLOT( chooseSaveFile() ) );     
    lbl_save_offset = new QLabel( "Offset:", fm_save );
    sb_save_offset  = new QSpinBox( 0, INT_MAX, 1, fm_save );
    gb_save_size    = new QGroupBox( 2, Qt::Horizontal, 
                                     "Partial save", fm_save );
    gb_save_size->setCheckable( true );
    gb_save_size->setChecked( false );
    lbl_save_size   = new QLabel( "Size:", gb_save_size );
    sb_save_size    = new QSpinBox( 0, INT_MAX, 1, gb_save_size );
    bt_save         = new QPushButton( "Save", fm_save );
    connect( bt_save, SIGNAL( clicked() ), 
             this, SLOT( save() ) ); 

    save_layout->addMultiCellWidget( gb_save_file, 0, 0, 0, 1 );
    save_layout->addWidget( lbl_save_offset, 1, 0 );    
    save_layout->addWidget( sb_save_offset,  1, 1 );  
    save_layout->addMultiCellWidget( gb_save_size,  2, 2, 0, 1 );
    save_layout->addMultiCellWidget( bt_save,       3, 3, 0, 1 );
    
    tb_actions->addTab( fm_save, "Save" );    
    tb_actions->setTabToolTip( fm_save, "Stores a sample in a file" );    
}

void IntValueActions::buildTruncateFrame( void )
{
    std::string value;
    Index       index;

    fm_truncate         = new QFrame( this );
    truncate_layout     = new QGridLayout( fm_truncate, 3, 2, 5 );

    int_validator       = new QIntValidator( INT_MIN, INT_MAX, this );
    
    lbl_lower           = new QLabel( "Lower bound:", fm_truncate );
    le_lower            = new QLineEdit( fm_truncate );
    object->get( "lower", value, index );
    le_lower->setText( QString( value ) );    
    le_lower->setValidator( int_validator );

    lbl_upper           = new QLabel( "Upper bound:", fm_truncate );
    le_upper            = new QLineEdit( fm_truncate );
    object->get( "upper", value, index );
    le_upper->setText( QString( value ) ); 
    le_upper->setValidator( int_validator );
    
    bt_truncate         = new QPushButton( "Truncate", fm_truncate );
    connect( bt_truncate, SIGNAL( clicked() ), 
             this, SLOT( truncate() ) );
                      
    truncate_layout->addWidget( lbl_lower, 0, 0 );
    truncate_layout->addWidget( le_lower,  0, 1 );    
    truncate_layout->addWidget( lbl_upper, 1, 0 );
    truncate_layout->addWidget( le_upper,  1, 1 );    
    truncate_layout->addMultiCellWidget( bt_truncate, 2, 2, 0, 1 );
    
    tb_actions->addTab( fm_truncate, "Truncate" );
    tb_actions->setTabToolTip( fm_truncate,
                               "Truncate sample values between two values" );
}

void IntValueActions::chooseLoadFile( void )
{
    QString file = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file != "" )
    {
        le_load_file->setText( file );
        
        main_window->setCurrentDir( file );
    }
}

void IntValueActions::load( void )
{
    if( le_load_file->text() == "" )
    {
        QMessageBox::warning( 0, "No file selected",
                              QString( "Please, select a file to load." ) );
    }
    else
    {    
        Arguments args;
        Token     tk_file, tk_offset, tk_size;

        tk_file.type = TK_STRING;
        tk_file.text = le_load_file->text().ascii();
        args.push_back( tk_file );    

        tk_offset.type = TK_INTEGER;
        tk_offset.text = QString( "%1" ).arg( sb_load_offset->value() ).ascii();
        args.push_back( tk_offset );    

        if( gb_load_size->isChecked() )
        {
            tk_size.type = TK_INTEGER;
            tk_size.text = QString( "%1" ).arg( sb_load_size->value() ).ascii();
            args.push_back( tk_size );
        }

        object->exec( "load", args );
    }
}

void IntValueActions::chooseSaveFile( void )
{
    QString file = QFileDialog::getSaveFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file != "" )
    {
        le_save_file->setText( file );
        main_window->setCurrentDir( file );
    }
}

void IntValueActions::save( void )
{
    if( le_save_file->text() == "" )
    {
        QMessageBox::warning( 0, "No file selected",
                              QString( "Please, select a file to save." ) );
    }
    else
    {
        Arguments args;
        Token     tk_file, tk_offset, tk_size;

        tk_file.type = TK_STRING;
        tk_file.text = le_save_file->text().ascii();
        args.push_back( tk_file );    

        tk_offset.type = TK_INTEGER;
        tk_offset.text = QString( "%1" ).arg( sb_save_offset->value() ).ascii();
        args.push_back( tk_offset );    

        if( gb_save_size->isChecked() )
        {
            tk_size.type = TK_INTEGER;
            tk_size.text = QString( "%1" ).arg( sb_save_size->value() ).ascii();
            args.push_back( tk_size );
        }

        object->exec( "save", args );
    }
}

void IntValueActions::truncate( void )
{
    Arguments args;
    Token     tk_lower, tk_upper;
    
    tk_lower.type = TK_INTEGER;
    tk_lower.text = le_lower->text().ascii();
    args.push_back( tk_lower );    

    tk_upper.type = TK_INTEGER;
    tk_upper.text = le_upper->text().ascii();
    args.push_back( tk_upper );
    
    object->exec( "truncate", args );              
}
