#ifndef IMPORTFROMTANGRAM_H
#define IMPORTFROMTANGRAM_H

#include "object.h"
#include "MainWindow.h"

#include <qhgroupbox.h>
#include <qdialog.h>
#include <qlineedit.h>

#define EXT_CURRENT_STATES_PERMUTATION ".vstat"

class ImportFromTangram: public QDialog
{
    Q_OBJECT

    private:
        MainWindow    *main_window;        

        QVBoxLayout   *main_layout;
        
        QHGroupBox    *gb_base_name;
        QLineEdit     *le_base_name;
        QPushButton   *bt_choose_base_name;
        
        QHGroupBox    *gb_state_variables;
        QFrame        *fm_state_variables;
        QGridLayout   *state_variables_layout;

        QHGroupBox    *gb_system_variables;              
        QListBox      *lb_system_state_variables;

        QHGroupBox    *gb_hidden_variables;
        QFrame        *fm_hidden_variables;
        QGridLayout   *hidden_variables_layout;
        QPushButton   *bt_add_hidden_variable;
        QPushButton   *bt_remove_hidden_variable;
        QListBox      *lb_hidden_state_variables;

        QHGroupBox    *gb_internal_variables;
        QFrame        *fm_internal_variables;
        QGridLayout   *internal_variables_layout;
        QPushButton   *bt_add_internal_variable;
        QPushButton   *bt_remove_internal_variable;
        QListBox      *lb_internal_state_variables;        

        QHBoxLayout   *buttons_layout;        
        QPushButton   *bt_import;        
        QPushButton   *bt_cancel;        
        
        void           readTangramStateVariables( QString );

    private slots:
        void chooseBaseNameFile( void );        
        void addHiddenVariable( void );              
        void removeHiddenVariable( void );           
        void addInternalVariable( void );            
        void removeInternalVariable( void );         
        void checkParameters( void );

    public:
        ImportFromTangram( MainWindow * );

        bool runImportFromTangram( Object * );              
};

#endif // IMPORTFROMTANGRAM_H
