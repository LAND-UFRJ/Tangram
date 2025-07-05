#include "HMMBatchVariableEdit.h"

#include <qmessagebox.h>
#include <qfiledialog.h> 

HMMBatchVariableEdit::HMMBatchVariableEdit( Object     *object, 
                                            MainWindow *main_window,
                                            bool        read_only )
                     :GHMMEdit( object, main_window, read_only )
{
}

void HMMBatchVariableEdit::buildInterface( void )
{
    buildCaption();

    window_layout = new QVBoxLayout( this, 5 );

    fm_parameters = new QFrame( this );

    buildN();      
    buildM();          
    buildPi();    
    buildA();
    buildObs();

    buildLayout();
    
    buildButtons();

    fillStructures();
}

void HMMBatchVariableEdit::buildLayout( void )
{
    // Add widgets to layout
    parameters_layout = new QGridLayout( fm_parameters, 5, 2, 5 );     
    
    parameters_layout->addWidget( lbl_N,           0, 0 );
    parameters_layout->addWidget( le_N,            0, 1 );
    parameters_layout->addWidget( lbl_M,           1, 0 );
    parameters_layout->addWidget( le_M,            1, 1 );
    parameters_layout->addMultiCellWidget( gb_pi,  2, 2, 0, 1 );
    parameters_layout->addMultiCellWidget( gb_A,   3, 3, 0, 1 );    
    parameters_layout->addMultiCellWidget( gb_obs, 4, 4, 0, 1 );     
    
    window_layout->addWidget( fm_parameters );
}

void HMMBatchVariableEdit::buildObs( void )
{
    gb_obs = new QVGroupBox( "Observation parameters. Last symbol represents end of batch.", fm_parameters );

    // r matrix
    lbl_obs_r = new QLabel( "Initial probabilities of symbols at each state:",
                            gb_obs );
    tb_obs_r  = new Matrix( gb_obs, n, m );
    tb_obs_r->setReadOnly( read_only );                                

    // p matrix
    lbl_obs_p = new QLabel( "Symbol transition matrices at each state:",
                            gb_obs );

    tb_state  = new QTabWidget( gb_obs );
    
    // Build each 3d matrix slice at a frame
    for( int i = 0; i < n; i++ )
    {
        QFrame      *new_frame    = new QFrame( this );
        QVBoxLayout *frame_layout = new QVBoxLayout( new_frame, 5 );       
        
        Matrix *new_matrix = new Matrix( new_frame, m, m );
        new_matrix->setReadOnly( read_only );
        
        matrix_list.push_back( new_matrix );        
        frame_layout->addWidget( new_matrix );
        
        tb_state->addTab( new_frame, QString( "State %1" ).arg( i + 1 ) );
    }
    
    if( !read_only )
    {
        bt_load_obs = new QPushButton( "Load", gb_obs );
        connect( bt_load_obs, SIGNAL( clicked() ), 
                 this, SLOT( loadObs() ) );    
    }      
}

void HMMBatchVariableEdit::fillStructures( void )
{
    fillPi();
    fillA();
    fillR();
    fillP();    
}

void HMMBatchVariableEdit::fillR( void )
{
    Index       index;
    std::string value;

    // Fill r matrix
    for( int i = 0; i < n; i++ )
        for( int j = 0; j < m; j++ )    
        {
            index.push_back( i );
            index.push_back( j );

            object->get( "r", value, index );
            tb_obs_r->setCell( i, j, value );

            index.clear();
        }
}

void HMMBatchVariableEdit::fillP( void )
{
    Index       index;
    std::string value;

    // For each state
    int i = 0;
    for( MatrixList::iterator it = matrix_list.begin(); 
         it != matrix_list.end();
         ++it, i++ )
    {
        for( int j = 0; j < m; j++ )
            for( int k = 0; k < m; k++ )
            {
                index.push_back( i );
                index.push_back( j );                
                index.push_back( k );            

                object->get( "p", value, index );            
                (*it)->setCell( j, k, value );

                index.clear();
            }
    }
}

void HMMBatchVariableEdit::saveStructures( void )
{
    savePi();
    saveA();
    saveR();
    saveP();
}

void HMMBatchVariableEdit::saveR( void )
{
    Index index;

    // Save r matrix
    for( int i = 0; i < n; i++ )
        for( int j = 0; j < m; j++ )    
        {
            index.push_back( i );
            index.push_back( j );            

            object->set( "r", QString( "%1" ).arg( 
                         tb_obs_r->getCell( i, j ) ).ascii(), index );
            index.clear();
        }
}


void HMMBatchVariableEdit::saveP( void )
{
    Index index;

    // For each state
    int i = 0;
    for( MatrixList::iterator it = matrix_list.begin(); 
         it != matrix_list.end();
         ++it, i++ )
    {
        for( int j = 0; j < m; j++ )
            for( int k = 0; k < m; k++ )
            {
                index.push_back( i );
                index.push_back( j );                
                index.push_back( k );            

                object->set( "p", QString( "%1" ).arg( 
                             (*it)->getCell( j, k ) ).ascii(), index );

                index.clear();
            }
    }
}

void HMMBatchVariableEdit::checkStructures( void )
{
    bool status_pi = tb_pi->isSthocastic();
    bool status_A  = tb_A->isSthocastic();
    bool status_r  = tb_obs_r->isSthocastic();
    bool status_p  = true;
    for( MatrixList::iterator it = matrix_list.begin(); 
         status_p && ( it != matrix_list.end() );
         ++it )
        status_p = (*it)->isSthocastic();

    if( status_pi && status_A && status_r && status_p )
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
                
            if( !status_r )
                tb_obs_r->makeSthocastic();               

            if( !status_p )
                for( MatrixList::iterator it = matrix_list.begin(); 
                     it != matrix_list.end();
                     ++it )
                    (*it)->makeSthocastic();
        }
    }
}

/**
 * Load observation parameters from a file
 */
void HMMBatchVariableEdit::loadObs( void )
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
    }                                                    

    main_window->setCurrentDir( file_name ); 
}
