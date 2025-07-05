#ifndef INTVALUEACTIONS_H
#define INTVALUEACTIONS_H

#include "MainWindow.h"

#include "object.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qvalidator.h>

class IntValueActions: public QGroupBox
{
    Q_OBJECT

    protected:
        Object        *object;
        MainWindow    *main_window;

        QTabWidget    *tb_actions;
        
        QFrame        *fm_load;
        QGridLayout   *load_layout;
        QGroupBox     *gb_load_file;
        QLineEdit     *le_load_file;
        QPushButton   *bt_load_file;
        QLabel        *lbl_load_offset;
        QSpinBox      *sb_load_offset;
        QGroupBox     *gb_load_size;
        QLabel        *lbl_load_size;
        QSpinBox      *sb_load_size;
        QPushButton   *bt_load;
        
        QFrame        *fm_save;
        QGridLayout   *save_layout;
        QGroupBox     *gb_save_file;
        QLineEdit     *le_save_file;
        QPushButton   *bt_save_file;
        QLabel        *lbl_save_offset;
        QSpinBox      *sb_save_offset;
        QGroupBox     *gb_save_size;
        QLabel        *lbl_save_size;
        QSpinBox      *sb_save_size;
        QPushButton   *bt_save;   

        QFrame        *fm_truncate;
        QGridLayout   *truncate_layout;        
        QLabel        *lbl_lower;
        QIntValidator *int_validator;
        QLineEdit     *le_lower; 
        QLabel        *lbl_upper;
        QLineEdit     *le_upper;        
        QPushButton   *bt_truncate;
        
        void           buildInterface( void );
        void           buildLoadFrame( void );
        void           buildSaveFrame( void );
        void           buildTruncateFrame( void );

    protected slots:
        void         chooseLoadFile( void );
        void         load( void );
        void         chooseSaveFile( void );     
        void         save( void );
        virtual void truncate( void );
        
    public:
        IntValueActions( QWidget *, Object * );
};

#endif // INTVALUEACTIONS_H
