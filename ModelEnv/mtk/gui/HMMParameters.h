#ifndef HMMPARAMETERS_H
#define HMMPARAMETERS_H

#include <string>

#include "MainWindow.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h> 

/**
 * Reallocates memory to fit the new vector size
 */
#define resizeVector( v, n ) \
            v = (double *)realloc( v, n * sizeof( double ) );

/**
 * Reallocates memory to fit the new matrix size
 */
#define resizeMatrix( m, r, c ) \
            m = (double **)realloc( m, r * sizeof( double * ) ); \
            for( int i = 0; i < r; i++ ) \
                m[i] = (double *)malloc( c * sizeof( double ) );
                
/**
 * Reallocates memory to fit the new 3D matrix size
 */
#define resize3DMatrix( m, r, c, d ) \
            m = (double ***)realloc( m, r * sizeof( double ** ) ); \
            for( int i = 0; i < r; i++ ) \
            { \
                m[i] = (double **)malloc( c * sizeof( double * ) ); \
                for( int j = 0; j < c; j++ ) \
                    m[i][j] = (double *)malloc( d * sizeof( double ) ); \
            }

class QBDParameterValues
{
    public:
        bool null;
        
        int  groups;             
        bool has_back;           
        int  back;               
        bool has_fwd;        
        int  fwd;
        
        bool isNull( void );        
        void setParameters( int, bool, int, bool, int );
        QBDParameterValues( void );
};

class HMMParameters: public QGroupBox
{
    Q_OBJECT

    /**
     * Related only to HMMParameters
     */
    private:
        QButtonGroup        *gb_B;
        QRadioButton        *rb_B_random;
        QRadioButton        *rb_B_custom;
        double             **B;

        void                 buildB( void );        
        void                 setDefaultB( void );
        
    private slots:
        void openBEditor( void );            

    /**
     * Related to HMMParameters and its descendants
     */
    protected:
        MainWindow          *main_window;

        QFrame              *fm_parameters;
        QGridLayout         *parameters_layout;
    
        QLabel              *lbl_N;
        QSpinBox            *sb_N;        

        QLabel              *lbl_M;
        QSpinBox            *sb_M;

        QButtonGroup        *gb_pi;
        QRadioButton        *rb_pi_random;
        QRadioButton        *rb_pi_custom;
        double              *pi;

        QButtonGroup        *gb_A;
        QRadioButton        *rb_A_full;
        QRadioButton        *rb_A_qbd;
        QRadioButton        *rb_A_coxian;
        QRadioButton        *rb_A_custom;
        double             **A;
        QBDParameterValues  *qbd_parameters;

        void                 buildN( void );
        void                 buildM( void );
        void                 buildPi( void );
        void                 buildA( void );
        virtual void         buildLayout( void );
        void                 setDefaultPi( void );
        void                 setDefaultA( void );
        
    protected slots:
        void         openPiEditor( void );
        void         openAEditor( void );
        void         openQBDParameters( void );     
        virtual void resetStateStructures( int );
        virtual void resetSymbolStructures( int );
        
    public:
        typedef enum
        {
            pi_random = 0,
            pi_custom = 1
        } InitialDistributionType;

        typedef enum
        {
            A_full   = 0,
            A_qdb    = 1,
            A_coxian = 2,
            A_custom = 3
        } TransitionMatrixType;
        
        typedef enum
        {
            B_random = 0,
            B_custom = 1
        } ObservationMatrixType;        

        /* Note: buildInterface() is public because virtual function cannot be
           called inside constructor */
        virtual void             buildInterface( void );
        int                      getNumberOfStates( void );
        int                      getNumberOfSymbols( void );
        InitialDistributionType  getInitialDistributionType( void );
        double                  *getInitialDistribution( void );
        TransitionMatrixType     getTransitionMatrixType( void );
        double                 **getTransitionMatrix( void );
        QBDParameterValues      *getQBDParameter( void );
        ObservationMatrixType    getObservationMatrixType( void );
        double                 **getObservationMatrix( void );
        
        HMMParameters( QWidget *, MainWindow * );
};

#endif // HMMPARAMETERS_H
