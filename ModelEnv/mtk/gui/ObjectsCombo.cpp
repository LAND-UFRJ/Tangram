#include "ObjectsCombo.h"

ObjectsCombo::ObjectsCombo( QWidget *parent, MainWindow *main_window )
             :QComboBox( parent )
{
    this->main_window = main_window;
}

void ObjectsCombo::showEvent( QShowEvent * )
{
    clear();
    insertStringList( main_window->getCurrentObjects() );
}

std::string ObjectsCombo::getObject( void )
{
    return currentText().ascii();
}
