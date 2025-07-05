#ifndef INTVALUEPARAMETERS_H
#define INTVALUEPARAMETERS_H

#include "MainWindow.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

class IntValueParameters: public QGroupBox
{
    Q_OBJECT

    protected:
        MainWindow  *main_window;
    
        QGroupBox   *gb_input_file;
        QLineEdit   *le_input_file;
        QPushButton *bt_choose_file;
        
        void         buildInterface();

    protected slots:
        void chooseFile( void );
        
    public:
        bool    hasInputFile( void );
        QString getInputFile( void );
                
        IntValueParameters( QWidget *, MainWindow * );
};

#endif // INTVALUEPARAMETERS_H
