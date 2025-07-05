#ifndef GHMMPARAMETERS_H
#define GHMMPARAMETERS_H

#include "MainWindow.h"
#include "HMMParameters.h"

#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

class GHMMParameters: public HMMParameters
{
    Q_OBJECT

    /**
     * Related only to GHMMParameters
     */
    private:
        QButtonGroup *gb_obs_r;
        QRadioButton *rb_r_random;
        QRadioButton *rb_r_custom;
        double       *r;
        
        QButtonGroup *gb_obs_p;
        QRadioButton *rb_p_random;
        QRadioButton *rb_p_custom;        
        double       *p;

        QButtonGroup *gb_obs_q;
        QRadioButton *rb_q_random;
        QRadioButton *rb_q_custom;                
        double       *q;        
    
        void          buildLayout( void );
        void          resetStateStructures( int );
        void          setDefaultR();
        void          setDefaultP();
        void          setDefaultQ();        
        
    private slots:
        void openREditor( void );
        void openPEditor( void );
        void openQEditor( void );                

    /**
     * Related to GHMMParameters and its descendants
     */
    protected:        
        QLabel       *lbl_batch_size;
        QSpinBox     *sb_batch_size;

        QGroupBox    *gb_obs;

        void          buildBatchSize( void );
        virtual void  buildObs( void );

    protected slots:
        
    public:
        typedef enum
        {
            r_random = 0,
            r_custom = 1
        } RVectorType;    

        typedef enum
        {
            p_random = 0,
            p_custom = 1
        } PVectorType;    

        typedef enum
        {
            q_random = 0,
            q_custom = 1
        } QVectorType;            
    
        int                      getBatchSize( void );    
        void                     buildInterface( void );
        RVectorType              getRVectorType( void );
        double                  *getRVector( void ); 
        PVectorType              getPVectorType( void );
        double                  *getPVector( void ); 
        QVectorType              getQVectorType( void );
        double                  *getQVector( void );

        GHMMParameters( QWidget *, MainWindow * );
};

#endif // GHMMPARAMETERS_H
