#include "MainWindow.h"
#include "CreateObject.h"

#include "HMMActions.h"
#include "GHMMActions.h"
#include "HMMBatchActions.h"
#include "HMMBatchVariableActions.h"
#include "IntValueActions.h"
#include "FloatValueActions.h"

#include "HMMEdit.h"
#include "GHMMEdit.h"
#include "HMMBatchEdit.h"
#include "HMMBatchVariableEdit.h"
#include "IntValueEdit.h"
#include "FloatValueEdit.h"

#include "StreamReader.h"

#include "terminal.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstylesheet.h>

#include <errno.h>

MainWindow::MainWindow( void )
           :QWidget( 0, 0 )
{
    current_dir = "";
    current_object_info.setNull();
    object_manager = ObjectManager::getObjectManager( );    
    
    buildInterface();
    
    redirectOutput();
}

/**
 * Redirects output stream
 */
void MainWindow::redirectOutput( void )
{
    Terminal::setTerminal( Terminal::TERMINAL_SCREEN );

    if( pipe( new_stdout ) == -1 )
    {
        QMessageBox::warning( 0, "Error",
                              QString( "Error in pipe( new_stdout ) command for redirecting output. Errno = " ) +
                              QString::number( errno ) );
    }
    else
    {
        dup2( new_stdout[1], 1 );
        StreamReader *out_reader = new StreamReader( new_stdout[0],
                                                     StreamReader::stdout,
                                                     this );
        out_reader->start();
    }
    
    if( pipe( new_stderr ) == -1 )
    {
        QMessageBox::warning( 0, "Error",
                              QString( "Error in pipe( new_stderr ) command for redirecting output. Errno = " ) +
                              QString::number( errno ) );
    }
    else
    {
        dup2( new_stderr[1], 2 );    

        StreamReader *err_reader = new StreamReader( new_stderr[0],
                                                     StreamReader::stderr,
                                                     this );
        err_reader->start();
    }
}

/**
 * Write output messages into console output
 */
void MainWindow::customEvent( QCustomEvent *event )
{
    if( event->type() == UPDATE_STREAM_EVENT )
    {
        UpdateStreamEvent *update_event = ( UpdateStreamEvent * ) event;
        
        if( update_event->getType() == StreamReader::stdout )
            te_stdout->append( update_event->getText() );
        else
            te_stdout->append( "<red>" + update_event->getText() + "</red>" );
    }
}

/**
 * Constructs MainWindow interface
 */
void MainWindow::buildInterface( void )
{
    setCaption( "Modeling tool kit" );

    window_layout = new QGridLayout( this, 2, 2, 5 );

    ////////// Object's group box //////////
    gb_objects = new QGroupBox( 1, Qt::Vertical, "Objects", 
                                this, "gb_objects" );
    fm_objects = new QFrame( gb_objects );
    objects_layout = new QGridLayout( fm_objects, 3, 2, 5 );
                                    
    // Object's list box
    lb_objects = new QListBox( fm_objects, "lb_objects" );
    connect( lb_objects, SIGNAL( highlighted( const QString & ) ), 
             this, SLOT( showObjectActions( const QString & ) ) );    
    
    // Object's creation and deletion buttons
    bt_create_object  = new QPushButton( "Create",  fm_objects, 
                                         "bt_create_object"  );
    bt_edit_object    = new QPushButton( "Edit",    fm_objects, 
                                         "bt_edit_object"    );
    bt_display_object = new QPushButton( "Display", fm_objects, 
                                         "bt_display_object" );
    bt_delete_object  = new QPushButton( "Delete",  fm_objects, 
                                         "bt_delete_object"  );
    connect( bt_create_object, SIGNAL( clicked() ), 
             this, SLOT( createObject() ) );
    connect( bt_edit_object, SIGNAL( clicked() ), 
             this, SLOT( editSelectedObject() ) ); 
    connect( bt_display_object, SIGNAL( clicked() ), 
             this, SLOT( displaySelectedObject() ) );              
    connect( bt_delete_object, SIGNAL( clicked() ), 
             this, SLOT( deleteSelectedObject() ) );

    objects_layout->addMultiCellWidget( lb_objects, 0, 0, 0, 1 );
    objects_layout->addWidget( bt_create_object,  1, 0 );
    objects_layout->addWidget( bt_delete_object,  2, 0 );
    objects_layout->addWidget( bt_edit_object,    1, 1 );
    objects_layout->addWidget( bt_display_object, 2, 1 );

    ////////// stdout's group box //////////
    gb_stdout = new QGroupBox( 2, Qt::Vertical, "Console output", 
                               this );
    te_stdout = new QTextEdit( gb_stdout );
    te_stdout->setReadOnly( true );
    
    /* StyleSheet to print in red */
    QStyleSheetItem *styleSheet = new QStyleSheetItem( te_stdout->styleSheet(), 
                                                       "red" );
    styleSheet->setColor( "red" );    
    
    bt_stdout_clear = new QPushButton( "Clear", gb_stdout );
    connect( bt_stdout_clear, SIGNAL( clicked() ), 
             this, SLOT( clearStdOut() ) );       

    // Build layout
    window_layout->addMultiCellWidget( gb_objects, 0, 0, 0, 1 );
    window_layout->addMultiCellWidget( gb_stdout,  1, 1, 0, 1 );
    adjustSize();    
}

void MainWindow::clearStdOut( void )
{
    te_stdout->clear();
}

void MainWindow::showObjectActions( const QString &object_name )
{
    // Remove old box, if set
    if( !current_object_info.isNull() )
    {
        window_layout->remove( current_object_info.gb_actions );
        current_object_info.gb_actions->hide();
    }

    // Set new plugin info
    current_object_info = getObjectInfo( object_name );

    // Adjust interface
    current_object_info.gb_actions->show();
    window_layout->remove( gb_objects );
    window_layout->addWidget( gb_objects,                     0, 0 );
    window_layout->addWidget( current_object_info.gb_actions, 0, 1 );
    adjustSize();     
}

/**
 * Open a dialog for the user to specify the object type and properties
 */ 
void MainWindow::createObject( void )
{
    CreateObject *create_object = new CreateObject( this );

    if( !create_object->getPlugins() )
    {
        QMessageBox::critical( 0, "No plugin found!",
                               QString( "Could not find any plugin.\nCheck if "
                                        "your MTK_PATH and TANGRAM2_HOME\n"
                                        "environment variable are "
                                        "correctly set." ) );        
    }
    else
    {
        create_object->buildInterface();
    
        if( create_object->exec() == QDialog::Accepted )
        {
            // Insert new object into object_info_list and listbox
            ObjectInfo object_info;
            object_info.object = create_object->getObject();
            object_info.type   = create_object->getType();

            // Create object actions
            if( object_info.type == "hmm" )
            {
                HMMActions *hmm_actions = new HMMActions( this,
                                                         object_info.object );
                hmm_actions->buildInterface();
                object_info.gb_actions = hmm_actions;
            }
            else if( object_info.type == "ghmm" )
            {
                GHMMActions *ghmm_actions = new GHMMActions( this,
                                                           object_info.object );
                ghmm_actions->buildInterface();
                object_info.gb_actions = ghmm_actions;
            }
            else if( object_info.type == "hmm_batch" )
            {
                HMMBatchActions *hmm_batch_actions = new HMMBatchActions( this,
                                                           object_info.object );
                hmm_batch_actions->buildInterface();
                object_info.gb_actions = hmm_batch_actions;
            }
            else if( object_info.type == "hmm_batch_variable" )
            {
                HMMBatchVariableActions *hmm_batch_variable_actions;
                hmm_batch_variable_actions = new HMMBatchVariableActions( this,
                                                           object_info.object );
                hmm_batch_variable_actions->buildInterface();
                object_info.gb_actions = hmm_batch_variable_actions;
            }
            else if( object_info.type == "intvalue" )
            {
                object_info.gb_actions = new IntValueActions( this,
                                                           object_info.object );
            }
            else if( object_info.type == "floatvalue" )
            {
                object_info.gb_actions = new FloatValueActions( this,
                                                           object_info.object );
            }

            object_info_list.push_back( object_info );

            lb_objects->insertItem( QString( object_info.object->getName() ) );
        }
    }

    delete create_object;
}

ObjectInfo MainWindow::getObjectInfo( QString name )
{
    ObjectInfo result;

    for( ObjectInfoList::iterator it = object_info_list.begin();
         it != object_info_list.end();
         ++it )
    {
        if( (*it).object->getName() == name )
        {
            result = *it;
            break;
        }
    }
    
    return result;
}

void MainWindow::deleteObjectInfo( QString name )
{
    for( ObjectInfoList::iterator it = object_info_list.begin();
         it != object_info_list.end();
         ++it )
    {
        if( (*it).object->getName() == name )
        {
            // Adjust interface
            window_layout->remove( (*it).gb_actions );
            window_layout->remove( gb_objects );            
            window_layout->addMultiCellWidget( gb_objects, 0, 0, 0, 1 );
            (*it).gb_actions->hide();
            adjustSize();
            
            // Remove info from list
            object_info_list.erase( it );
            break;
        }
    }
}

/**
 * Edit the selected object
 */
void MainWindow::editSelectedObject( void )
{
    if( lb_objects->currentItem() == -1 )
    {
        QMessageBox::warning( 0, "No object selected",
                              QString( "Please, select an object to edit." ) );
    }
    else
    {
        ObjectInfo object_info = getObjectInfo( lb_objects->currentText() );
        
        if( object_info.type == "hmm" )
        {    
            HMMEdit *hmm_edit = new HMMEdit( object_info.object, this );
            hmm_edit->buildInterface();
            hmm_edit->exec();
            delete hmm_edit;
        }
        else if( object_info.type == "ghmm" )
        {
            GHMMEdit *ghmm_edit = new GHMMEdit( object_info.object, this );
            ghmm_edit->buildInterface();
            ghmm_edit->exec();
            delete ghmm_edit;        
        }
        else if( object_info.type == "hmm_batch" )
        {
            HMMBatchEdit *hmm_batch_edit = new HMMBatchEdit( object_info.object,
                                                             this );
            hmm_batch_edit->buildInterface();
            hmm_batch_edit->exec();
            delete hmm_batch_edit;        
        }
        else if( object_info.type == "hmm_batch_variable" )
        {
            HMMBatchVariableEdit *hmm_batch_variable_edit;
            hmm_batch_variable_edit = new HMMBatchVariableEdit( 
                                          object_info.object, this );
            hmm_batch_variable_edit->buildInterface();
            hmm_batch_variable_edit->exec();
            delete hmm_batch_variable_edit;        
        }
        else if( object_info.type == "intvalue" )
        {
            IntValueEdit *intvalue_edit = new IntValueEdit( object_info.object );
            intvalue_edit->exec();
            delete intvalue_edit;            
        }
        else if( object_info.type == "floatvalue" )
        {
            FloatValueEdit *floatvalue_edit = new FloatValueEdit( object_info.object );
            floatvalue_edit->exec();
            delete floatvalue_edit;            
        }        
    }
}

/**
 * Display the selected object
 */
void MainWindow::displaySelectedObject( void )
{
    if( lb_objects->currentItem() == -1 )
    {
        QMessageBox::warning( 0, "No object selected",
                              QString( "Please, select an object to "
                                       "display." ) );
    }
    else
    {
        ObjectInfo object_info = getObjectInfo( lb_objects->currentText() );
        
        if( object_info.type == "hmm" )
        {              
            HMMEdit *hmm_edit = new HMMEdit( object_info.object, this, true );
            hmm_edit->buildInterface();
            hmm_edit->exec();
            delete hmm_edit;
        }
        else if( object_info.type == "ghmm" )
        {
            GHMMEdit *ghmm_edit = new GHMMEdit( object_info.object,
                                                this, true );
            ghmm_edit->buildInterface();
            ghmm_edit->exec();
            delete ghmm_edit;        
        }
        else if( object_info.type == "hmm_batch" )
        {           
            HMMBatchEdit *hmm_batch_edit = new HMMBatchEdit( object_info.object,
                                                             this,
                                                             true );
            hmm_batch_edit->buildInterface();
            hmm_batch_edit->exec();

            delete hmm_batch_edit;        
        }
        else if( object_info.type == "hmm_batch_variable" )
        {
            HMMBatchVariableEdit *hmm_batch_variable_edit;
            hmm_batch_variable_edit = new HMMBatchVariableEdit( 
                                          object_info.object, this, true );
            hmm_batch_variable_edit->buildInterface();
            hmm_batch_variable_edit->exec();
            delete hmm_batch_variable_edit;        
        }        
        else if( object_info.type == "intvalue" )
        {
            IntValueEdit *intvalue_edit = new IntValueEdit( object_info.object,
                                                            true );
            intvalue_edit->exec();
            delete intvalue_edit;            
        }        
        else if( object_info.type == "floatvalue" )
        {
            FloatValueEdit *floatvalue_edit = new FloatValueEdit( object_info.object,
                                                                  true );
            floatvalue_edit->exec();
            delete floatvalue_edit;            
        }        
    }
}

/**
 * Deletes the selected object
 */
void MainWindow::deleteSelectedObject( void )
{
    if( lb_objects->currentItem() == -1 )
    {
        QMessageBox::warning( 0, "No object selected",
                             QString( "Please, select an object to remove." ) );
    }
    else
    {
        QString object_name = lb_objects->currentText();

        deleteObjectInfo( object_name );

        lb_objects->removeItem( lb_objects->currentItem() );
    
        object_manager->destroyObject( object_name.ascii() );
    }
}

QString MainWindow::getCurrentDir( void )
{
    if( current_dir == "" )
        return ".";
    else
        return current_dir;
}

void MainWindow::setCurrentDir( QString path )
{
    // Extract directory from path
    int index = path.findRev( '/' );

    current_dir = path.left( index );
}       

ObjectManager *MainWindow::getObjectManager( void )
{
    return object_manager;
}        

QStringList MainWindow::getCurrentObjects( void )
{
    QStringList result;

    for( ObjectInfoList::iterator it = object_info_list.begin();
         it != object_info_list.end();
         ++it )
    {
        result.append( QString( (*it).object->getName() ) );
    }
    
    return result;
}

/********************
 * Class ObjectInfo *
 ********************/

/**
 * Returns true if ObjectInfo is set
 */
bool ObjectInfo::isNull( void )
{
    if( object == NULL )
        return true;
    else
        return false;
}

/**
 * Initializes ObjectInfo
 */
void ObjectInfo::setNull( void )
{
    object = NULL;
}
