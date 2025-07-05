#include "GHMMActions.h"

#include <qmessagebox.h>
#include <qfiledialog.h>

GHMMActions::GHMMActions( QWidget *parent, Object *object )
            :HMMActions( parent, object )
{
}

void GHMMActions::buildInterface( void )
{
    setTitle( "GHMM object actions" );

    tb_actions = new QTabWidget( this );
    
    buildViterbiFrame();
    buildLikelihoodFrame();
    buildTrainFrame();    
    buildSimulateFrame();       
    buildLossRateFrame();
    buildStateProbabilityFrame();    
    buildBurstSizeFrame();   
    buildAutocorrelationFrame();
    buildSaveFrame();        
    
    hide();    
}

void GHMMActions::buildTrainFrame( void )
{
    fm_train     = new QFrame( this );

    tb_train     = new QTabWidget( fm_train );
    connect( tb_train, SIGNAL( currentChanged( QWidget * ) ), 
             this, SLOT( toggleTrainFast( QWidget * ) ) );    

    train_layout = new QGridLayout( fm_train, 2, 2, 5 );

    // Build subframes        
    buildTrainFromObservationsFrame();
    buildTrainFromStatesFrame();

    // Run
    bt_train     = new QPushButton( "Run", fm_train );
    connect( bt_train, SIGNAL( clicked() ), 
             this, SLOT( runTrainSlow() ) );

    // Run faster
    bt_train_fast = new QPushButton( "Run faster", fm_train );
    connect( bt_train_fast, SIGNAL( clicked() ), 
             this, SLOT( runTrainFast() ) );

    train_layout->addMultiCellWidget( tb_train, 0, 0, 0, 1 );
    train_layout->addWidget( bt_train,          1, 0 ); 
    train_layout->addWidget( bt_train_fast,     1, 1 );    

    tb_actions->addTab( fm_train, "Training" );  
    tb_actions->setTabToolTip( fm_train,
        "Estimates a model's parameters by maximizing the likelihood of an observation sample\n"
        "using the Baum-Welch algorithm" );
}

void GHMMActions::buildTrainFromObservationsFrame( void )
{
    fm_train_obs         = new QFrame( fm_train );
    train_obs_layout     = new QGridLayout( fm_train_obs, 3, 2, 5 );

    // Iterations
    lbl_train_obs_it     = new QLabel( "Number of iterations:", fm_train_obs );
    sb_train_obs_it      = new QSpinBox( 1, INT_MAX, 1, fm_train_obs );

    // Threshold
    gb_train_obs_thr     = new QGroupBox( 1, Qt::Vertical,
                                          "Log-likelihood threshold",
                                          fm_train_obs );
    gb_train_obs_thr->setCheckable( true );
    gb_train_obs_thr->setChecked( false );    
    le_train_obs_thr     = new QLineEdit( gb_train_obs_thr );
    le_train_obs_thr->setText( "0.001" );

    // Observation sources
    lbl_obs_src = new QLabel( "Observations source:", fm_train_obs );
    cb_obs_src  = new ObjectsCombo( fm_train_obs, main_window );   

    // Build mainframe layout
    train_obs_layout->addWidget( lbl_train_obs_it, 0, 0 );
    train_obs_layout->addWidget( sb_train_obs_it,  0, 1 );    
    train_obs_layout->addMultiCellWidget( gb_train_obs_thr, 1, 1, 0, 1 );
    train_obs_layout->addWidget( lbl_obs_src, 2, 0 ); 
    train_obs_layout->addWidget( cb_obs_src,  2, 1 );     

    tb_train->addTab( fm_train_obs, "From observations" );
}

void GHMMActions::runTrainFast( void )
{
    runTrain( true );
}

void GHMMActions::runTrainSlow( void )
{
    runTrain( false );
}

void GHMMActions::runTrain( bool fast )
{
    Arguments args;
   
    switch( tb_train->currentPageIndex() )
    {
        // Only from observations
        case 0:
        {
            Token tk_it, tk_thr, tk_obj;
            
            tk_it.type = TK_INTEGER;
            tk_it.text = sb_train_obs_it->text().ascii();
            args.push_back( tk_it );
            
            if( gb_train_obs_thr->isChecked() )
            {
                tk_thr.type = TK_REAL;
                tk_thr.text = le_train_obs_thr->text().ascii();
                args.push_back( tk_thr );            
            }

            tk_obj.type = TK_IDENTIFIER;
            tk_obj.text = cb_obs_src->currentText().ascii();
            args.push_back( tk_obj );
        }
        break;

        // From observations and states
        case 1:
        {
            Token tk_obs, tk_st;
            
            tk_obs.type = TK_IDENTIFIER;
            tk_obs.text = cb_train_st_obs->getObject();
            args.push_back( tk_obs );

            tk_st.type = TK_IDENTIFIER;
            tk_st.text = cb_train_st_states->getObject();
            args.push_back( tk_st );
        }
        break;
    }
    
    if( fast )
        object->exec( "training_fast", args );    
    else            
        object->exec( "training", args );    
}

void GHMMActions::buildLikelihoodFrame( void )
{
    fm_likelihood        = new QFrame( this );
    likelihood_layout    = new QGridLayout( fm_likelihood, 2, 2, 5 );           

    // Observations
    lbl_likelihood_obs   = new QLabel( "Observations source:", fm_likelihood );  
    cb_likelihood_obs    = new ObjectsCombo( fm_likelihood, main_window );    

    // Run
    bt_likelihood        = new QPushButton( "Run", fm_likelihood );
    connect( bt_likelihood, SIGNAL( clicked() ), 
             this, SLOT( runLikelihoodSlow() ) );     
    // Run fast
    bt_likelihood_fast   = new QPushButton( "Run faster", fm_likelihood );
    connect( bt_likelihood_fast, SIGNAL( clicked() ), 
             this, SLOT( runLikelihoodFast() ) );             
    
    // Build layout
    likelihood_layout->addWidget( lbl_likelihood_obs, 0, 0 ); 
    likelihood_layout->addWidget( cb_likelihood_obs,  0, 1 );
    likelihood_layout->addWidget( bt_likelihood,      1, 0 );    
    likelihood_layout->addWidget( bt_likelihood_fast, 1, 1 );    
    
    tb_actions->addTab( fm_likelihood, "Likelihood" );
    tb_actions->setTabToolTip( fm_likelihood,
        "Calculates and displays the probability that a sample was\n"
        "generated by the current model parameters" );
}

void GHMMActions::runLikelihoodFast( void )
{
    runLikelihood( true );
}

void GHMMActions::runLikelihoodSlow( void )
{
    runLikelihood( false );
}

void GHMMActions::runLikelihood( bool fast )
{
    Arguments args;
    Token     tk_object_obs, tk_object_states;

    tk_object_obs.type = TK_IDENTIFIER;
    tk_object_obs.text = cb_likelihood_obs->getObject();
    args.push_back( tk_object_obs );    

    if( fast )
        object->exec( "likelihood_fast", args );
    else        
        object->exec( "likelihood", args );
}

void GHMMActions::buildBurstSizeFrame( void )
{
    fm_burst_size     = new QFrame( this );
    tb_burst_size     = new QTabWidget( fm_burst_size );
    burst_size_layout = new QVBoxLayout( fm_burst_size, 5 );

    // Build subframes        
    buildBurstSizeSteadyStateFrame();
    buildBurstSizeTransientFrame();

    // Run
    bt_burst_size     = new QPushButton( "Run", fm_burst_size );
    connect( bt_burst_size, SIGNAL( clicked() ), 
             this, SLOT( runBurstSize() ) );
 
    burst_size_layout->addWidget( tb_burst_size ); 
    burst_size_layout->addWidget( bt_burst_size );    

    tb_actions->addTab( fm_burst_size, "Burst size" );
    tb_actions->setTabToolTip( fm_burst_size,
        "Calculates the burst size distribution" );
}

void GHMMActions::buildBurstSizeSteadyStateFrame( void )
{
    fm_burst_size_ss     = new QFrame( fm_burst_size );
    burst_size_ss_layout = new QGridLayout( fm_burst_size_ss, 3, 2, 5 );

    // Symbol
    lbl_burst_size_ss_symbol = new QLabel( "Symbol:", fm_burst_size_ss );
    cb_burst_size_ss_symbol  = new QComboBox( fm_burst_size_ss );
    cb_burst_size_ss_symbol->insertItem( "0" );
    cb_burst_size_ss_symbol->insertItem( "1" );    

    // Threshold
    lbl_burst_size_ss_thr    = new QLabel( "Threshold:", fm_burst_size_ss );
    le_burst_size_ss_thr     = new QLineEdit( fm_burst_size_ss );
    le_burst_size_ss_thr->setText( "0.001" );

    // Max samples
    gb_burst_size_ss_samples = new QGroupBox( 1, Qt::Vertical,
                                              "Limit samples",
                                              fm_burst_size_ss );
    gb_burst_size_ss_samples->setCheckable( true );
    gb_burst_size_ss_samples->setChecked( false );    
    sb_burst_size_ss_samples = new QSpinBox( 1, INT_MAX, 1,
                                             gb_burst_size_ss_samples );

    // Build layout
    burst_size_ss_layout->addWidget( lbl_burst_size_ss_symbol, 0, 0 );
    burst_size_ss_layout->addWidget( cb_burst_size_ss_symbol,  0, 1 );    
    burst_size_ss_layout->addWidget( lbl_burst_size_ss_thr,    1, 0 );        
    burst_size_ss_layout->addWidget( le_burst_size_ss_thr,     1, 1 );
    burst_size_ss_layout->addMultiCellWidget( gb_burst_size_ss_samples,
                                              2, 2, 0, 1 );

    tb_burst_size->addTab( fm_burst_size_ss, "Steady state" );
}

void GHMMActions::buildBurstSizeTransientFrame( void )
{
    fm_burst_size_tr     = new QFrame( fm_burst_size );
    burst_size_tr_layout = new QGridLayout( fm_burst_size_tr, 4, 2, 5 );

    // Observations
    lbl_burst_size_tr_src = new QLabel( "Observations source:",
                                        fm_burst_size_tr );    
    cb_burst_size_tr_src  = new ObjectsCombo( fm_burst_size_tr, main_window );

    // Symbol
    lbl_burst_size_tr_symbol = new QLabel( "Symbol:", fm_burst_size_tr );
    cb_burst_size_tr_symbol  = new QComboBox( fm_burst_size_tr );
    cb_burst_size_tr_symbol->insertItem( "0" );
    cb_burst_size_tr_symbol->insertItem( "1" );    

    // Threshold
    lbl_burst_size_tr_thr    = new QLabel( "Threshold:", fm_burst_size_tr );
    le_burst_size_tr_thr     = new QLineEdit( fm_burst_size_tr );
    le_burst_size_tr_thr->setText( "0.001" );

    // Max samples
    gb_burst_size_tr_samples = new QGroupBox( 1, Qt::Vertical,
                                              "Limit samples",
                                              fm_burst_size_tr );
    gb_burst_size_tr_samples->setCheckable( true );
    gb_burst_size_tr_samples->setChecked( false );    
    sb_burst_size_tr_samples = new QSpinBox( 1, INT_MAX, 1,
                                             gb_burst_size_tr_samples );

    // Build layout
    burst_size_tr_layout->addWidget( lbl_burst_size_tr_src,    0, 0 );
    burst_size_tr_layout->addWidget( cb_burst_size_tr_src,     0, 1 );
    burst_size_tr_layout->addWidget( lbl_burst_size_tr_symbol, 1, 0 );
    burst_size_tr_layout->addWidget( cb_burst_size_tr_symbol,  1, 1 );    
    burst_size_tr_layout->addWidget( lbl_burst_size_tr_thr,    2, 0 );        
    burst_size_tr_layout->addWidget( le_burst_size_tr_thr,     2, 1 );
    burst_size_tr_layout->addMultiCellWidget( gb_burst_size_tr_samples,
                                              3, 3, 0, 1 );

    tb_burst_size->addTab( fm_burst_size_tr, "Transient" );
}

void GHMMActions::runBurstSize( void )
{
    Arguments args;
   
    switch( tb_burst_size->currentPageIndex() )
    {
        // Steady state
        case 0:
        {
            Token tk_symbol, tk_thr, tk_samples;
            
            tk_symbol.type = TK_INTEGER;
            tk_symbol.text = cb_burst_size_ss_symbol->currentText().ascii();
            args.push_back( tk_symbol );
            
            tk_thr.type = TK_REAL;
            tk_thr.text = le_burst_size_ss_thr->text().ascii();
            args.push_back( tk_thr );       

            if( gb_burst_size_ss_samples->isChecked() )
            {
                tk_samples.type = TK_INTEGER;
                tk_samples.text = sb_burst_size_ss_samples->text().ascii();
                args.push_back( tk_samples );       
            }
        }
        break;

        // Transient
        case 1:
        {
            Token tk_src, tk_symbol, tk_thr, tk_samples;        

            tk_src.type = TK_IDENTIFIER;
            tk_src.text = cb_burst_size_tr_src->getObject();
            args.push_back( tk_src );

            tk_symbol.type = TK_INTEGER;
            tk_symbol.text = cb_burst_size_ss_symbol->currentText().ascii();
            args.push_back( tk_symbol );
            
            tk_thr.type = TK_REAL;
            tk_thr.text = le_burst_size_ss_thr->text().ascii();
            args.push_back( tk_thr );       

            if( gb_burst_size_ss_samples->isChecked() )
            {
                tk_samples.type = TK_INTEGER;
                tk_samples.text = sb_burst_size_ss_samples->text().ascii();
                args.push_back( tk_samples );       
            }
        }
        break;
    }
    
    object->exec( "burst_size", args );     
}

void GHMMActions::buildAutocorrelationFrame( void )
{
    fm_autocorrelation     = new QFrame( this );
    autocorrelation_layout = new QGridLayout( fm_autocorrelation, 3, 2, 5 );
    
    // Threshold
    lbl_autocorrelation_thr = new QLabel( "Threshold", fm_autocorrelation );
    le_autocorrelation_thr  = new QLineEdit( fm_autocorrelation );
    le_autocorrelation_thr->setText( "0.001" );
    
    // Max samples
    gb_autocorrelation_samples = new QGroupBox( 1, Qt::Vertical,
                                                "Limit samples",
                                                fm_autocorrelation );
    gb_autocorrelation_samples->setCheckable( true );
    gb_autocorrelation_samples->setChecked( false );    
    sb_autocorrelation_samples = new QSpinBox( 1, INT_MAX, 1,
                                               gb_autocorrelation_samples );    

    // Run
    bt_autocorrelation = new QPushButton( "Run", fm_autocorrelation );
    connect( bt_autocorrelation, SIGNAL( clicked() ), 
             this, SLOT( runAutocorrelation() ) );

    // Build layout
    autocorrelation_layout->addWidget( lbl_autocorrelation_thr, 0, 0 );
    autocorrelation_layout->addWidget( le_autocorrelation_thr,  0, 1 );
    autocorrelation_layout->addMultiCellWidget( gb_autocorrelation_samples,
                                                1, 1, 0, 1 );
    autocorrelation_layout->addMultiCellWidget( bt_autocorrelation,
                                                2, 2, 0, 1 );

    tb_actions->addTab( fm_autocorrelation, "Autocorrelation" );
    tb_actions->setTabToolTip( fm_autocorrelation,
        "Evaluates the autocorrelation" );    
}

void GHMMActions::runAutocorrelation( void )
{
    Arguments args;
    Token     tk_thr, tk_samples;
    
    tk_thr.type = TK_REAL;
    tk_thr.text = le_autocorrelation_thr->text().ascii();
    args.push_back( tk_thr );
    
    if( gb_autocorrelation_samples->isChecked() )
    {
        tk_samples.type = TK_INTEGER;
        tk_samples.text = sb_autocorrelation_samples->text().ascii();
        args.push_back( tk_samples );     
    }
    
    object->exec( "autocorrelation", args );     
}        

/**
 * Disable "Run faster" button, when "From states" tab is selected, because the
 * 'fast' version is not implemented for this case
 */
void GHMMActions::toggleTrainFast( QWidget *frame )
{
    if( frame == fm_train_obs )
        bt_train_fast->setEnabled( true );
    else
        bt_train_fast->setEnabled( false );
}

void GHMMActions::buildSaveFrame( void )
{
    fm_save         = new QFrame( this );
    save_layout     = new QVBoxLayout( fm_save, 5 );    
    
    // Parameter options
    bg_save_option  = new QVButtonGroup( "Parameter", fm_save );
    rb_save_all     = new QRadioButton( "All", bg_save_option );
    rb_save_pi      = new QRadioButton( "Initial state vector",
                                         bg_save_option );
    rb_save_A       = new QRadioButton( "State transition matrix",
                                         bg_save_option );
    rb_save_obs     = new QRadioButton( "Observation parameters",
                                         bg_save_option );
    // Default option is all parameters
    rb_save_all->setChecked( true );
    
    // File name
    gb_save_file    = new QGroupBox( 2, Qt::Horizontal,
                                     "File name", fm_save );
    le_save_file    = new QLineEdit( gb_save_file );
    bt_save_file    = new QPushButton( "Choose file", gb_save_file );
    connect( bt_save_file, SIGNAL( clicked() ), 
             this, SLOT( chooseSaveFile() ) );     
    bt_save         = new QPushButton( "Save", fm_save );
    connect( bt_save, SIGNAL( clicked() ), 
             this, SLOT( save() ) ); 

    save_layout->addWidget( bg_save_option );
    save_layout->addWidget( gb_save_file );
    save_layout->addWidget( bt_save      );
    
    tb_actions->addTab( fm_save, "Save" ); 
    tb_actions->setTabToolTip( fm_save,
        "Stores a model description or an individual parameter in a file" );
}

void GHMMActions::save( void )
{
    if( le_save_file->text() == "" )
    {
        QMessageBox::warning( 0, "No file selected",
                              QString( "Please, select a file to save." ) );
    }
    else
    {
        Arguments args;
        Token file, param;

        /* Building filename */
        file.type = TK_STRING;
        file.text = le_save_file->text().ascii();
        args.push_back( file );
        
        switch( bg_save_option->selectedId() )
        {
            case 0:
                param.type = TK_IDENTIFIER;
                param.text = QString( "all" ).ascii();
                args.push_back( param );
                object->exec( "save", args ); 
                break;

            case 1:
                param.type = TK_IDENTIFIER;
                param.text = QString( "pi" ).ascii();
                args.push_back( param );
                object->exec( "save", args ); 
                break;

            case 2:
                param.type = TK_IDENTIFIER;
                param.text = QString( "A" ).ascii();
                args.push_back( param );
                object->exec( "save", args ); 
                break;          
                
            case 3:
                param.type = TK_IDENTIFIER;
                param.text = QString( "obs" ).ascii();
                args.push_back( param );
                object->exec( "save", args ); 
                break;                     
        }    
    }
}
