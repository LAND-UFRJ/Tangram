#ifndef HMMBATCHPARAMETERS_H
#define HMMBATCHPARAMETERS_H

#include "MainWindow.h"
#include "GHMMParameters.h"

#include <qbuttongroup.h>
#include <qradiobutton.h>

class HMMBatchParameters: public GHMMParameters
{
    Q_OBJECT

    /**
     * Related only to HMMBatchParameters
     */
    private:
        
    private slots:

    /**
     * Related to HMMBatchParameters and its descendants
     */
    protected:
        QButtonGroup   *gb_obs_r;
        QRadioButton   *rb_r_random;
        QRadioButton   *rb_r_custom;
        double        **r;
        
        QButtonGroup   *gb_obs_p;
        QRadioButton   *rb_p_random;
        QRadioButton   *rb_p_custom;        
        double       ***p;    
    
        virtual void    buildLayout( void );
        void            buildObs( void );
        
        void            setDefaultR();               
        void            setDefaultP();                 
        
        void            resetStateStructures( int );
        void            resetSymbolStructures( int );          

    private slots:
        void openREditor( void );
        void openPEditor( void );
        
    public:
        typedef enum
        {
            r_random = 0,
            r_custom = 1
        } RMatrixType;    

        typedef enum
        {
            p_random = 0,
            p_custom = 1
        } PMatrixType; 
    
        RMatrixType    getRMatrixType( void );                 
        double       **getRMatrix( void );                     
        PMatrixType    getPMatrixType( void );                 
        double      ***getPMatrix( void );                     

        void           buildInterface( void );

        HMMBatchParameters( QWidget *, MainWindow * );
};

#endif // HMMBATCHPARAMETERS_H
