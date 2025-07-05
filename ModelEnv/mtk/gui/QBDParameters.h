#ifndef QBDPARAMETERS_H
#define QBDPARAMETERS_H

#include "HMMParameters.h"

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h> 
#include <qgroupbox.h>
#include <qlayout.h> 
#include <qframe.h> 

class QBDParameters: public QDialog
{
    Q_OBJECT

    private:
        int                 n;    
        QBDParameterValues *values;
        
        QVBoxLayout        *window_layout;
        QFrame             *fm_parameters;
        QGridLayout        *parameters_layout;
        
        QLabel             *lbl_groups;
        QSpinBox           *sb_groups;

        QGroupBox          *gb_back;
        QSpinBox           *sb_back;

        QGroupBox          *gb_fwd;        
        QSpinBox           *sb_fwd;                   
        
        QHBoxLayout        *buttons_layout;   
        QPushButton        *bt_ok;
        QPushButton        *bt_cancel;        
        
        void buildInterface( void );

    private slots:
        void setParameters( void );
        
    public:
        int  getGroups( void );
        bool hasBack( void );
        int  getBack( void );
        bool hasForward( void );
        int  getForward( void );        

        QBDParameters( QBDParameterValues *, int );
};

#endif // QBDPARAMETERS_H
