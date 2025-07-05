#ifndef GHMMEDIT_H
#define GHMMEDIT_H

#include "HMMEdit.h"

#include <qvgroupbox.h>
#include <qpushbutton.h>

class GHMMEdit: public HMMEdit
{
    Q_OBJECT

    /**
     * Related only to GHMMEdit
     */
    private:
        QLabel      *lbl_obs_r;
        Matrix      *tb_obs_r;
        QLabel      *lbl_obs_p;
        Matrix      *tb_obs_p;
        QLabel      *lbl_obs_q;
        Matrix      *tb_obs_q;        
    
        void         fillR( void );
        void         fillP( void );
        void         fillQ( void );

        void         saveR( void );
        void         saveP( void );
        void         saveQ( void );                         

    /**
     * Related to GHMMEdit and its descendants
     */
    protected:
        int          batch_size;
        QLabel      *lbl_batch_size;                
        QLineEdit   *le_batch_size;

        QVGroupBox  *gb_obs;
        QPushButton *bt_load_obs;

        void         buildBatchSize( void );        
        virtual void buildLayout( void );   

        virtual void buildObs( void );

        virtual void fillStructures( void );
        virtual void saveStructures( void );

    protected slots:
        virtual void checkStructures( void );
        virtual void loadObs( void );
        
    public:
        void buildInterface( void );

        GHMMEdit( Object *, MainWindow *, bool = false );
};

#endif // GHMMEDIT_H
