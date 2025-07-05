#ifndef CREATEOBJECT_H
#define CREATEOBJECT_H

#include <list>
#include <string>

#include "types.h"
#include "plugin.h"
#include "plugin_manager.h"
#include "object.h"

#include "MainWindow.h"

#include <qdialog.h>
#include <qlayout.h> 
#include <qpushbutton.h>
#include <qgroupbox.h> 
#include <qlineedit.h> 
#include <qlabel.h>
#include <qframe.h>
#include <qcombobox.h>

/**
 * PluginInfo
 */
class PluginInfo
{
    public:
        Plugin    *plugin;
        QString    name;
        QGroupBox *box;
        bool       isNull( void );
        void       setNull( void );
};

typedef std::list<PluginInfo> PluginInfoList;

/**
 * CreateObject
 */
class CreateObject: public QDialog
{
    Q_OBJECT

    private:
        MainWindow    *main_window;

        Object        *new_object;
        ObjectManager *object_manager;

        PluginInfo     current_plugin_info;
        PluginInfoList plugin_info_list;
    
        QGridLayout   *window_layout;       
   
        QGroupBox     *gb_identification;
        QFrame        *fm_identification;
        QGridLayout   *identification_layout;
        QLabel        *lbl_name;            
        QLineEdit     *le_name;             
        QLabel        *lbl_type;            
        QComboBox     *cb_type;

        QHBoxLayout   *buttons_layout;      
        QPushButton   *bt_create;           
        QPushButton   *bt_load;
        QPushButton   *bt_import_from_tangram;
        QPushButton   *bt_cancel;
       
        void           populateTypeOptions( void );   
        void           createBoxes( void );
        bool           checkIdentificationParameters( void );
        PluginInfo     getPluginInfo( QString );

        bool           createHMMObject( bool = false );
        bool           createGHMMObject( bool = false );
        bool           createHMMBatchObject( bool = false, bool = false );
        bool           createHMMBatchVariableObject( bool = false );
        bool           createIntValueObject( void );
        bool           createFloatValueObject( void );        

    private slots:
        void createObject( void );
        void loadObject( void );
        void importFromTangram( void );
        void changeBox( const QString & );
        
    public:
        Object  *getObject( void );
        QString  getType( void );
        bool     getPlugins( void );
        void     buildInterface( void ); 
        
        CreateObject( MainWindow * );
};

#endif // CREATEOBJECT_H
