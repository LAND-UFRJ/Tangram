#include "ImportFromTangram.h"

#include <qmessagebox.h>
#include <qfiledialog.h>

ImportFromTangram::ImportFromTangram( MainWindow *main_window )
                  :QDialog( 0, 0, true, 0 )
{
    setCaption( "Import parameters from a Tangram model" );

    this->main_window = main_window;

    main_layout = new QVBoxLayout( this, 5 );    

    // Base name
    gb_base_name = new QHGroupBox( "Tangram model file name", this );
    le_base_name = new QLineEdit( gb_base_name );

    bt_choose_base_name = new QPushButton( "Choose file", gb_base_name );
    connect( bt_choose_base_name, SIGNAL( clicked() ), 
             this, SLOT( chooseBaseNameFile() ) );     

    // State variables
    gb_state_variables = new QHGroupBox( "State variables", this );
    fm_state_variables = new QFrame( gb_state_variables );
    state_variables_layout = new QGridLayout( fm_state_variables, 2, 2, 5 );

    // System state variables    
    gb_system_variables = new QHGroupBox( "System variables",
                                           fm_state_variables ); 
    lb_system_state_variables = new QListBox( gb_system_variables );

    // Hidden model variables 
    gb_hidden_variables = new QHGroupBox( "Hidden model variables",
                                           fm_state_variables );
    fm_hidden_variables = new QFrame( gb_hidden_variables );
    hidden_variables_layout = new QGridLayout( fm_hidden_variables, 2, 2, 5 );

    bt_add_hidden_variable =  new QPushButton( "Add", fm_hidden_variables );
    connect( bt_add_hidden_variable, SIGNAL( clicked() ), 
             this, SLOT( addHiddenVariable() ) );     

    bt_remove_hidden_variable = new QPushButton( "Remove",
                                                  fm_hidden_variables );
    connect( bt_remove_hidden_variable, SIGNAL( clicked() ), 
             this, SLOT( removeHiddenVariable() ) );     

    lb_hidden_state_variables = new QListBox( fm_hidden_variables );

    hidden_variables_layout->addWidget( bt_add_hidden_variable, 0, 0 );
    hidden_variables_layout->addWidget( bt_remove_hidden_variable, 1, 0 );
    hidden_variables_layout->addMultiCellWidget( lb_hidden_state_variables,
                                                 0, 1,
                                                 1, 1 );
    // Internal model variables
    gb_internal_variables = new QHGroupBox( "Internal model variables",
                                             fm_state_variables );
    fm_internal_variables = new QFrame( gb_internal_variables );
    internal_variables_layout = new QGridLayout( fm_internal_variables,
                                                 2, 2, 5 );

    bt_add_internal_variable = new QPushButton( "Add", fm_internal_variables );
    connect( bt_add_internal_variable, SIGNAL( clicked() ), 
             this, SLOT( addInternalVariable() ) );     

    bt_remove_internal_variable = new QPushButton( "Remove",
                                                    fm_internal_variables );
    connect( bt_remove_internal_variable, SIGNAL( clicked() ), 
             this, SLOT( removeInternalVariable() ) );     

    lb_internal_state_variables = new QListBox( fm_internal_variables );

    internal_variables_layout->addWidget( bt_add_internal_variable, 0, 0 );
    internal_variables_layout->addWidget( bt_remove_internal_variable, 1, 0 );
    internal_variables_layout->addMultiCellWidget( lb_internal_state_variables,
                                                   0, 1,
                                                   1, 1 );
    // Build state variable layout                           
    state_variables_layout->addMultiCellWidget( gb_system_variables,
                                                0, 1,
                                                0, 0 );
    state_variables_layout->addWidget( gb_hidden_variables,   0, 1 );
    state_variables_layout->addWidget( gb_internal_variables, 1, 1 );

    // Buttons
    bt_import = new QPushButton( "Import", this );
    connect( bt_import, SIGNAL( clicked() ), 
             this, SLOT( checkParameters() ) );

    bt_cancel = new QPushButton( "Cancel", this );
    connect( bt_cancel, SIGNAL( clicked() ), 
             this, SLOT( reject() ) );
    
    // Build layouts
    main_layout->addWidget( gb_base_name       );
    main_layout->addWidget( gb_state_variables );

    buttons_layout = new QHBoxLayout( main_layout, 5 );
    buttons_layout->addWidget( bt_import );    
    buttons_layout->addWidget( bt_cancel );       
}

/**
 * Check parameters and accept dialog if ok
 */
void ImportFromTangram::checkParameters( void )
{
    int  hidden_num_st_var,
         internal_num_st_var;
    bool status;

    hidden_num_st_var   = lb_hidden_state_variables->count();
    internal_num_st_var = lb_internal_state_variables->count();
    status              = true;

    // Check parameters
    if( !hidden_num_st_var )
    {
        QMessageBox::warning( 0, "No hidden model variable found",
                       QString( "Please, add a hidden model variable." ) );
        status = false;
    }

    if( status && !internal_num_st_var )
    {
        int answer = QMessageBox::question( 0, "No internal model variable "
                                            "found",
                                            "No internal model variables were "
                                            "chosen. Are you working with a "
                                            "HMM object?",
                                            QMessageBox::Yes,
                                            QMessageBox::No| 
                                            QMessageBox::Default );

        if( answer == QMessageBox::No )
            status = false;
    }
    
    if( status )
        accept();
}

/**
 * Return true if import operation in done with success
 */
bool ImportFromTangram::runImportFromTangram( Object *object )
{
    register int i;
    int          hidden_num_st_var,
                 internal_num_st_var;
    Arguments    args;
    Token        tk_obj_name,
                 tk_hidden_num_st_var,
                 tk_internal_num_st_var,
                 tk_st_var_name;

    hidden_num_st_var   = lb_hidden_state_variables->count();
    internal_num_st_var = lb_internal_state_variables->count();

    tk_obj_name.type = TK_STRING;
    tk_obj_name.text = le_base_name->text().ascii();
    args.push_back( tk_obj_name );    

    tk_hidden_num_st_var.type = TK_INTEGER;
    tk_hidden_num_st_var.text =
                             QString( "%1" ).arg( hidden_num_st_var ).ascii();
    args.push_back( tk_hidden_num_st_var );    

    if( internal_num_st_var )
    {
        tk_internal_num_st_var.type = TK_INTEGER;
        tk_internal_num_st_var.text =
                             QString( "%1" ).arg( internal_num_st_var ).ascii();
        args.push_back( tk_internal_num_st_var );
    }

    for( i = 0; i < hidden_num_st_var; i++ )
    {
        tk_st_var_name.type = TK_STRING;
        tk_st_var_name.text =
                  lb_hidden_state_variables->text( i ).ascii();
        args.push_back( tk_st_var_name );    
    }

    for( i = 0; i < internal_num_st_var; i++ )
    {
        tk_st_var_name.type = TK_STRING;
        tk_st_var_name.text =
                  lb_internal_state_variables->text( i ).ascii();
        args.push_back( tk_st_var_name );    
    }

    if( object->exec( "import_from_tangram", args ) )
        return true;
    else
        return false;
}

void ImportFromTangram::chooseBaseNameFile( void )
{
    QString file = QFileDialog::getOpenFileName( main_window->getCurrentDir(),
                                                 "Tangram models (*.obj)",
                                                 this,
                                                 "open file dialog",
                                                 "Choose a file" );
    if( file != "" )
    {
        le_base_name->setText( file );
        
        main_window->setCurrentDir( file );
        
        readTangramStateVariables( file );
    }
}

/**
 * Reads state variables related to base_name Tangram model and populates
 * lb_system_state_variables
 */
void ImportFromTangram::readTangramStateVariables( QString base_name )
{
    // Remove file name extension and append other one
    QString states_file_name;
    
    states_file_name  = base_name.left( base_name.findRev( '.', -1 ) );
    states_file_name += EXT_CURRENT_STATES_PERMUTATION;
    
    // Read state variables
    FILE *fd;
    char  state_variable[256];
    
    fd = fopen( states_file_name.ascii(), "r" );
    
    if( fd )
    {
        while( fgets( state_variable, 256, fd ) )
            lb_system_state_variables->insertItem(
                                  QString( state_variable ).stripWhiteSpace() );
    
        fclose( fd );
    }
    else
    {
        QMessageBox::critical( 0, "Error!",
                               QString( "Cannot read state variables file." ) );        
    }
}

void ImportFromTangram::addHiddenVariable( void )
{
    int selected_item = lb_system_state_variables->currentItem();

    if( selected_item == -1 )
    {
        QMessageBox::warning( 0, "No state variable selected",
                         QString( "Please, select a system variable." ) );
    }
    else
    {
        // Add to hidden model listbox
        lb_hidden_state_variables->insertItem( 
              lb_system_state_variables->text( selected_item ) );
              
        // Remove from system variable listbox
        lb_system_state_variables->removeItem( selected_item );
    }
}

void ImportFromTangram::removeHiddenVariable( void )
{
    int selected_item = lb_hidden_state_variables->currentItem();

    if( selected_item == -1 )
    {
        QMessageBox::warning( 0, "No state variable selected",
                         QString( "Please, select a hidden model variable." ) );
    }
    else
    {
        // Add to system variable listbox
        lb_system_state_variables->insertItem( 
              lb_hidden_state_variables->text( selected_item ) );
              
        // Remove from hidden model listbox
        lb_hidden_state_variables->removeItem( selected_item );
    }
}

void ImportFromTangram::addInternalVariable( void )
{
    int selected_item = lb_system_state_variables->currentItem();

    if( selected_item == -1 )
    {
        QMessageBox::warning( 0, "No state variable selected",
                         QString( "Please, select a system variable." ) );
    }
    else
    {
        // Add to internal model listbox
        lb_internal_state_variables->insertItem( 
              lb_system_state_variables->text( selected_item ) );
              
        // Remove from system variable listbox
        lb_system_state_variables->removeItem( selected_item );
    }
}

void ImportFromTangram::removeInternalVariable( void )
{
    int selected_item = lb_internal_state_variables->currentItem();

    if( selected_item == -1 )
    {
        QMessageBox::warning( 0, "No state variable selected",
                         QString( "Please, select an internal model variable." ) );
    }
    else
    {
        // Add to system variable listbox
        lb_system_state_variables->insertItem( 
              lb_internal_state_variables->text( selected_item ) );
              
        // Remove from internal model listbox
        lb_internal_state_variables->removeItem( selected_item );
    }
}
