#include "MatrixEditor.h"

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qlayout.h>

/**
 * 3D matrix
 */ 
MatrixEditor::MatrixEditor( MainWindow *main_window,
                            int rows, int cols, int deep,
                            double ***matrix, 
                            QString title,
                            MatrixType matrix_type )
             :QDialog( 0, 0, true, 0 )
{
    this->main_window = main_window;
    this->rows        = rows;
    this->cols        = cols;
    this->deep        = deep;
    this->matrix_type = matrix_type;

    build3DInterface( title );
    
    // Fill table if provided
    if( matrix )
    {
        // For each slice
        int i = 0;
        for( MatrixList::iterator it = matrix_list.begin(); 
             it != matrix_list.end();
             ++it, i++ )
        {
            for( int j = 0; j < cols; j++ )
                for( int k = 0; k < deep; k++ )
                    (*it)->setCell( j, k, matrix[i][j][k] );
        }
    }  
}             

/**
 * 2D matrix
 */ 
MatrixEditor::MatrixEditor( MainWindow *main_window, int rows, int cols, 
                            double **matrix, QString title,
                            MatrixType matrix_type )
             :QDialog( 0, 0, true, 0 )
{
    this->main_window = main_window;
    this->rows        = rows;
    this->cols        = cols;
    this->deep        = 1;
    this->matrix_type = matrix_type;

    buildInterface( title );

    // Fill table if provided
    if( matrix )
    {
        for( int i = 0; i < rows; i++ )
            for( int j = 0; j < cols; j++ )
                tb_matrix->setCell( i, j, matrix[i][j] );
    }
}

/**
 * 1D matrix
 */ 
MatrixEditor::MatrixEditor( MainWindow *main_window, int cols,
                            double *vector, QString title,
                            MatrixType matrix_type )
             :QDialog( 0, 0, true, 0 )
{
    this->main_window = main_window;
    this->rows        = 1;
    this->deep        = 1;    
    this->cols        = cols;
    this->matrix_type = matrix_type;

    buildInterface( title );

    // Fill table if provided
    if( vector )
    {
        for( int i = 0; i < cols; i++ )
            tb_matrix->setCell( 0, i, vector[i] );
    }
}

void MatrixEditor::buildInterface( QString title )
{
    setCaption( title );

    window_layout = new QGridLayout( this, 2, 3, 5 );
    
    tb_matrix = new Matrix( this, rows, cols );

    ////////// Buttons //////////    
    bt_save   = new QPushButton( "Save",   this, "bt_save"   );
    bt_load   = new QPushButton( "Load",   this, "bt_load"   );
    bt_cancel = new QPushButton( "Cancel", this, "bt_cancel" );
    
    connect( bt_save, SIGNAL( clicked() ), 
             this, SLOT( checkMatrix() ) );
    connect( bt_load, SIGNAL( clicked() ), 
             this, SLOT( loadMatrix() ) );
    connect( bt_cancel, SIGNAL( clicked() ), 
             this, SLOT( reject() ) );
    
    // Build layout
    window_layout->addMultiCellWidget( tb_matrix, 0, 0, 0, 2 );
    window_layout->addWidget( bt_save,   1, 0 );
    window_layout->addWidget( bt_load,   1, 1 );
    window_layout->addWidget( bt_cancel, 1, 2 );        
        
    adjustSize();
}

void MatrixEditor::build3DInterface( QString title )
{
    setCaption( title );

    window_layout = new QGridLayout( this, 2, 3, 5 );

    tb_slice      = new QTabWidget( this );
    
    // Build each 3d matrix slice at a frame
    for( int frame = 0; frame < rows; frame++ )
    {
        QFrame *new_frame  = new QFrame( this );
        QVBoxLayout *frame_layout = new QVBoxLayout( new_frame, 5 );       
        
        Matrix *new_matrix = new Matrix( new_frame, cols, deep );
        matrix_list.push_back( new_matrix );        
        frame_layout->addWidget( new_matrix );
        
        // Note: 'State' here is a lack of generality, since it's a generic 3D
        // matrix editor. But for interface purposes it's better than 'slice'
        tb_slice->addTab( new_frame, QString( "State %1" ).arg( frame + 1 ) );
    }

    ////////// Buttons //////////    
    bt_save   = new QPushButton( "Save",   this, "bt_save"   );
    bt_load   = new QPushButton( "Load",   this, "bt_load"   );
    bt_cancel = new QPushButton( "Cancel", this, "bt_cancel" );
    
    connect( bt_save, SIGNAL( clicked() ), 
             this, SLOT( check3DMatrix() ) );
    connect( bt_load, SIGNAL( clicked() ), 
             this, SLOT( load3DMatrix() ) );
    connect( bt_cancel, SIGNAL( clicked() ), 
             this, SLOT( reject() ) );
    
    // Build layout
    window_layout->addMultiCellWidget( tb_slice, 0, 0, 0, 2 );
    window_layout->addWidget( bt_save,   1, 0 );
    window_layout->addWidget( bt_load,   1, 1 );
    window_layout->addWidget( bt_cancel, 1, 2 );        

    adjustSize();
}

/**
 * Load matrix entries from a file
 */
void MatrixEditor::load3DMatrix( void )
{
    // Open dialog
    QString file = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file != "" )
    {
        // Load matrix
        FILE *fp = fopen( file.ascii(), "r" );
        
        if( fp )
        {
            double cell;

            int i = 0;
            for( MatrixList::iterator it = matrix_list.begin(); 
                 it != matrix_list.end();
                 ++it, i++ )
            {
                for( int j = 0; j < cols; j++ )
                    for( int k = 0; k < deep; k++ )
                    {
                        if( fscanf( fp, "%lf", &cell) != 1 )
                        {
                            QMessageBox::critical( 0, "Error loading file!",
                                   QString( "Error loading 3D matrix from file." ) );
                        }
                        (*it)->setCell( j, k, cell );                    
                    }
            }
        }
        else
        {
            QMessageBox::critical( 0, "Error loading file!",
                                   QString( "Could not open file " + file ) );
        }
    
        main_window->setCurrentDir( file );
    }    
}

/**
 * Load matrix entries from a file
 */
void MatrixEditor::loadMatrix( void )
{
    // Open dialog
    QString file = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
    if( file != "" )
    {
        // Load matrix
        FILE *fp = fopen( file.ascii(), "r" );
        
        if( fp )
        {
            double cell;
        
            for( int i = 0; i < rows; i++ )
            {
                for( int j = 0; j < cols; j++ )
                {
                    if( fscanf( fp, "%lf", &cell ) != 1 )
                    {
                        QMessageBox::critical( 0, "Error loading file!",
                               QString( "Error loading matrix from file." ) );
                    }
                    tb_matrix->setCell( i, j, cell );
                }
            }
        }
        else
        {
            QMessageBox::critical( 0, "Error loading file!",
                                   QString( "Could not load file " + file ) );
        }
    
        main_window->setCurrentDir( file );
    }    
}

bool MatrixEditor::hasCellEmpty( bool is_3D )
{
    bool status = false;

    if( is_3D )
    {
        // Check all slices
        for( MatrixList::iterator it = matrix_list.begin(); 
             !status && ( it != matrix_list.end() );
             ++it )
        {
            status = (*it)->hasCellEmpty();
        }    
    }
    else
        status = tb_matrix->hasCellEmpty();
        
    return status;
}

bool MatrixEditor::isSthocastic( bool is_3D )
{
    bool status = true;

    if( is_3D )
    {
        // Check all slices
        for( MatrixList::iterator it = matrix_list.begin(); 
             status && ( it != matrix_list.end() );
             ++it )
        {
            status = (*it)->isSthocastic();
        }    
    }
    else
        status = tb_matrix->isSthocastic();
        
    return status;
} 

void MatrixEditor::makeSthocastic( bool is_3D )
{
    if( is_3D )
    {
        // Check all slices
        for( MatrixList::iterator it = matrix_list.begin(); 
             it != matrix_list.end();
             ++it )
        {
            (*it)->makeSthocastic();
        }    
    }
    else
        tb_matrix->makeSthocastic();
} 

bool MatrixEditor::isIndependentProb( bool is_3D )
{
    bool status = true;

    if( is_3D )
    {
        // Check all slices
        for( MatrixList::iterator it = matrix_list.begin(); 
             status && ( it != matrix_list.end() );
             ++it )
        {
            status = (*it)->isIndependentProb();
        }    
    }
    else
        status = tb_matrix->isIndependentProb();
        
    return status;
}                

/**
 * Finish editing all cells
 */
void MatrixEditor::commitChanges( bool is_3D )
{
    if( is_3D )
    {
        // Check all slices
        for( MatrixList::iterator it = matrix_list.begin(); 
             it != matrix_list.end();
             ++it )
        {
            (*it)->commitChanges();
        }    
    }
    else
        tb_matrix->commitChanges();
}

void MatrixEditor::check3DMatrix( void )
{
   checkMatrix( true ); 
}

void MatrixEditor::checkMatrix( bool is_3D )
{
    commitChanges( is_3D );

    // Only process data if no cell is empty
    if( hasCellEmpty( is_3D ) )
    {
        QMessageBox::information( 0, "A cell is empty!",
                                  QString( "Please, fill all cells\n"
                                           "before saving the matrix." ) );        
    }
    else
    {
        switch( matrix_type )
        {
            case sthocastic:
            {            
                if( isSthocastic( is_3D ) )
                    accept();
                else
                {
                    int answer = QMessageBox::question( this, "Not normalized "
                                                        "rows",
                                                        "At least one of the "
                                                        "matrix "
                                                        "rows does "
                                                        "not sum one.\nDo you "
                                                        "want "
                                                        "to normalize these "
                                                        "rows?",
                                                        QMessageBox::Yes | 
                                                        QMessageBox::Default,
                                                        QMessageBox::No );
                    if( answer == QMessageBox::Yes )
                        makeSthocastic( is_3D );
                }        
            }
            break;

            case independent_prob:
            {            
                if( isIndependentProb( is_3D ) )            
                    accept();
                else
                {
                    QMessageBox::warning( 0, "Some cell is not a probability!",
                                          QString( "All cell values must to be "
                                                   "between 0 and 1" ) );
                }
            }
            break;
        }
    }
}

/**
 * Fill 3D matrix with table values
 */
void MatrixEditor::get3DMatrix( double ***matrix )
{
    int i = 0;
    for( MatrixList::iterator it = matrix_list.begin(); 
         it != matrix_list.end();
         ++it, i++ )
    {
        for( int j = 0; j < cols; j++ )
            for( int k = 0; k < deep; k++ )
                matrix[i][j][k] = (*it)->getCell( j, k );
    }
}

/**
 * Fill matrix with table values
 */
void MatrixEditor::getMatrix( double **matrix )
{
    for( int i = 0; i < rows; i++ )
        for( int j = 0; j < cols; j++ )
            matrix[i][j] = tb_matrix->getCell( i, j );
}

/**
 * Fill vector with table values
 */
void MatrixEditor::getVector( double *vector )
{
    for( int i = 0; i < cols; i++ )
        vector[i] = tb_matrix->getCell( 0, i );
}
