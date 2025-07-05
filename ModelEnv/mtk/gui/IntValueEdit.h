#ifndef INTVALUEEDIT_H
#define INTVALUEEDIT_H

#include "object.h"

#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h> 
#include <qvalidator.h>

class IntValueEdit: public QDialog
{
    Q_OBJECT

    protected:
        Object        *object;
        bool           read_only;
        
        QVBoxLayout   *window_layout;
        QFrame        *fm_parameters;
        QGridLayout   *parameters_layout;

        QIntValidator *int_validator; 

        QLabel        *lbl_lower;
        QLineEdit     *le_lower;

        QLabel        *lbl_upper;
        QLineEdit     *le_upper;
        
        QLabel        *lbl_size;
        QLineEdit     *le_size;

        QLabel        *lbl_data;
        QTextEdit     *te_data;
        
        QHBoxLayout   *buttons_layout;        
        QPushButton   *bt_save;             
        QPushButton   *bt_cancel;         
        QPushButton   *bt_close;        
    
        void           buildInterface( void );

    protected slots:
        void saveObject( void );
        
    public:
        IntValueEdit( Object *, bool = false );
};

#endif // INTVALUEEDIT_H
