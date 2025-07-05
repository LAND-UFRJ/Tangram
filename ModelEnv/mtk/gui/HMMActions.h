#ifndef HMMACTIONS_H
#define HMMACTIONS_H

#include "MainWindow.h"
#include "ObjectsCombo.h"

#include "object.h"

#include <qgroupbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qlistbox.h>
#include <qvbuttongroup.h> 
#include <qradiobutton.h> 

class HMMActions: public QGroupBox
{
    Q_OBJECT

    /**
     * Related only to HMMActions
     */
    private:
        QFrame       *fm_forecast;
        QGridLayout  *forecast_layout;
        QLabel       *lbl_forecast_obs;
        ObjectsCombo *cb_forecast_obs;
        QLabel       *lbl_forecast_wind;
        QSpinBox     *sb_forecast_wind;
        QGroupBox    *gb_forecast_store;
        ObjectsCombo *cb_forecast_store;
        QPushButton  *bt_forecast;

        QGroupBox    *gb_train_obs_obj_src;
        QFrame       *fm_train_obs_obj_src;
        QGridLayout  *train_obs_obj_src_layout;
        ObjectsCombo *cb_train_obs_obj_src;
        QPushButton  *bt_train_obs_obj_src_add;
        QPushButton  *bt_train_obs_obj_src_rm;
        QListBox     *lb_train_obs_obj_src;

        QGroupBox    *gb_likelihood_states;
        ObjectsCombo *cb_likelihood_states;

        QVBoxLayout  *train_layout;

        QRadioButton  *rb_save_B;        

        void          buildForecastFrame( void );

    private slots:
        void runForecast( void );
        void addObject( void );               
        void removeObject( void );                
                   
    /**
     * Related to HMMActions and its descendants
     */
    protected:
        MainWindow    *main_window;
        Object        *object;

        QTabWidget    *tb_actions;
        
        QFrame        *fm_viterbi;
        QGridLayout   *viterbi_layout;
        QLabel        *lbl_viterbi_obs;      
        ObjectsCombo  *cb_viterbi_obs;       
        QLabel        *lbl_viterbi_states;      
        ObjectsCombo  *cb_viterbi_states;
        QPushButton   *bt_viterbi;

        QFrame        *fm_save;
        QVBoxLayout   *save_layout;
        QVButtonGroup *bg_save_option;
        QRadioButton  *rb_save_all;
        QRadioButton  *rb_save_pi;
        QRadioButton  *rb_save_A;
        QGroupBox     *gb_save_file;
        QLineEdit     *le_save_file;
        QPushButton   *bt_save_file;
        QPushButton   *bt_save;
        
        QFrame        *fm_likelihood;
        QGridLayout   *likelihood_layout;
        QLabel        *lbl_likelihood_obs;
        ObjectsCombo  *cb_likelihood_obs;
        QPushButton   *bt_likelihood; 
        
        QFrame        *fm_simulate;
        QGridLayout   *simulate_layout;
        QLabel        *lbl_simulate_sample;
        QSpinBox      *sb_simulate_sample;
        QLabel        *lbl_simulate_obs;
        ObjectsCombo  *cb_simulate_obs;
        QGroupBox     *gb_simulate_store;
        ObjectsCombo  *cb_simulate_store;
        QPushButton   *bt_simulate;
        
        QFrame        *fm_loss_rate;
        QVBoxLayout   *loss_rate_layout;
        QGroupBox     *gb_loss_rate_from_trace;
        QFrame        *fm_loss_rate_from_trace;
        QGridLayout   *loss_rate_from_trace_layout;
        QLabel        *lbl_loss_rate_src;
        ObjectsCombo  *cb_loss_rate_src;
        QLabel        *lbl_loss_rate_future_steps;
        QSpinBox      *sb_loss_rate_future_steps;
        QPushButton   *bt_loss_rate;
        
        QFrame        *fm_state_prob;
        QVBoxLayout   *state_prob_layout;
        QGroupBox     *gb_state_prob_obs;
        ObjectsCombo  *cb_state_prob_obs;
        QPushButton   *bt_state_prob;         
        
        QFrame        *fm_train;
        QTabWidget    *tb_train;
        QPushButton   *bt_train;

        QFrame        *fm_train_obs;
        QGridLayout   *train_obs_layout;
        QLabel        *lbl_train_obs_it;
        QSpinBox      *sb_train_obs_it;
        QGroupBox     *gb_train_obs_thr;
        QLineEdit     *le_train_obs_thr;

        QFrame        *fm_train_st;
        QGridLayout   *train_st_layout;
        QLabel        *lbl_train_st_obs;      
        ObjectsCombo  *cb_train_st_obs;       
        QLabel        *lbl_train_st_states;       
        ObjectsCombo  *cb_train_st_states;

        void           buildViterbiFrame( void );
        virtual void   buildLikelihoodFrame( void );
        virtual void   buildTrainFrame( void );
        virtual void   buildTrainFromObservationsFrame( void );
        void           buildTrainFromStatesFrame( void );
        void           buildSimulateFrame( void );
        void           buildLossRateFrame( void );        
        void           buildStateProbabilityFrame( void );
        virtual void   buildSaveFrame( void );        

    protected slots:
        void          runViterbi( void );
        virtual void  runLikelihood( void );      
        virtual void  runTrain( void );
        void          runSimulate( void );
        void          runLossRate( void );        
        void          runStateProbability( void );          
        void          chooseSaveFile( void );
        virtual void  save( void );

    public:
        /* Note: buildInterface() is public because virtual function cannot be
           called inside constructor */    
        virtual void buildInterface( void );

        HMMActions( QWidget *, Object * );
};

#endif // HMMACTIONS_H
