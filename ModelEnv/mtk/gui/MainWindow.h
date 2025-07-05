#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <list>

#include "object.h"
#include "object_manager.h"

#include <qlayout.h> 
#include <qwidget.h> 
#include <qgroupbox.h> 
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtextedit.h>
#include <qevent.h>
#include <qframe.h>

/**
 * ObjectInfo
 */
class ObjectInfo
{
    public:
        Object    *object;
        QString    type;
        QGroupBox *gb_actions;
        bool       isNull( void );
        void       setNull( void );        
};

typedef std::list<ObjectInfo> ObjectInfoList;

class MainWindow: public QWidget
{
    Q_OBJECT

    private:
        int               new_stdout[2];
        int               new_stderr[2];        
        
        QString           current_dir;
        ObjectInfo        current_object_info;
        ObjectManager    *object_manager;        

        QGridLayout      *window_layout;
    
        QGroupBox        *gb_objects;
        QFrame           *fm_objects;
        QGridLayout      *objects_layout;
        QListBox         *lb_objects; 
        QPushButton      *bt_create_object;
        QPushButton      *bt_delete_object;
        QPushButton      *bt_edit_object;
        QPushButton      *bt_display_object;       

        QGroupBox        *gb_stdout;
        QTextEdit        *te_stdout;
        QPushButton      *bt_stdout_clear;

        ObjectInfoList    object_info_list;
        static const int  COLUMN_NAME = 0;
        static const int  COLUMN_TYPE = 1;

        void              buildInterface( void );                 

        ObjectInfo        getObjectInfo( QString );
        void              deleteObjectInfo( QString );
        void              redirectOutput( void );
        void              customEvent( QCustomEvent * );

    private slots:
        void createObject( void );
        void showObjectActions( const QString & );
        void editSelectedObject( void );
        void displaySelectedObject( void );        
        void deleteSelectedObject( void );        
        void clearStdOut( void );        
        
    public:
        QString        getCurrentDir( void );
        QStringList    getCurrentObjects( void );
        void           setCurrentDir( QString );
        ObjectManager *getObjectManager( void );
        
        MainWindow( void );
};

#endif // MAINWINDOW_H
