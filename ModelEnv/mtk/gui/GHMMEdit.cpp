#include "GHMMEdit.h"

#include <qmessagebox.h>
#include <qfiledialog.h> 

GHMMEdit::GHMMEdit( Object *object, MainWindow *main_window, bool read_only )
         :HMMEdit( object, main_window, read_only )
{
}

void GHMMEdit::buildInterface( void )
{
    buildCaption();

    window_layout = new QVBoxLayout( this, 5 );

    fm_parameters = new QFrame( this );

    buildN();      
    buildBatchSize();    
    buildPi();    
    buildA();    
    buildObs();

    buildLayout();
    
    buildButtons();
    
    fillStructures();
}

void GHMMEdit::buildBatchSize( void )
{
    std::string value;
    Index       index;

    lbl_batch_size = new QLabel( "Batch size:", fm_parameters );
    le_batch_size  = new QLineEdit( fm_parameters );
    le_batch_size->setReadOnly( true );  // user cannot change model dimensions

    // Get 'batch_size' value
    object->get( "B", value, index );
    batch_size = QString( value ).toInt();
    le_batch_size->setText( QString( value ) ); 
}

void GHMMEdit::buildObs( void )
{
    gb_obs = new QVGroupBox( "Observation parameters", fm_parameters );

    // r vector
    lbl_obs_r = new QLabel( "Initial probabilities of symbol 1:", gb_obs );
    tb_obs_r  = new Matrix( gb_obs, 1, n );
    tb_obs_r->setReadOnly( read_only );    

    // p vector
    lbl_obs_p = new QLabel( "Symbol 0 to 1 transition probabilities:", gb_obs );
    tb_obs_p  = new Matrix( gb_obs, 1, n );
    tb_obs_p->setReadOnly( read_only );  
    
    // q vector
    lbl_obs_q = new QLabel( "Symbol 1 to 0 transition probabilities:", gb_obs );
    tb_obs_q  = new Matrix( gb_obs, 1, n );
    tb_obs_q->setReadOnly( read_only );   

    if( !read_only )
    {
        bt_load_obs = new QPushButton( "Load", gb_obs );
        connect( bt_load_obs, SIGNAL( clicked() ), 
                 this, SLOT( loadObs() ) );    
    }    
}

void GHMMEdit::buildLayout( void )
{
    // Add widgets to layout
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 5 );     
    
    parameters_layout->addWidget( lbl_N,           0, 0 );
    parameters_layout->addWidget( le_N,            0, 1 );
    parameters_layout->addWidget( lbl_batch_size,  1, 0 );
    parameters_layout->addWidget( le_batch_size,   1, 1 );
    parameters_layout->addMultiCellWidget( gb_pi,  2, 2, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,   3, 3, 0, 1 );    
    parameters_layout->addMultiCellWidget( gb_obs, 4, 4, 0, 1 );
    
    window_layout->addWidget( fm_parameters );
}
        
void GHMMEdit::fillStructures( void )
{
    fillPi();
    fillA();
    fillR();
    fillP();    
    fillQ();
}

void GHMMEdit::fillR( void )
{
    Index       index;
    std::string value;

    // Fill r vector
    for( int j = 0; j < n; j++ )
    {
        index.push_back( j );
        
        object->get( "r", value, index );
        tb_obs_r->setCell( 0, j, value );
        
        index.clear();
    }
}


void GHMMEdit::fillP( void )
{
    Index       index;
    std::string value;

    // Fill p vector
    for( int j = 0; j < n; j++ )
    {
        index.push_back( j );
        
        object->get( "p", value, index );
        tb_obs_p->setCell( 0, j, value );
        
        index.clear();
    }
}

void GHMMEdit::fillQ( void )
{
    Index       index;
    std::string value;

    // Fill q vector
    for( int j = 0; j < n; j++ )
    {
        index.push_back( j );
        
        object->get( "q", value, index );
        tb_obs_q->setCell( 0, j, value );
        
        index.clear();
    }
}

void GHMMEdit::saveStructures( void )
{
    savePi();
    saveA();
    saveR();
    saveP();
    saveQ();        
}

void GHMMEdit::saveR( void )
{
    Index index;

    // Save r
    for( int i = 0; i < n; i++ )
    {
        index.push_back( i );

        object->set( "r", QString( "%1" ).arg( 
                     tb_obs_r->getCell( 0, i ) ).ascii(), index );
        index.clear();
    } 
}


void GHMMEdit::saveP( void )
{
    Index index;

    // Save p
    for( int i = 0; i < n; i++ )
    {
        index.push_back( i );

        object->set( "p", QString( "%1" ).arg( 
                     tb_obs_p->getCell( 0, i ) ).ascii(), index );
        index.clear();
    } 
}


void GHMMEdit::saveQ( void )
{
    Index index;

    // Save q
    for( int i = 0; i < n; i++ )
    {
        index.push_back( i );

        object->set( "q", QString( "%1" ).arg( 
                     tb_obs_q->getCell( 0, i ) ).ascii(), index );
        index.clear();
    } 
}
        
void GHMMEdit::checkStructures( void )
{
    bool status_pi = tb_pi->isSthocastic();
    bool status_A  = tb_A->isSthocastic();
    bool status_r  = tb_obs_r->isIndependentProb();
    bool status_p  = tb_obs_p->isIndependentProb();
    bool status_q  = tb_obs_q->isIndependentProb();        

    if( status_pi && status_A && status_r && status_p && status_q )
    {
        saveStructures();
        accept();
    }
    // Check stochastic matrices
    else if( !status_pi || !status_A )
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
        }
    }
    // Check observation vectors        
    else if( !status_r || !status_p || !status_q )
    {
        QMessageBox::warning( 0, "Some cell is not a probability!",
                              QString( "All cell values must to be "
                                       "between 0 and 1" ) );
    }
}

/**
 * Load observation parameters from a file
 */
void GHMMEdit::loadObs( void )
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
        
        param.type = TK_IDENTIFIER;
        param.text = QString( "obs" ).ascii();
        args.push_back( param );

        object->exec( "load", args );
        fillR();
        fillP();
        fillQ();                
    }                                                    

    main_window->setCurrentDir( file_name ); 
}
