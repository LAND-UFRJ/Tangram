#ifndef OBJECTSCOMBO_H
#define OBJECTSCOMBO_H

#include "MainWindow.h"

#include <string>

#include <qcombobox.h>
#include <qevent.h> 

class ObjectsCombo: public QComboBox
{
    protected:
        void showEvent( QShowEvent * );

    private:
        MainWindow *main_window;

    public:
        std::string getObject( void );
    
        ObjectsCombo( QWidget *, MainWindow * );
};

#endif // OBJECTSCOMBO_H

