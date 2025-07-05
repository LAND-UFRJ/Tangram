#include "IntValueParameters.h"

#include <qfiledialog.h>

IntValueParameters::IntValueParameters( QWidget *parent,
                                        MainWindow *main_window )
                   :QGroupBox( 100, Qt::Vertical, "Parameters", parent )
{
    this->main_window = main_window;

    buildInterface();

    hide();    
}

void IntValueParameters::buildInterface()
{
    gb_input_file = new QGroupBox( 2, Qt::Horizontal, "Input file", this );
    gb_input_file->setCheckable( true );
    gb_input_file->setChecked( false );  
    
    le_input_file = new QLineEdit( gb_input_file );
    
    bt_choose_file = new QPushButton( "Choose file", gb_input_file );
    connect( bt_choose_file, SIGNAL( clicked() ), 
             this, SLOT( chooseFile() ) );    
}

void IntValueParameters::chooseFile( void )
{
    // Open dialog
    QString input_file = QFileDialog::getOpenFileName(
                                             main_window->getCurrentDir(),
                                            "Text files (*.txt);;All files (*)",
                                             this,
                                            "open file dialog",
                                            "Choose a file" );    

    if( input_file != "" )
    {
        le_input_file->setText( input_file );
        
        main_window->setCurrentDir( input_file );
    }
}

bool IntValueParameters::hasInputFile( void )
{
    return gb_input_file->isChecked();
}

QString IntValueParameters::getInputFile( void )
{
    return le_input_file->text();
}
