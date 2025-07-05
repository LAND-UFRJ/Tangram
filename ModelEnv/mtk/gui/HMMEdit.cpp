#include "HMMEdit.h"

#include <string>

#include <qmessagebox.h>
#include <qfiledialog.h> 

HMMEdit::HMMEdit( Object *object, MainWindow *main_window, bool read_only )
        :QDialog( 0, 0, true, 0 )
{
    this->object      = object;
    this->main_window = main_window;
    this->read_only   = read_only; 
}

void HMMEdit::buildInterface( void )
{
    buildCaption();

    window_layout = new QVBoxLayout( this, 5 );

    fm_parameters = new QFrame( this );

    buildN();      
    buildM();    
    buildPi();    
    buildA();    
    buildB();

    buildLayout();
    
    buildButtons();
    
    fillStructures();
}

void HMMEdit::buildCaption( void )
{
    // Set caption
    QString caption;    
    
    if( read_only )
        caption = "Displaying object '";
    else
        caption = "Editing object '";

    caption += QString( object->getName() ) + "'";

    setCaption( caption );
}

void HMMEdit::buildN( void )
{
    std::string value;
    Index       index;

    // Number of states
    lbl_N  = new QLabel( "Number of states:", fm_parameters, "lbl_N"  );
    le_N   = new QLineEdit( fm_parameters, "le_N" );
    le_N->setReadOnly( true );  // user cannot change model dimensions
    
    // Get 'n' value
    object->get( "N", value, index );
    n = QString( value ).toInt();
    le_N->setText( QString( value ) );    
}

void HMMEdit::buildM( void )
{
    std::string value;
    Index       index;
    
    // Number of symbols
    lbl_M  = new QLabel( "Number of symbols:", fm_parameters, "lbl_M"  );
    le_M   = new QLineEdit( fm_parameters, "le_M" );
    le_M->setReadOnly( true );  // user cannot change model dimensions    

    // Get 'm' value
    object->get( "M", value, index );
    m = QString( value ).toInt();
    le_M->setText( QString( value ) ); 
}

void HMMEdit::buildPi( void )
{
    // Initial state distribution
    gb_pi  = new QVGroupBox( "Initial state distribution", fm_parameters );
    tb_pi  = new Matrix( gb_pi, 1, n );
    tb_pi->setReadOnly( read_only );

    if( !read_only )
    {
        bt_load_pi = new QPushButton( "Load", gb_pi );
        connect( bt_load_pi, SIGNAL( clicked() ), 
                 this, SLOT( loadPi() ) );    
    }
}

void HMMEdit::buildA( void )
{
    // State transition matrix
    gb_A  = new QVGroupBox( "State transition matrix", fm_parameters );
    tb_A  = new Matrix( gb_A, n, n );
    tb_A->setReadOnly( read_only );      

    if( !read_only )
    {
        bt_load_A = new QPushButton( "Load", gb_A );
        connect( bt_load_A, SIGNAL( clicked() ), 
                 this, SLOT( loadA() ) );    
    }
}

void HMMEdit::buildB( void )
{
    // Observation symbol matrix
    gb_B  = new QVGroupBox( "Observation symbol matrix", fm_parameters );
    tb_B  = new Matrix( gb_B, n, m );
    tb_B->setReadOnly( read_only ); 

    if( !read_only )
    {
        bt_load_B = new QPushButton( "Load", gb_B );
        connect( bt_load_B, SIGNAL( clicked() ), 
                 this, SLOT( loadB() ) );    
    }
}

void HMMEdit::buildLayout( void )
{
    // Add widgets to layout
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 5 );     
    
    parameters_layout->addWidget( lbl_N,  0, 0 );
    parameters_layout->addWidget( le_N,   0, 1 );
    parameters_layout->addWidget( lbl_M,  1, 0 );
    parameters_layout->addWidget( le_M,   1, 1 );
    parameters_layout->addMultiCellWidget( gb_pi, 2, 2, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,  3, 3, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_B,  4, 4, 0, 1 );        
    
    window_layout->addWidget( fm_parameters );
}

void HMMEdit::buildButtons( void )
{
    ////////// Buttons //////////
    buttons_layout = new QHBoxLayout( window_layout, 5 );

    if( read_only )
    {
        bt_close  = new QPushButton( "Close", this, "bt_close" );
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
                 this, SLOT( checkStructures() ) );
        connect( bt_cancel, SIGNAL( clicked() ), 
                 this, SLOT( reject() ) );

        buttons_layout->addWidget( bt_save );
        buttons_layout->addWidget( bt_cancel );
    }
}

void HMMEdit::fillStructures( void )
{
    fillPi();
    fillA();
    fillB();        
}

void HMMEdit::fillPi( void )
{
    Index       index;
    std::string value;

    // Fill pi vector
    for( int j = 0; j < n; j++ )
    {
        index.push_back( j );
        
        object->get( "pi", value, index );
        tb_pi->setCell( 0, j, value );
        
        index.clear();
    }
}

void HMMEdit::fillA( void )
{
    Index       index;
    std::string value;

    // Fill A matrix
    for( int i = 0; i < n; i++ )
    {
        for( int j = 0; j < n; j++ )
        {
            index.push_back( i );
            index.push_back( j );

            object->get( "A", value, index );
            tb_A->setCell( i, j, value );

            index.clear();
        }
    }
}

void HMMEdit::fillB( void )
{
    Index       index;
    std::string value;

    // Fill B matrix
    for( int i = 0; i < n; i++ )
    {
        for( int j = 0; j < m; j++ )
        {
            index.push_back( i );
            index.push_back( j );

            object->get( "B", value, index );
            tb_B->setCell( i, j, value );

            index.clear();
        }
    }
}

/**
 * Save matrices entries into the object
 */
void HMMEdit::saveStructures( void )
{
    savePi();
    saveA();
    saveB();
}

void HMMEdit::savePi( void )
{
    Index index;

    // Save pi
    for( int i = 0; i < n; i++ )
    {
        index.push_back( i );

        object->set( "pi", QString( "%1" ).arg( 
                     tb_pi->getCell( 0, i ) ).ascii(), index );
        index.clear();
    }   
}

void HMMEdit::saveA( void )
{
    Index index;

    // Save A
    for( int i = 0; i < n; i++ )
    {
        for( int j = 0; j < n; j++ )
        {
            index.push_back( i );
            index.push_back( j );            

            object->set( "A", QString( "%1" ).arg( 
                         tb_A->getCell( i, j ) ).ascii(), index );
            index.clear();
        }
    }
}

void HMMEdit::saveB( void )
{
    Index index;

    // Save B
    for( int i = 0; i < n; i++ )
    {
        for( int j = 0; j < m; j++ )
        {
            index.push_back( i );
            index.push_back( j );            

            object->set( "B", QString( "%1" ).arg( 
                         tb_B->getCell( i, j ) ).ascii(), index );
            index.clear();
        }
    }
}

/**
 * Checks if all matrices respect the stochastic constrains
 */
void HMMEdit::checkStructures( void )
{
    bool status_pi = tb_pi->isSthocastic();
    bool status_A  = tb_A->isSthocastic();
    bool status_B  = tb_B->isSthocastic();

    if( status_pi && status_A && status_B )
    {
        saveStructures();
        accept();
    }
    else
    {
        int answer = QMessageBox::question( this, "Not normalized rows",
                                            "At least one of the matrix rows "
                                            "does "
                                            "not sum one.\nDo you want "
                                            "to normalize these rows?",
                                            QMessageBox::Yes | 
                                            QMessageBox::Default,
                                            QMessageBox::No );
        if( answer == QMessageBox::Yes )
        {
            if( !status_pi )
                tb_pi->makeSthocastic();

            if( !status_A )
                tb_A->makeSthocastic();
                
            if( !status_B )
                tb_B->makeSthocastic();                
        }
    }  
}

/**
 * Loads initial state distribution vector from a file
 */
void HMMEdit::loadPi( void )
{
    // Open dialog
    QString file_name = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file_name != "" )
    {
        Arguments args;
        Token file, param;
        
        file.type = TK_STRING;
        file.text = file_name.ascii();
        args.push_back( file );
        
        param.type = TK_STRING;
        param.text = QString( "pi" ).ascii();
        args.push_back( param );

        object->exec( "load", args );
        fillPi();
    }                                                    

    main_window->setCurrentDir( file_name ); 
}

/**
 * Loads state transition matrix from a file
 */
void HMMEdit::loadA( void )
{
    // Open dialog
    QString file_name = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file_name != "" )
    {
        Arguments args;
        Token file, param;
        
        file.type = TK_STRING;
        file.text = file_name.ascii();
        args.push_back( file );
        
        param.type = TK_STRING;
        param.text = QString( "A" ).ascii();
        args.push_back( param );

        object->exec( "load", args );
        fillA();
    }                                                    

    main_window->setCurrentDir( file_name ); 
}

/**
 * Loads observation symbol matrix vector from a file
 */
void HMMEdit::loadB( void )
{
    // Open dialog
    QString file_name = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file_name != "" )
    {
        Arguments args;
        Token file, param;
        
        file.type = TK_STRING;
        file.text = file_name.ascii();
        args.push_back( file );
        
        param.type = TK_STRING;
        param.text = QString( "B" ).ascii();
        args.push_back( param );

        object->exec( "load", args );
        fillB();
    }                                                    

    main_window->setCurrentDir( file_name ); 
}
