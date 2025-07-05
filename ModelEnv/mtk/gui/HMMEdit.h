#ifndef HMMEDIT_H
#define HMMEDIT_H

#include "Matrix.h"
#include "MainWindow.h"

#include "object.h"

#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qpushbutton.h> 
#include <qvgroupbox.h> 

class HMMEdit: public QDialog
{
    Q_OBJECT

    /**
     * Related only to HMMEdit
     */
    private:
        QVGroupBox  *gb_B;
        Matrix      *tb_B;
        QPushButton *bt_load_B;        
        
        void         buildB( void );        
        void         fillB( void );        
        void         saveB( void ); 

    private slots:       
        void         loadB( void );          

    /**
     * Related to HMMEdit and its descendants
     */
    protected:

        Object      *object;
        MainWindow  *main_window;
        bool         read_only;
              
        QVBoxLayout *window_layout;      

        QFrame      *fm_parameters;
        QGridLayout *parameters_layout;

        int          n;        
        QLabel      *lbl_N;               
        QLineEdit   *le_N;                

        int          m;        
        QLabel      *lbl_M;                
        QLineEdit   *le_M;                

        QVGroupBox  *gb_pi;
        Matrix      *tb_pi;
        QPushButton *bt_load_pi;        

        QVGroupBox  *gb_A;
        Matrix      *tb_A;
        QPushButton *bt_load_A;        

        QHBoxLayout *buttons_layout;        
        QPushButton *bt_save;             
        QPushButton *bt_cancel;         
        QPushButton *bt_close;        

        void         buildCaption( void );
        void         buildN( void );
        void         buildM( void ); 
        void         buildPi( void );
        void         buildA( void );
        void         buildButtons( void );        

        virtual void buildLayout( void );        
        
        virtual void fillStructures( void );
        void         fillPi( void );
        void         fillA( void );        
        
        virtual void saveStructures( void );
        void         savePi( void );
        void         saveA( void );        

    protected slots:
        void         loadPi( void );
        void         loadA( void );
        virtual void checkStructures( void );

    public:
        /* Note: buildInterface() is public because virtual function cannot be
           called inside constructor */    
        virtual void buildInterface( void );

        HMMEdit( Object *, MainWindow *, bool = false );
};

#endif // HMMEDIT_H
