#ifndef GHMMACTIONS_H
#define GHMMACTIONS_H

#include "HMMActions.h"

class GHMMActions: public HMMActions
{
    Q_OBJECT

    /**
     * Related only to GHMMActions
     */                
    private:
        QFrame       *fm_burst_size;
        QTabWidget   *tb_burst_size;
        QVBoxLayout  *burst_size_layout;
        QPushButton  *bt_burst_size;
        
        QFrame       *fm_burst_size_ss;
        QGridLayout  *burst_size_ss_layout;
        QLabel       *lbl_burst_size_ss_symbol;
        QComboBox    *cb_burst_size_ss_symbol;
        QLabel       *lbl_burst_size_ss_thr;
        QLineEdit    *le_burst_size_ss_thr;
        QGroupBox    *gb_burst_size_ss_samples;
        QSpinBox     *sb_burst_size_ss_samples;
        
        QFrame       *fm_burst_size_tr;
        QGridLayout  *burst_size_tr_layout;
        QLabel       *lbl_burst_size_tr_src;
        ObjectsCombo *cb_burst_size_tr_src;
        QLabel       *lbl_burst_size_tr_symbol;
        QComboBox    *cb_burst_size_tr_symbol;
        QLabel       *lbl_burst_size_tr_thr;
        QLineEdit    *le_burst_size_tr_thr;
        QGroupBox    *gb_burst_size_tr_samples;
        QSpinBox     *sb_burst_size_tr_samples;
        
        QFrame       *fm_autocorrelation;
        QGridLayout  *autocorrelation_layout;
        QLabel       *lbl_autocorrelation_thr;
        QLineEdit    *le_autocorrelation_thr;
        QGroupBox    *gb_autocorrelation_samples;
        QSpinBox     *sb_autocorrelation_samples;
        QPushButton  *bt_autocorrelation;

        void          buildBurstSizeFrame( void );
        void          buildBurstSizeTransientFrame( void );        
        void          buildBurstSizeSteadyStateFrame( void );

        void          buildAutocorrelationFrame( void );

    private slots:
        void runBurstSize( void );        
        void runAutocorrelation( void );        

    /**
     * Related to GHMMActions and its descendants
     */                
    protected:
        QLabel       *lbl_obs_src;
        ObjectsCombo *cb_obs_src;

        QPushButton  *bt_likelihood_fast;

        QRadioButton  *rb_save_obs;                

        QGridLayout  *train_layout;
        QPushButton  *bt_train_fast;

        void          buildTrainFrame( void );    
        void          buildTrainFromObservationsFrame( void );

        void          buildLikelihoodFrame( void );
        void          buildSaveFrame( void );

        void          runTrain( bool );
        void          runLikelihood( bool );
    
    protected slots:
        void runTrainSlow( void );
        void toggleTrainFast( QWidget * );
        void runTrainFast( void );
        void runLikelihoodSlow( void );        
        void runLikelihoodFast( void );
        void save( void );
        
    public:
        void buildInterface( void );

        GHMMActions( QWidget *, Object * );
};

#endif // GHMMACTIONS_H
