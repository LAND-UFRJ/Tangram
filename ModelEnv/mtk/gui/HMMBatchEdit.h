#ifndef HMMBATCHEDIT_H
#define HMMBATCHEDIT_H

#include "GHMMEdit.h"

#include <qtabwidget.h>
#include <qpushbutton.h>

#include <list>

typedef std::list<Matrix *> MatrixList;

class HMMBatchEdit: public GHMMEdit
{
    Q_OBJECT

    protected:
        QLabel      *lbl_obs_r;
        Matrix      *tb_obs_r;

        QLabel      *lbl_obs_p;
        QTabWidget  *tb_state;
        
        MatrixList   matrix_list;
    
        virtual void buildLayout( void );
        void         buildObs( void );
        
        void         fillStructures( void );
        void         fillR( void );                
        void         fillP( void );                
        
        void         saveStructures( void );        
        void         saveR( void );                
        void         saveP( void ); 

    protected slots:
        void         checkStructures( void );    
        void         loadObs( void );

    public:
        virtual void buildInterface( void );

        HMMBatchEdit( Object *, MainWindow *, bool = false );
};

#endif // HMMBATCHEDIT_H
