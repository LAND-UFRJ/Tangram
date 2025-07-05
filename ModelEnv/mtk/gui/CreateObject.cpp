#include "CreateObject.h"
#include "HMMParameters.h"
#include "GHMMParameters.h"
#include "HMMBatchParameters.h"
#include "HMMBatchVariableParameters.h"
#include "IntValueParameters.h"
#include "FloatValueParameters.h"
#include "ImportFromTangram.h"

#include <qmessagebox.h>
#include <qfiledialog.h>

CreateObject::CreateObject( MainWindow *main_window )
             :QDialog( 0, 0, true, 0 )
{
    this->main_window = main_window;
    object_manager    = main_window->getObjectManager();
    current_plugin_info.setNull();
}

/**
 * Constructs window interface
 */
void CreateObject::buildInterface( void )
{
    setCaption( "Create object" );

    window_layout = new QGridLayout( this, 3, 1, 5 );
    
    ////////// Object's parameters //////////
    createBoxes();

    ////////// Object's identification //////////
    gb_identification = new QGroupBox( 1, Qt::Vertical, "Identification", 
                                       this, "gb_identification" );
    fm_identification = new QFrame( gb_identification );
    identification_layout = new QGridLayout( fm_identification, 2, 2, 5 );

    // Name
    lbl_name = new QLabel( "Name:", fm_identification, "lbl_name" );
    le_name  = new QLineEdit( fm_identification, "le_name" );

    // Type
    lbl_type = new QLabel( "Type:", fm_identification, "lbl_type" );
    cb_type  = new QComboBox( false, fm_identification, "cb_type" );

    populateTypeOptions();

    identification_layout->addWidget( lbl_name, 0, 0 );             
    identification_layout->addWidget( le_name,  0, 1 );
    identification_layout->addWidget( lbl_type, 1, 0 );
    identification_layout->addWidget( cb_type,  1, 1 );
    
    // Force frame change for the first time
    changeBox( cb_type->text( 0 ) );

    // Connect for the other times
    connect( cb_type, SIGNAL( activated( const QString & ) ), 
             this, SLOT( changeBox( const QString & ) ) );

    window_layout->addWidget( gb_identification, 0, 0 );

    ////////// Buttons //////////    
    buttons_layout = new QHBoxLayout( window_layout, 5 );
    
    bt_create              = new QPushButton( "Create",              this );
    bt_load                = new QPushButton( "Load",                this );
    bt_import_from_tangram = new QPushButton( "Import from Tangram", this );
    bt_cancel              = new QPushButton( "Cancel",              this );
    
    connect( bt_create, SIGNAL( clicked() ), 
             this, SLOT( createObject() ) );
    connect( bt_load, SIGNAL( clicked() ), 
             this, SLOT( loadObject() ) );
    connect( bt_import_from_tangram, SIGNAL( clicked() ), 
             this, SLOT( importFromTangram() ) );
    connect( bt_cancel, SIGNAL( clicked() ), 
             this, SLOT( reject() ) );
    
    buttons_layout->addWidget( bt_create              );
    buttons_layout->addWidget( bt_load                );    
    buttons_layout->addWidget( bt_import_from_tangram );    
    buttons_layout->addWidget( bt_cancel              );
}

/**
 * Changes the current frame, based on which object type is selected
 */
void CreateObject::changeBox( const QString &object_type )
{
    PluginInfo plugin_info;

    // Remove old box, if set
    if( !current_plugin_info.isNull() )
    {
        window_layout->remove( current_plugin_info.box );
        current_plugin_info.box->hide();
    }

    // Set new plugin info
    current_plugin_info = getPluginInfo( object_type );

    current_plugin_info.box->show();
    
    window_layout->addWidget( current_plugin_info.box, 1, 0 );    

    adjustSize();     
}

/**
 * Gets the PluginInfo object which name is 'name'
 */
PluginInfo CreateObject::getPluginInfo( QString name )
{
    PluginInfo result;
    
    PluginInfoList::iterator it;

    for( it = plugin_info_list.begin( ); it != plugin_info_list.end( ); it++ )
    {
        if( (*it).name == name )
        {
            result = (*it);
            break;
        }
    }
    
    return result;
}

/**
 * Creates all frames and keep them in PluginInfoList
 */
void CreateObject::createBoxes( void )
{
    PluginInfoList::iterator it;

    for( it = plugin_info_list.begin( ); it != plugin_info_list.end( ); it++ )
    {
        if( (*it).name == "hmm" )
        {
            HMMParameters *hmm_parameters = new HMMParameters( this,
                                                               main_window );
            hmm_parameters->buildInterface();
            (*it).box = hmm_parameters;
        }
        else if( (*it).name == "ghmm" )
        {
            GHMMParameters *ghmm_parameters = new GHMMParameters( this,
                                                                  main_window );
            ghmm_parameters->buildInterface();
            (*it).box = ghmm_parameters;
        }
        else if( (*it).name == "hmm_batch" )
        {
            HMMBatchParameters *hmm_batch_parameters = new HMMBatchParameters(
                                                               this,
                                                               main_window );
            hmm_batch_parameters->buildInterface();
            (*it).box = hmm_batch_parameters;
        }        
        else if( (*it).name == "hmm_batch_variable" )
        {
            HMMBatchVariableParameters *hmm_batch_variable_parameters;
            hmm_batch_variable_parameters = new HMMBatchVariableParameters(
                                                this,
                                                main_window );
            hmm_batch_variable_parameters->buildInterface();
            (*it).box = hmm_batch_variable_parameters;
        }
        else if( (*it).name == "intvalue" )
        {
            (*it).box = new IntValueParameters( this, main_window );
        }
        else if( (*it).name == "floatvalue" )
        {
            (*it).box = new FloatValueParameters( this, main_window );
        }        
    }
}

/**
 * Populates plugin type combo with available plugins
 */
void CreateObject::populateTypeOptions()
{
    PluginInfoList::iterator it;

    for( it = plugin_info_list.begin( ); it != plugin_info_list.end( ); it++ )
    {
        cb_type->insertItem( QString( (*it).name ) );
    }    
}

/**
 * Gets plugins from plugin_manager and build plugin's parameters frames
 * Returns false if no plugin is found
 */
bool CreateObject::getPlugins( void )
{
    bool                                 status;
    PluginManager                       *plugin_manager;
    PluginManager::PluginList            plugin_list;
    PluginManager::PluginList::iterator  it;

    status         = true;
    plugin_manager = PluginManager::getPluginManager( );
    plugin_list    = plugin_manager->getPluginList();

    // No plugin found
    if( !( plugin_list.size() > 0 ) )
        status = false;

    // Populate plugin info and insert it into list
    if( status )
    {
        for( it = plugin_list.begin( ); 
             it != plugin_list.end( );
             it++ )
        {
            PluginInfo info;

            info.plugin = (*it);
            info.name   = QString( (*it)->getName() );
            info.box    = NULL;  // will be filled in createBoxes()

            // Skips example plugin, which does not have a graphical interface
            if( info.name != "example" )
                plugin_info_list.push_back( info );
        }
    }

    return status;
}

/**
 * Returns true if the identification parameters are correctly set
 */
bool CreateObject::checkIdentificationParameters( void )
{
    bool status = true;

    // Check for object's name
    if( le_name->text() == "" )
    {
        QMessageBox::warning( 0, "Type object name",
                              QString( "Please, specify object name." ) );
        le_name->setFocus();
        status = false;
    }
    // Check if there is an object with the same name
    else if( object_manager->isNameUsed( le_name->text().ascii() ) )
    {
        QMessageBox::warning( 0, "Object already exists!",
                              QString( "There is already an object named '" +
                                       le_name->text() +
                                       "'.\nPlease, specify another name." ) );
        le_name->setFocus();
        status = false;
    }
    
    return status;
}

/**
 * Actually creates the object
 */
void CreateObject::createObject( void )
{
    bool status = checkIdentificationParameters();
    
    // If everything is ok, try to create object
    if( status )
    {
        // Create specific object, according to user choice
        if( current_plugin_info.name == "hmm" )
            status = createHMMObject();        
        else if( current_plugin_info.name == "ghmm" )
            status = createGHMMObject();
        else if( current_plugin_info.name == "hmm_batch" )
            status = createHMMBatchObject();
        else if( current_plugin_info.name == "hmm_batch_variable" )
            status = createHMMBatchVariableObject();
        else if( current_plugin_info.name == "intvalue" )
            status = createIntValueObject();
        else if( current_plugin_info.name == "floatvalue" )
            status = createFloatValueObject();            
    }
    
    if( status )
        accept();
}

/**
 * Loads the object description from a file
 */
void CreateObject::loadObject( void )
{
    bool status = checkIdentificationParameters();
    QString object_name = le_name->text().ascii();    

    // intvalue and floatvalue plugins cannot be loaded
    if( status                                   &&
        ( current_plugin_info.name == "intvalue" || 
          current_plugin_info.name == "floatvalue" ) )
    {
        QMessageBox::information( 0, "Cannot load object",
                                  QString( "Sorry, cannot load this object "
                                           "type." ) );
        status = false;
    }

    // If everything is ok, try to create object
    if( status )
    {
        // Open dialog
        QString object_description = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );
        if( object_description == "" )
        {
            status = false;        
        }
        else
        {
            // Create specific object, according to user choice
            if( current_plugin_info.name == "hmm" )
                status = createHMMObject( true );
            else if( current_plugin_info.name == "ghmm" )
                status = createGHMMObject( true );               
            else if( current_plugin_info.name == "hmm_batch" )
                status = createHMMBatchObject( true );               
            else if( current_plugin_info.name == "hmm_batch_variable" )
                status = createHMMBatchVariableObject( true );

            // Load data
            if( status )
            {
                // Load object description
                Arguments args;
                Token obj_desc, param;

                obj_desc.type = TK_STRING;
                obj_desc.text = object_description.ascii();
                args.push_back( obj_desc );

                param.type = TK_STRING;
                param.text = QString( "all" ).ascii();
                args.push_back( param );

                new_object->exec( "load", args );
            } 
                
            main_window->setCurrentDir( object_description );               
        }
    }
    
    if( status )
        accept();
}

/**
 * Imports parameters from a Tangram2 mode
 */
void CreateObject::importFromTangram( void )
{
    bool    status      = checkIdentificationParameters();
    QString object_name = le_name->text().ascii();    

    // intvalue and floatvalue plugins cannot be loaded
    if( status                                     &&
        ( current_plugin_info.name == "intvalue"   || 
          current_plugin_info.name == "floatvalue" ) )
    {
        QMessageBox::information( 0, "Cannot import object from Tangram model",
                                  QString( "Sorry, cannot import this object\n"
                                           "type from a Tangram model." ) );
        status = false;
    }

    // If everything is ok, try to create object
    if( status )
    {
        // Open dialog
        ImportFromTangram *import_from_tangram = new ImportFromTangram(
                                                                  main_window );

        if( import_from_tangram->exec() == QDialog::Accepted )
        {
            // Create specific object, according to user choice
            if( current_plugin_info.name == "hmm" )
                status = createHMMObject( true );
            else if( current_plugin_info.name == "ghmm" )
                status = createGHMMObject( true );
            else if( current_plugin_info.name == "hmm_batch" )
                status = createHMMBatchObject( true );
            else if( current_plugin_info.name == "hmm_batch_variable" )
                status = createHMMBatchVariableObject( true );

            // If import data failed, close window and delete
            // the object just created
            if(  status &&
                !import_from_tangram->runImportFromTangram( new_object ) )
            {
                object_manager->destroyObject( object_name.ascii() );
                
                reject();
                
                status = false;
            }
        }
        else
            status = false;
    }
    
    if( status )
        accept();
}

/**
 * Creates a HMM object
 * Return true in case of success
 * If 'just_create' is true, the object parameters are not set, just the object
 * constructor is called
 */
bool CreateObject::createHMMObject( bool just_create )
{
    bool           status;
    Arguments      args;    
    HMMParameters *hmm_box;
    int            n, m;
    std::string    object_name;

    // Get mandatory parameters
    hmm_box     = (HMMParameters *)current_plugin_info.box;
    n           = hmm_box->getNumberOfStates();
    m           = hmm_box->getNumberOfSymbols(); 
    object_name = le_name->text().ascii();    

    // Create tokens
    Token tk_n, tk_m;
    tk_n.type = TK_INTEGER;
    tk_n.text = QString( "%1" ).arg( n ).ascii();    
    tk_m.type = TK_INTEGER;
    tk_m.text = QString( "%1" ).arg( m ).ascii();

    // Fill arguments
    args.push_back( tk_n );   
    args.push_back( tk_m );

    // Create object
    status = object_manager->createObject( current_plugin_info.plugin,
                                           object_name, args );
    args.clear();

    if( status )
    {
        // Set new_object attribute
        new_object = object_manager->getObjectByName( object_name );
    }

    if( !just_create )
    {
        if( status )
        {
            // Initial state distribution
            switch( hmm_box->getInitialDistributionType() )
            {
                case HMMParameters::pi_random:
                    // Do nothing, this is the default
                break;

                case HMMParameters::pi_custom:
                {
                    double *pi = hmm_box->getInitialDistribution();
                    Index   index;

                    for( int i = 0; i < n; i++ )
                    {
                        index.push_back( i );

                        status = new_object->set( "pi", QString( "%1" ).arg( 
                                                  pi[i] ).ascii(), index );
                        index.clear();
                    }
                }
                break;            
            }
        }

        // State transition matrix
        if( status )
        {
            switch( hmm_box->getTransitionMatrixType() )
            {
                case HMMParameters::A_full:
                    // Do nothing, this is the default
                break;

                case HMMParameters::A_qdb:
                {
                    QBDParameterValues *parameters = hmm_box->getQBDParameter();

                    // Create tokens
                    Token tk_groups, tk_elements, tk_symbols, tk_back, tk_fwd;

                    tk_groups.type = TK_INTEGER;
                    tk_groups.text = QString( "%1" ).arg( 
                                        parameters->groups ).ascii();

                    // Note: user cannot change the number of states at this
                    // point in the graphic mode
                    tk_elements.type = TK_INTEGER;
                    tk_elements.text = QString( "%1" ).arg( 
                                        n / parameters->groups ).ascii();

                    // Note: user cannot change the number of symbols at this
                    // point in the graphic mode
                    tk_symbols.type = TK_INTEGER;
                    tk_symbols.text = QString( "%1" ).arg( m ).ascii();

                    if( parameters->has_back )
                    {
                        tk_back.type = TK_INTEGER;
                        tk_back.text = QString( "%1" ).arg( 
                                        parameters->back ).ascii();

                        if( parameters->has_fwd )
                        {
                            tk_fwd.type = TK_INTEGER;
                            tk_fwd.text = QString( "%1" ).arg( 
                                            parameters->fwd ).ascii();                    
                        }
                    }

                    // Fill arguments
                    args.push_back( tk_groups   );   
                    args.push_back( tk_elements );
                    args.push_back( tk_symbols  );
                    if( parameters->has_back )
                    {
                        args.push_back( tk_back );

                        if( parameters->has_fwd )
                            args.push_back( tk_fwd );                    
                    }
                    
                    status = new_object->exec( "set_qbd", args );

                    args.clear();
                }
                break;

                case HMMParameters::A_coxian:
                {
                    // Create tokens
                    Token tk_n, tk_m;

                    tk_n.type = TK_INTEGER;
                    tk_n.text = QString( "%1" ).arg( n ).ascii();

                    tk_m.type = TK_INTEGER;
                    tk_m.text = QString( "%1" ).arg( m ).ascii();

                    // Fill arguments
                    args.push_back( tk_n );   
                    args.push_back( tk_m );            

                    status = new_object->exec( "set_coxian", args );

                    args.clear();
                }
                break;

                case HMMParameters::A_custom:
                {
                    double **A = hmm_box->getTransitionMatrix();
                    Index    index;

                    for( int i = 0; i < n; i++ )
                    {
                        for( int j = 0; j < n; j++ )
                        {
                            index.push_back( i );
                            index.push_back( j );
                            
                            status = new_object->set( "A", QString( "%1" ).arg( 
                                                     A[i][j] ).ascii(), index );
                            index.clear();
                        }
                    }
                }
                break;
            }    
        }

        // Observation symbol matrix
        if( status )
        {
            switch( hmm_box->getObservationMatrixType() )
            {
                case HMMParameters::B_random:
                    // Do nothing, this is the default
                break;

                case HMMParameters::B_custom:
                {
                    double **B = hmm_box->getObservationMatrix();
                    Index    index;

                    for( int i = 0; i < n; i++ )
                    {
                        for( int j = 0; j < m; j++ )
                        {
                            index.push_back( i );
                            index.push_back( j );         
                            
                            status = new_object->set( "B", QString( "%1" ).arg( 
                                                     B[i][j] ).ascii(), index );
                            index.clear();
                        }
                    }
                }
                break;            
            }
        }    
    }
    
    return status;
}

bool CreateObject::createHMMBatchVariableObject( bool just_create )
{
    return createHMMBatchObject( just_create, true );
}

/**
 * Creates a HMMBatch object
 * Return true in case of success
 * If 'just_create' is true, the object parameters are not set, just the object
 * constructor is called
 * If 'variable' is true, a hmm_batch_variable object is created instead
 */
bool CreateObject::createHMMBatchObject( bool just_create, bool variable )
{
    bool                status;
    Arguments           args;    
    HMMBatchParameters *hmm_batch_box;
    int                 n, m, batch;
    std::string         object_name;

    // Get mandatory parameters
    hmm_batch_box = (HMMBatchParameters *)current_plugin_info.box;
    n             = hmm_batch_box->getNumberOfStates();
    m             = hmm_batch_box->getNumberOfSymbols(); 
    
    if( !variable )
        batch = hmm_batch_box->getBatchSize();     

    object_name   = le_name->text().ascii();    

    // Create tokens
    Token tk_n, tk_m, tk_batch;
    tk_n.type = TK_INTEGER;
    tk_n.text = QString( "%1" ).arg( n ).ascii();    
    tk_m.type = TK_INTEGER;
    tk_m.text = QString( "%1" ).arg( m ).ascii();

    if( !variable )
    {
        tk_batch.type = TK_INTEGER;
        tk_batch.text = QString( "%1" ).arg( batch ).ascii();
    }

    // Fill arguments
    args.push_back( tk_n     );
    args.push_back( tk_m     );

    if( !variable )
        args.push_back( tk_batch );

    // Create object
    status = object_manager->createObject( current_plugin_info.plugin,
                                           object_name, args );
    args.clear();

    if( status )
    {
        // Set new_object attribute
        new_object = object_manager->getObjectByName( object_name );
    }

    if( !just_create )
    {
        if( status )
        {
            // Initial state distribution
            switch( hmm_batch_box->getInitialDistributionType() )
            {
                case HMMParameters::pi_random:
                    // Do nothing, this is the default
                break;

                case HMMParameters::pi_custom:
                {
                    double *pi = hmm_batch_box->getInitialDistribution();
                    Index   index;

                    for( int i = 0; i < n; i++ )
                    {
                        index.push_back( i );

                        status = new_object->set( "pi", QString( "%1" ).arg( 
                                                  pi[i] ).ascii(), index );
                        index.clear();
                    }
                }
                break;            
            }
        }

        // State transition matrix
        if( status )
        {
            switch( hmm_batch_box->getTransitionMatrixType() )
            {
                case HMMParameters::A_full:
                    // Do nothing, this is the default
                break;

                case HMMParameters::A_qdb:
                {
                    QBDParameterValues *parameters = hmm_batch_box->getQBDParameter();

                    // Create tokens
                    Token tk_groups, tk_elements, tk_symbols, tk_back, tk_fwd;

                    tk_groups.type = TK_INTEGER;
                    tk_groups.text = QString( "%1" ).arg( 
                                        parameters->groups ).ascii();

                    // Note: user cannot change the number of states at this
                    // point in the graphic mode
                    tk_elements.type = TK_INTEGER;
                    tk_elements.text = QString( "%1" ).arg( 
                                        n / parameters->groups ).ascii();

                    // Note: user cannot change the number of symbols at this
                    // point in the graphic mode
                    tk_symbols.type = TK_INTEGER;
                    tk_symbols.text = QString( "%1" ).arg( m ).ascii();

                    if( parameters->has_back )
                    {
                        tk_back.type = TK_INTEGER;
                        tk_back.text = QString( "%1" ).arg( 
                                        parameters->back ).ascii();

                        if( parameters->has_fwd )
                        {
                            tk_fwd.type = TK_INTEGER;
                            tk_fwd.text = QString( "%1" ).arg( 
                                            parameters->fwd ).ascii();                    
                        }
                    }

                    // Fill arguments
                    args.push_back( tk_groups   );   
                    args.push_back( tk_elements );
                    args.push_back( tk_symbols  );
                    if( parameters->has_back )
                    {
                        args.push_back( tk_back );

                        if( parameters->has_fwd )
                            args.push_back( tk_fwd );                    
                    }
                    
                    status = new_object->exec( "set_qbd", args );

                    args.clear();
                }
                break;

                case HMMParameters::A_coxian:
                {
                    // Create tokens
                    Token tk_n, tk_m;

                    tk_n.type = TK_INTEGER;
                    tk_n.text = QString( "%1" ).arg( n ).ascii();

                    tk_m.type = TK_INTEGER;
                    tk_m.text = QString( "%1" ).arg( m ).ascii();

                    // Fill arguments
                    args.push_back( tk_n );   
                    args.push_back( tk_m );            

                    status = new_object->exec( "set_coxian", args );

                    args.clear();
                }
                break;

                case HMMParameters::A_custom:
                {
                    double **A = hmm_batch_box->getTransitionMatrix();
                    Index    index;

                    for( int i = 0; i < n; i++ )
                    {
                        for( int j = 0; j < n; j++ )
                        {
                            index.push_back( i );
                            index.push_back( j );
                            
                            status = new_object->set( "A", QString( "%1" ).arg( 
                                                     A[i][j] ).ascii(), index );
                            index.clear();
                        }
                    }
                }
                break;
            }    
        }

        // Observation parameters
        if( status )
        {            
            // Initial probabities os symbols at each state
            switch( hmm_batch_box->getRMatrixType() )
            {
                case HMMBatchParameters::r_random:
                    // Do nothing, this is the default
                break;

                case HMMBatchParameters::r_custom:
                {
                    double **r = hmm_batch_box->getRMatrix();
                    Index    index;

                    for( int i = 0; i < n; i++ )
                        for( int j = 0; j < m; j++ )                    
                        {
                            index.push_back( i );
                            index.push_back( j );                            

                            status = new_object->set( "r", QString( "%1" ).arg( 
                                                      r[i][j] ).ascii(),
                                                      index );
                            index.clear();
                        }
                }
                break;            
            }
            
            // Symbol transition matrices at each state
            switch( hmm_batch_box->getPMatrixType() )
            {
                case HMMBatchParameters::p_random:
                    // Do nothing, this is the default
                break;

                case HMMBatchParameters::p_custom:
                {
                    double ***p = hmm_batch_box->getPMatrix();
                    Index     index;

                    for( int i = 0; i < n; i++ )
                        for( int j = 0; j < m; j++ )                   
                            for( int k = 0; k < m; k++ )                        
                            {
                                index.push_back( i );
                                index.push_back( j );                            
                                index.push_back( k );

                                status = new_object->set( "p",
                                         QString( "%1" ).arg(
                                            p[i][j][k] ).ascii(), index );

                                index.clear();
                            }
                }
                break;            
            }            
        }    
    }
    
    return status;
}

/**
 * Creates a GHMM object
 * Return true in case of success
 * If 'just_create' is true, the object parameters are not set, just the object
 * constructor is called 
 */
bool CreateObject::createGHMMObject( bool just_create )
{
    bool            status;
    Arguments       args;    
    GHMMParameters *ghmm_box;
    int             n, batch_size;
    std::string     object_name;

    // Get mandatory parameters
    ghmm_box    = (GHMMParameters *)current_plugin_info.box;
    n           = ghmm_box->getNumberOfStates();
    batch_size  = ghmm_box->getBatchSize(); 
    object_name = le_name->text().ascii();    

    // Create tokens
    Token tk_n, tk_batch_size;
    tk_n.type = TK_INTEGER;
    tk_n.text = QString( "%1" ).arg( n ).ascii();    
    tk_batch_size.type = TK_INTEGER;
    tk_batch_size.text = QString( "%1" ).arg( batch_size ).ascii();

    // Fill arguments
    args.push_back( tk_n          );   
    args.push_back( tk_batch_size );

    // Create object
    status = object_manager->createObject( current_plugin_info.plugin,
                                           object_name, args );
    args.clear();

    if( status )
    {
        // Set new_object attribute
        new_object = object_manager->getObjectByName( object_name );
    }

    if( !just_create )
    {
        if( status )
        {
            // Initial state distribution
            switch( ghmm_box->getInitialDistributionType() )
            {
                case HMMParameters::pi_random:
                    // Do nothing, this is the default
                break;

                case HMMParameters::pi_custom:
                {
                    double *pi = ghmm_box->getInitialDistribution();
                    Index   index;

                    for( int i = 0; i < n; i++ )
                    {
                        index.push_back( i );

                        status = new_object->set( "pi", QString( "%1" ).arg( 
                                                  pi[i] ).ascii(), index );
                        index.clear();
                    }
                }
                break;            
            }
        }

        // State transition matrix
        if( status )
        {
            switch( ghmm_box->getTransitionMatrixType() )
            {
                case HMMParameters::A_full:
                    // Do nothing, this is the default
                break;

                case HMMParameters::A_qdb:
                {
                    QBDParameterValues *parameters = ghmm_box->getQBDParameter();

                    // Create tokens
                    Token tk_groups, tk_elements, tk_batch_size,
                          tk_back, tk_fwd;

                    tk_groups.type = TK_INTEGER;
                    tk_groups.text = QString( "%1" ).arg( 
                                        parameters->groups ).ascii();

                    // Note: user cannot change the number of states at this
                    // point in the graphic mode
                    tk_elements.type = TK_INTEGER;
                    tk_elements.text = QString( "%1" ).arg( 
                                        n / parameters->groups ).ascii();

                    // Note: user cannot change the batch_size at this
                    // point in the graphic mode
                    tk_batch_size.type = TK_INTEGER;
                    tk_batch_size.text = QString( "%1" ).arg( batch_size ).ascii();

                    if( parameters->has_back )
                    {
                        tk_back.type = TK_INTEGER;
                        tk_back.text = QString( "%1" ).arg( 
                                        parameters->back ).ascii();

                        if( parameters->has_fwd )
                        {
                            tk_fwd.type = TK_INTEGER;
                            tk_fwd.text = QString( "%1" ).arg( 
                                            parameters->fwd ).ascii();                    
                        }
                    }

                    // Fill arguments
                    args.push_back( tk_groups     );   
                    args.push_back( tk_elements   );
                    args.push_back( tk_batch_size );
                    if( parameters->has_back )
                    {
                        args.push_back( tk_back );

                        if( parameters->has_fwd )
                            args.push_back( tk_fwd );                    
                    }
                    
                    status = new_object->exec( "set_qbd", args );

                    args.clear();
                }
                break;

                case HMMParameters::A_coxian:
                {
                    // Create tokens
                    Token tk_n, tk_batch_size;

                    tk_n.type = TK_INTEGER;
                    tk_n.text = QString( "%1" ).arg( n ).ascii();

                    tk_batch_size.type = TK_INTEGER;
                    tk_batch_size.text = QString( "%1" ).arg( batch_size ).ascii();

                    // Fill arguments
                    args.push_back( tk_n );   
                    args.push_back( tk_batch_size );            

                    status = new_object->exec( "set_coxian", args );

                    args.clear();
                }
                break;

                case HMMParameters::A_custom:
                {
                    double **A = ghmm_box->getTransitionMatrix();
                    Index    index;

                    for( int i = 0; i < n; i++ )
                    {
                        for( int j = 0; j < n; j++ )
                        {
                            index.push_back( i );
                            index.push_back( j );
                            
                            status = new_object->set( "A", QString( "%1" ).arg( 
                                                     A[i][j] ).ascii(), index );
                            index.clear();
                        }
                    }
                }
                break;
            }    
        }

        // Observation symbol
        if( status )
        {
            // r vector
            switch( ghmm_box->getRVectorType() )
            {
                case GHMMParameters::r_random:
                    // Do nothing, this is the default
                break;

                case GHMMParameters::r_custom:
                {
                    double *r = ghmm_box->getRVector();
                    Index   index;

                    for( int i = 0; i < n; i++ )
                    {
                        index.push_back( i );

                        status = new_object->set( "r", QString( "%1" ).arg( 
                                                  r[i] ).ascii(), index );
                        index.clear();
                    }
                }
                break;            
            }
            
            // p vector
            switch( ghmm_box->getPVectorType() )
            {
                case GHMMParameters::p_random:
                    // Do nothing, this is the default
                break;

                case GHMMParameters::p_custom:
                {
                    double *p = ghmm_box->getPVector();
                    Index   index;

                    for( int i = 0; i < n; i++ )
                    {
                        index.push_back( i );

                        status = new_object->set( "p", QString( "%1" ).arg( 
                                                  p[i] ).ascii(), index );
                        index.clear();
                    }
                }
                break;            
            }
            
            // q vector
            switch( ghmm_box->getQVectorType() )
            {
                case GHMMParameters::q_random:
                    // Do nothing, this is the default
                break;

                case GHMMParameters::q_custom:
                {
                    double *q = ghmm_box->getQVector();
                    Index   index;

                    for( int i = 0; i < n; i++ )
                    {
                        index.push_back( i );

                        status = new_object->set( "q", QString( "%1" ).arg( 
                                                  q[i] ).ascii(), index );
                        index.clear();
                    }
                }
                break;            
            }            
        }    
    }
    
    return status;    
}

/**
 * Creates an intvalue object
 * Return true in case of success
 */
bool CreateObject::createIntValueObject( void )
{
    bool                status = true;
    Arguments           args;    
    IntValueParameters *intvalue_box;
    std::string         object_name;
    
    // Get parameters
    intvalue_box = (IntValueParameters *)current_plugin_info.box;
    object_name  = le_name->text().ascii();

    if( intvalue_box->hasInputFile() )
    {
        if( intvalue_box->getInputFile() == "" )
        {
            QMessageBox::warning( 0, "No file specified",
                                  QString( "Please, specify a file name." ) );
            status = false;
        }
        else
        {    
            Token tk_input_file;
            tk_input_file.type = TK_STRING;
            tk_input_file.text = intvalue_box->getInputFile().ascii();

            args.push_back( tk_input_file );
        }
    }
    
    // Create object
    if( status )
    {
        object_manager->createObject( current_plugin_info.plugin,
                                      object_name, args );
        // Set new_object attribute
        new_object = object_manager->getObjectByName( object_name );
    }
    
    return status;                                               
}

/**
 * Creates a floatvalue object
 * Return true in case of success
 */
bool CreateObject::createFloatValueObject( void )
{
    bool                  status = true;
    Arguments             args;   
    FloatValueParameters *floatvalue_box;
    std::string           object_name;
    
    // Get parameters
    floatvalue_box = (FloatValueParameters *)current_plugin_info.box;
    object_name    = le_name->text().ascii();

    if( floatvalue_box->hasInputFile() )
    {
        if( floatvalue_box->getInputFile() == "" )
        {
            QMessageBox::warning( 0, "No file specified",
                                  QString( "Please, specify a file name." ) );
            status = false;
        }
        else
        {    
            Token tk_input_file;
            tk_input_file.type = TK_STRING;
            tk_input_file.text = floatvalue_box->getInputFile().ascii();

            args.push_back( tk_input_file );
        }
    }
    
    // Create object
    if( status )
    {
        object_manager->createObject( current_plugin_info.plugin,
                                      object_name, args );
        // Set new_object attribute
        new_object = object_manager->getObjectByName( object_name );
    }
    
    return status;                                               
}

Object *CreateObject::getObject( void )
{
    return new_object;
}

QString CreateObject::getType( void )
{
    return cb_type->currentText();
}

/********************
 * Class PluginInfo *
 ********************/
 
/**
 * Returns true if PluginInfo is set
 */
bool PluginInfo::isNull( void )
{
    if( plugin == NULL )
        return true;
    else
        return false;
}

/**
 * Initializes PluginInfo
 */
void PluginInfo::setNull( void )
{
    plugin = NULL;
}
