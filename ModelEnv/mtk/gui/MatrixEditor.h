#ifndef MATRIXEDITOR_H
#define MATRIXEDITOR_H

#include "Matrix.h"
#include "MainWindow.h"

#include <list>

#include <qdialog.h>
#include <qlayout.h> 
#include <qpushbutton.h>
#include <qtabwidget.h>

typedef std::list<Matrix *> MatrixList;

class MatrixEditor: public QDialog
{
    Q_OBJECT

    public:
        typedef enum
        {
            sthocastic,       /* all rows must sum one */
            independent_prob  /* each cell must to be a probability, but there
                                 is no relation between cells */
        } MatrixType;
    
        void get3DMatrix( double *** );
        void getMatrix( double ** );
        void getVector( double *  );        

        MatrixEditor( MainWindow *, int, int, int,
                      double *** = NULL,
                      QString = "Editing 3D matrix", MatrixType = sthocastic );
        MatrixEditor( MainWindow *, int, int,
                      double ** = NULL,
                      QString = "Editing matrix",    MatrixType = sthocastic );
        MatrixEditor( MainWindow *, int,
                      double * = NULL,
                      QString = "Editing vector",    MatrixType = sthocastic );

    private:
        MainWindow   *main_window;                           
        int           rows, cols, deep;                      
        MatrixType    matrix_type;                           
    
        QGridLayout  *window_layout;                         

        Matrix       *tb_matrix;                             
        MatrixList    matrix_list;
        
        QTabWidget   *tb_slice;                                
        
        QPushButton  *bt_save;                                 
        QPushButton  *bt_load;                                 
        QPushButton  *bt_cancel;                               
        
        void          buildInterface( QString );               
        void          build3DInterface( QString );

        void          commitChanges( bool );        
        bool          hasCellEmpty( bool );       
        bool          isSthocastic( bool );
        void          makeSthocastic( bool );
        bool          isIndependentProb( bool );
        
    private slots:
        void checkMatrix( bool = false );
        void check3DMatrix( void );        
        
        void loadMatrix( void );
        void load3DMatrix( void );
};

#endif // MATRIXEDITOR_H
