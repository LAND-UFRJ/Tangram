#include "Matrix.h"

Matrix::Matrix( QWidget *parent, int rows, int cols, double **matrix )
       :QTable( rows, cols, parent )
{
    // Fill table if provided
    if( matrix )
    {
        for( int i = 0; i < rows; i++ )
            for( int j = 0; j < cols; j++ )
                setCell( i, j, matrix[i][j] );
    }
}

Matrix::Matrix( QWidget *parent, int cols, double *vector )
       :QTable( 1, cols, parent )
{
    // Fill table if provided
    if( vector )
    {
        for( int i = 0; i < cols; i++ )
            setCell( 0, i, vector[i] );
    }
}

/**
 * Sets the value of a cell
 */
void Matrix::setCell( int row, int col, double value )
{
    setItem( row, col, 
                  new QTableItem( this,
                                  QTableItem::WhenCurrent,
                                  QString::number( value, 'f', 4 ) ) );
}

void Matrix::setCell( int row, int col, std::string value )
{
    // Convert to double format, i.e., no scientific notatio
    QString cell = QString( "%1" ).arg( QString( value ).toDouble() );

    setItem( row, col, 
                  new QTableItem( this, 
                                  QTableItem::WhenCurrent,
                                  cell ) );
}

/**
 * Gets the value of a cell
 */
double Matrix::getCell( int row, int col )
{
    QTableItem *item;

    item = this->item( row, col );
    
    return item->text().toDouble();
}

/**
 * Set all cells no NotEditing mode
 */
void Matrix::commitChanges( void )
{
    int    rows   = numRows();
    int    cols   = numCols();

    for( int i = 0; i < rows; i++ )
        for( int j = 0; j < cols; j++ )
            setEditMode( QTable::NotEditing, i, j );
}

/**
 * Returns true if all matrix rows sum one
 */
bool Matrix::isSthocastic( void )
{
    bool   status = true;
    int    rows   = numRows();
    int    cols   = numCols();
    double sum;

    // Is there some not normalized row?        
    for( int i = 0; status && ( i < rows ); i++ )
    {
        sum = 0.0;
    
        for( int j = 0; j < cols; j++ )
            sum += getCell( i, j );
        
        if( sum - 1.0 > epsilon )
            status = false;
    }
    
    return status;
}

/**
 * Make all rows sum one
 */
void Matrix::makeSthocastic( void )
{
    int    rows   = numRows();
    int    cols   = numCols();
    double sum;

    // Is there some not normalized row?        
    for( int i = 0; i < rows; i++ )
    {
        sum = 0.0;
    
        for( int j = 0; j < cols; j++ )
            sum += getCell( i, j );
        
        // Normalize it
        if( sum - 1.0 > epsilon )
            for( int j = 0; j < cols; j++ )
                setCell( i, j, getCell( i, j ) / sum );
    }
}

/**
 * Returns true if all cell are in the interval [0,1]
 */
bool Matrix::isIndependentProb( void )
{
    int rows = numRows();
    int cols = numCols();

    for( int i = 0; i < rows; i++ )
    {
        for( int j = 0; j < cols; j++ )
        {
            double cell = getCell( i, j );
            
            // Check interval
            if( ( cell < 0.0 ) || ( cell > 1.0 ) )
                return false;
        }
    }
    
    return true;
}

bool Matrix::hasCellEmpty( void )
{
    int rows = numRows();
    int cols = numCols();

    for( int i = 0; i < rows; i++ )
        for( int j = 0; j < cols; j++ )
            if( text( i, j ) == "" )
                return true;
    
    return false;
}

/**
 * Set all cells 'read_only'
 */
void Matrix::setReadOnly( bool read_only )
{
    int rows = numRows();
    
    for( int i = 0; i < rows; i++ )
        setRowReadOnly( i, read_only );
}
