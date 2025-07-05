/*
 * Copyright (C) 1999-2009, Edmundo Albuquerque de Souza e Silva.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE.QPL included in the packaging of this file.
 *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* Header file for distribution.cpp */

/**
*	@file distribution.h
*   @brief <b>Event distributions</b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.1
*   @since 1.0
*   @author Daniel Ratton Figueredo (1.0)
*   @author Carlos F. Brito (1.0)
*   @author Guilherme Dutra G. Jaime (1.1)
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*
*/

#ifndef __DISTRIBUTION_H__
#define __DISTRIBUTION_H__

#include "random.h"
#define   CACHE_VALUE      10000

/** Sample_File current mode of operation */
enum
{
    CLOSED     ,
    WRITE_MODE ,
    READ_MODE 
};

/** @name Ditribution Defines
 *  possible distributions types 
 *  @warning It must be the same of gramatica.h
 */
/*@{*/
#define UNIFORM_DIST        1
#define DETERMINISTIC_DIST  2
#define EXPONENTIAL_DIST    3
#define GAUSSIAN_DIST       4
#define FBM_DIST            5
#define FARIMA_DIST         6
#define ERLANG_M_DIST       7
#define LOGNORMAL_DIST      8
#define FILE_DIST           9
#define WEIBULL_DIST        10
#define PARETO_DIST         11
#define GAUSSIAN_DIST_NUM   12
#define REW_REACH_DIST      13
#define TRUNCLOGNORMAL_DIST 14
#define TRUNC_PARETO_DIST   15
#define INIT_DIST           16
/*@}*/


typedef class Distribution_Numeric {


 public:
    Distribution_Numeric(Random_obj *);
    virtual ~Distribution_Numeric();

    virtual TGFLOAT next_sample()=0;
    virtual int     show_type()  =0;
    virtual void    print_dist() =0;
    Random_obj      *the_random_obj;

/** generates a U[0,1] */
    TGFLOAT randval();

} Distribution_Numeric;


/**
*  This class generates random numbers according to a Gaussian
*  distribution. It receives input parameters in NUMERIC
*  format.
*   @class Gaussian_Distrib_Numeric
*   @version 1.1
*   @since 1.0
*   @author Daniel Ratton Figueredo (1.0)
*   @author Carlos F. Brito (1.0)
*   @author Guilherme Dutra G. Jaime (1.1)
*
*/

typedef class Gaussian_Distrib_Numeric : public Distribution_Numeric {

 public:
    Gaussian_Distrib_Numeric(Random_obj *);
    ~Gaussian_Distrib_Numeric();

    TGFLOAT     next_sample();
    int         show_type();
    int         ch_mean(TGFLOAT);
    int         ch_variance(TGFLOAT);
    void        print_dist();


 private:
 /** Mean value of Gaussian distribution */
    TGFLOAT mean;
 /**   Variance of Gaussian distribution */
    TGFLOAT variance;

} Gaussian_Distrib_Numeric;




/**
*
* Developed for operation with sample files (in this
* context, text files with numbers, one by line). It implements
* a cache facility to reduce the number of disk readings,
* improving the retrieval time of the sample file contents.
* @class Sample_File
*
*   @version 1.0
*   @since 1.0
*   @author Daniel Ratton Figueredo (1.0)
*   @author Carlos F. Brito (1.0)
*/


typedef class Sample_File {

public:
   Sample_File();
   ~Sample_File();
   int     open_r(char*, int);
   int     open_w(char*);
   TGFLOAT read();
   int     write(TGFLOAT);
   int     close();
   int     delete_file();
   int     get_file_mode();
   int     rewind_file();
   int     eof();

private:
/** File descriptor for access to a file on disk */
   FILE *   fd;
/** File name */
   char     f_name[MAXSTRING];
/** Length of the cache page to be used in read operations */
   int      cache_length;
/** Pointer to the cache array wich will contain the samples retrieved from file */
   TGFLOAT *sample_cache;
/** Stores the current mode of operation: READ_MODE, WRITE_MODE, CLOSED */
   int      file_status;
/** Indicates the current sample index to be read in the cache array sample_cache */
   int      sample_index;
/** Indicates the actual maximum sample index wich can be read in the cache array sample_cache */
   int      max_sample_index;
/** Indicates if the cache array sample_cache contains the last page of data from the file */
   int      last_cache_page;
/** Indicates if the end of file was reached */
   int      EOF_reached;

} Sample_File;



/** 
*
* Base class of all distribution classes for TANGRAM-2 simulator.
* It's an ADT and is fully related with TANGRAM-2 internal data
* types like expression and object_state.
* @class Distribution
*
*   @version 1.0
*   @since 1.0
*   @author Daniel Ratton Figueredo (1.0)
*   @author Carlos F. Brito (1.0)
*/

typedef class Distribution {

 public:
    Distribution(Random_obj *);
    virtual ~Distribution();

    virtual TGFLOAT next_sample(Object_State *) =0;
    virtual int     show_type()  =0;
    virtual void    print_dist() =0;
    virtual void    reset(int)   =0;
    Random_obj      *the_random_obj;

    TGFLOAT randval();  /**< generates a U[0,1] */

} Distribution;


/** 
*
* This class generates random numbers according to an
* Exponential distribution.
*
* @class Exponential_Distrib
*
*/
typedef class Exponential_Distrib : public Distribution {

 public:
    Exponential_Distrib(Random_obj *);
    ~Exponential_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_rate();
    int         ch_rate(Expression *);
    void        print_dist();
    void        reset(int);

 private:
    Expression *rate;    /**< Rate of Exponential  distribution */

} Exponential_Distrib;



/** 
*
* This class generates random numbers according to an
* Erlang distribution with "m" stages.
*
* @class Erlang_m_Distrib
*
*/
typedef class Erlang_m_Distrib : public Distribution {

 public:
    Erlang_m_Distrib(Random_obj *);
    ~Erlang_m_Distrib();


    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_rate();
    Expression *show_stages();
    int         ch_rate(Expression *);
    int         ch_stages(Expression *);
    void        print_dist();
    void        reset(int);

 private:
    Expression *rate;    /**< Rate of Erlang  distribution */
    Expression *stages;  /**< Number of stages  of Erlang  distribution */
} Erlang_m_Distrib;


/** 
*
*  This class generates a deterministic number equal to
*  its 'rate' value.
*
* @class Deterministic_Distrib
*
*/
typedef class Deterministic_Distrib : public Distribution {

 public:
    Deterministic_Distrib(Random_obj *);
    ~Deterministic_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_rate();
    int         ch_rate(Expression *);
    void        print_dist();
    void        reset(int);

 private:
    Expression *rate;  /**< "Rate" of Deterministic  distribution */

} Deterministic_Distrib;


/** 
*
*  This class generates random numbers according to an
*  Uniform distribution.
*
* @class Uniform_Distrib
*
*/
typedef class Uniform_Distrib : public Distribution {

 public:
    Uniform_Distrib(Random_obj *);
    ~Uniform_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_lower();
    int         ch_lower(Expression *);
    Expression *show_upper();
    int         ch_upper(Expression *);
    void        print_dist();
    void        reset(int);

 private:
    Expression *lower;      /**< Lower limit for Uniform  distribution */
    Expression *upper;      /**< Upper limit for Uniform  distribution */

} Uniform_Distrib;


/** 
*
*  This class generates random numbers according to a
*  Gaussian distribution.
*
* @class Gaussian_Distrib
*
*/
typedef class Gaussian_Distrib : public Distribution {

 public:
    Gaussian_Distrib(Random_obj *);
    ~Gaussian_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_mean();
    int         ch_mean(Expression *);
    Expression *show_variance();
    int         ch_variance(Expression *);
    void        print_dist();
    void        reset(int);

 private:
    Expression *mean;     /**< Mean value of Gaussian distribution */
    Expression *variance; /**< Variance  of Gaussian distribution */

} Gaussian_Distrib;


/** 
*
* This class generates random numbers according to a
* Lognormal distribution.
*
* @class Lognormal_Distrib
*
*/
typedef class Lognormal_Distrib : public Distribution {

 public:
    Lognormal_Distrib(Random_obj *);
    ~Lognormal_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_mean();
    int         ch_mean(Expression *);
    Expression *show_variance();
    int         ch_variance(Expression *);
    void        print_dist();
    void        reset(int);

 private:

    Expression               *mean;      /**< Mean value of Lognormal distribution */
    Expression               *variance;  /**<  Variance  of Lognormal distribution */
    Gaussian_Distrib_Numeric Gaussvar;   /**< Gaussian number generator for Lognormal distribution */

} Lognormal_Distrib;


/** 
*
*  This class generates random numbers according to a
*  Truncated Lognormal distribution respecting the minimum and
*  maximum bounds.
*
* @class TruncLognormal_Distrib
*
*/
typedef class TruncLognormal_Distrib : public Distribution {

 public:
    TruncLognormal_Distrib( Random_obj * );
    ~TruncLognormal_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_mean();
    Expression *show_minimum();
    Expression *show_maximum();
    int         ch_mean(Expression *);
    Expression *show_variance();
    int         ch_variance(Expression *);
    int         ch_minimum(Expression *);
    int         ch_maximum(Expression *);
    void        print_dist();
    void        reset(int);

 private:

    Expression               *mean;      /**< Mean value of Lognormal distribution */
    Expression               *variance;  /**<  Variance  of Lognormal distribution */
    Expression               *minimum;   /**< Mean value of Lognormal distribution */
    Expression               *maximum;   /**<  Variance  of Lognormal distribution */
    Gaussian_Distrib_Numeric Gaussvar;   /**< Gaussian number generator for Lognormal distribution */

} TruncLognormal_Distrib;


/** 
*
*  This class generates random numbers according to a
*  Weibull distribution
*
* @class Weibull_Distrib
*
*/
typedef class Weibull_Distrib : public Distribution {

 public:
    Weibull_Distrib(Random_obj *);
    ~Weibull_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_scale();
    Expression *show_shape();
    int         ch_scale(Expression *);
    int         ch_shape(Expression *);
    void        print_dist();
    void        reset(int);

 private:

    Expression               *scale;    /**< Scale value of Weibull distribution */
    Expression               *shape;    /**<  Shape  of Weibull distribution */

} Weibull_Distrib;


/** 
*
*  This class generates random numbers according to a
*  Pareto distribution.
*
* @class Pareto_Distrib
*
*/
typedef class Pareto_Distrib : public Distribution {

 public:
    Pareto_Distrib(Random_obj *);
    ~Pareto_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_scale();
    Expression *show_shape();
    int         ch_scale(Expression *);
    int         ch_shape(Expression *);
    void        print_dist();
    void        reset(int);

 protected:

    Expression               *scale;   /**< Scale  of Pareto distribution */
    Expression               *shape;   /**<  Shape  of Pareto distribution */

} Pareto_Distrib;


/** 
*
*  This class generates random numbers according to a
*  Truncated Pareto distribution.
*
* @class Trunc_Pareto_Distrib
*
*/
typedef class Trunc_Pareto_Distrib : public Pareto_Distrib {

 public:
    Trunc_Pareto_Distrib(Random_obj *);
    ~Trunc_Pareto_Distrib();

    TGFLOAT     next_sample(Object_State *);
    int         show_type();
    Expression *show_maximum();
    int         ch_maximum(Expression *);
    void        print_dist();

 private:

    Expression               *maximum; /**< Max value of Truncated Pareto     
                                          generated samples distribution´s. */
} Trunc_Pareto_Distrib;


/** 
*
*  This class retrieves numbers from a specified
*  sample file.
*
* @class File_Distrib
*
*/
typedef class File_Distrib : public Distribution {

 public:
    File_Distrib(Random_obj *);
    ~File_Distrib();

    TGFLOAT      next_sample(Object_State *);
    TGFLOAT      next_sample();
    int          show_type();
    int          ch_file(char*);
    int          ch_file(char*,int);
    void         show_file(char*);
    void         print_dist();
    void         reset(int);

 private:
    char          f_name[MAXSTRING];    /**< Name of the file wich holds the samples */
    Sample_File   f_samples;            /**< Sample file object to be asociated with the file named in 'f_name' */
    int           total_ev_tsc;         /**< Number of events to be generated during the actual time scale */
    int           ev_tsc;               /**< Number of events already ocurred in the actual time scale */
    TGFLOAT       inter_time_tsc;       /**< Time between events in the actual time scale */

} File_Distrib;

/** 
*
* This is not a distribution indeed.
* It represents the REWARD_REACHED event type.
*
* @class Rew_Reach_Distrib
*
*/
typedef class Rew_Reach_Distrib : public Distribution {

 public:
    Rew_Reach_Distrib(Random_obj *);
    ~Rew_Reach_Distrib();

    TGFLOAT  next_sample(Object_State *);
    int      show_type();
    void     print_dist();
    void     reset(int);

    TGFLOAT      show_delta_time();
    void         ch_delta_time(TGFLOAT);
    TGFLOAT      show_cr();
    void         ch_cr(TGFLOAT);
    TGFLOAT      show_ir();
    void         ch_ir(TGFLOAT);
    int          show_skip_event();
    void         ch_skip_event(int);
    int          show_need_evaluation();
    void         ch_need_evaluation(int);
    int          show_trigger_now();
    void         ch_trigger_now(int);
    TGFLOAT      show_level();
    void         ch_level(TGFLOAT);

    Stamp_List  *show_stamp_list(int);
    void         add_stamp(Stamp *);
    void         clear_stamp_list();

    RR_Touch_List *show_rr_touch_list(int);
    void           add_rr_touch(RR_Touch *);
    void           clear_rr_touch_list();

 private:

    /** delta_time keeps the next sample.                */
    TGFLOAT       delta_time;

    /** cr, ir and level are used to keep the rew values 
        that causes the next event.                      */
    TGFLOAT       cr;
    TGFLOAT       ir;
    TGFLOAT       level;

    /** skip_event is a flag used to cancel the event    
        that should not trigger actually.                */
    int           skip_event;

    /** need_evaluation is a flag used to indicate that the 
       REWARD_REACHED event nedds or not to be evaluated   */
    int           need_evaluation;

    /** trigger_now is a flag used to force the trigger  
        of the event when some CR touch their bound.     */
    int           trigger_now;

    /** stamp_list keeps the pairs rew_id, cr_target     
        that will be stamped.                            */
    Stamp_List    *stamp_list;

    /** rr_touch_list keeps the info to calculate delta_time 
       this is done by calling will_reach_reward method!    */
    RR_Touch_List *rr_touch_list;

} Rew_Reach_Distrib;


/** 
*
* This is not a distribution indeed.
* This is used by Rew_Reach_Distrib.
*
* @class Stamp
*
*/
typedef class Stamp : public Chained_Element
{

 public:
    Stamp(TGFLOAT, TGFLOAT, int, int);
    ~Stamp();

    TGFLOAT  show_delta_time();
    void     ch_delta_time( TGFLOAT );

    int      show_rew_id();
    void     ch_rew_id( int );

    int      show_skip_flag();
    void     ch_skip_flag( int );

    TGFLOAT  show_cr_target();
    void     ch_cr_target( TGFLOAT );

 private:
    /** time to reach */
    TGFLOAT  delta_time;
    /** id of the reward that will receive cr_target */
    int      rew_id;
    /** cr_target keeps the reward value to be set when the
        event triggers. (to avoid numeric problems in comparison) */
    TGFLOAT  cr_target;
    /** this flag indicates if this reward will generate an event
        that will be fake or real. skip_flag==FALSE (real event) */
    int      skip_flag;

} Stamp;


/** 
*
* This is not a distribution indeed.
* This is used by Rew_Reach_Distrib.
*
* @class Stamp_List
*
*/
typedef class Stamp_List : public Chained_List
{
 public:
    Stamp_List();
    Stamp_List(Stamp_List *);
    ~Stamp_List();

    int    add_tail_stamp(Stamp *);
    Stamp *show_1st_stamp();
    Stamp *get_1st_stamp();
    Stamp *get_curr_stamp();
    Stamp *show_next_stamp();
    Stamp *show_curr_stamp();
    int    query_stamp(int);
    int    show_no_stamp();
    int    have_to_skip();
} Stamp_List;


/** 
*
* This is not a distribution indeed.
* This is used by Rew_Reach_Distrib.
*
* @class RR_Touch
*
*/
typedef class RR_Touch : public Chained_Element
{

 public:
    RR_Touch( int, TGFLOAT, int, Reward_Measure * );
    ~RR_Touch();

    int  show_rew_type();
    void ch_rew_type( int );

    int  show_direction();
    void ch_direction( int );

    TGFLOAT show_expr_value();
    void    ch_expr_value( TGFLOAT );

    Reward_Measure *show_rew_meas();
    void ch_rew_meas( Reward_Measure * );

    Rate_Reward_Desc *show_rate_rew_desc();
    void ch_rate_rew_desc( Rate_Reward_Desc * );

    Rate_Reward_Sum *show_rate_rew_sum();
    void ch_rate_rew_sum( Rate_Reward_Sum * );

 private:
    TGFLOAT  expr_value;
    int      direction; /**< LOW_REACH_TYPE or UPP_REACH_TYPE */
    int      rew_type;  /**< GET_CR_TYPE    or GET_CR_SUM_TYPE */

    Reward_Measure   *rew_meas;
    Rate_Reward_Desc *rate_rew_desc;
    Rate_Reward_Sum  *rate_rew_sum;

} RR_Touch;


/** 
*
* This is not a distribution indeed.
* This is used by Rew_Reach_Distrib.
*
* @class RR_Touch_List
*
*/
typedef class RR_Touch_List : public Chained_List
{
 public:
    RR_Touch_List();
    RR_Touch_List(RR_Touch_List *);
    ~RR_Touch_List();

    int    add_tail_rr_touch(RR_Touch *);
    RR_Touch *show_1st_rr_touch();
    RR_Touch *get_1st_rr_touch();
    RR_Touch *get_curr_rr_touch();
    RR_Touch *show_next_rr_touch();
    RR_Touch *show_curr_rr_touch();
    int    show_no_rr_touch();
} RR_Touch_List;

/** 
*
* This is not a distribution indeed.
* This is used by Initialization Event.
*
* @class Init_Distrib
*
*/
typedef class Init_Distrib : public Distribution 
{
 public:
    Init_Distrib( Random_obj * );
    ~Init_Distrib(); 
 
    TGFLOAT next_sample( Object_State * );
    int     show_type();
    void    print_dist();
    void    reset(int);

 private:    
    static const TGFLOAT init_event_time = 0.0;  /*< All init events runs at time zero.
                                                     If other event is scheduled
                                                     at this time, the function
                                                     Simulator_Event_List::show_smallest_time
                                                     will force INIT to run
                                                     first */
};


/** 
*
*  This class generates random numbers according to an
*  Fractional Brownian Motion (FBM) sample path realization.
*  It generates a file with the FBM samples, to be used in
*  'next_sample()' calls. The FBM file is generated according
*  to the successive random additions algorithm (see "The Science
*  of Fractal Images", B.B. Mandelbrot, pp. 86) .
*
* @class FBM_Distrib
*
*/
typedef class FBM_Distrib : public Distribution {

 public:
    FBM_Distrib(Random_obj *);
    ~FBM_Distrib();

    TGFLOAT next_sample(Object_State *);
    int     show_type();
    int     ch_param(char*, int, TGFLOAT, TGFLOAT, TGFLOAT, TGFLOAT );
    void    print_dist();
    void    reset(int);

 private:
    int               generate();

    Gaussian_Distrib_Numeric  Gaussvar;       /**< Gaussian number generator for FBM's sample path generation  */
    int	              maxlevel;               /**< Maximum number of levels to be used in the FBM  generation (see reference)*/
    TGFLOAT           mean_value;             /**< Mean value parameter in FBM generation */
    TGFLOAT           stddev;                 /**< Standard deviation parameter in FBM generation */
    TGFLOAT           hurst;                  /**< Hurst parameter in FBM generation */
    TGFLOAT           tsc;                    /**< Time scale lentgh to be used in 'next_sample()' methods */
    char              filename[MAXSTRING];    /**< Name of the file wich holds the FBM samples */
    Sample_File       fbm_file;               /**< Sample file object to be asociated with the file named in 'filename' */
    int               total_ev_tsc;           /**< Number of events to be generated during the actual time scale */
    int               ev_tsc;                 /**< Number of events already ocurred in the actual time scale */
    TGFLOAT           inter_time_tsc;         /**< Time between events in the actual time scale */
    int               init_OK;                /**< Indicates if the initialization/generation of FBM was successfull */
} FBM_Distrib;


/** 
*
*  This class generates random numbers according to an 
*  fractional ARIMA (0,d,0) sample path realization.
*  It generates a file with the f-ARIMA samples, to be used in
*  'next_sample()' calls. The f-ARIMA file is generated according
*  to the Hosking's algorithm (see "Analysis, Modeling and Generation 
*  of Self-Similar VBR Video Traffic", Mark Garret and Walter 
*  Wilinger, ACM SigComm, 1994).
*
* @class FARIMA_Distrib
*
*/
typedef class FARIMA_Distrib : public Distribution {
    
 public:
    FARIMA_Distrib(Random_obj *);
    ~FARIMA_Distrib();

    TGFLOAT next_sample(Object_State *);
    int     show_type();
    int     ch_param(char*, int, TGFLOAT, TGFLOAT, TGFLOAT, TGFLOAT);
    void    print_dist();
    void    reset(int);


 private:
    int               generate();

    Gaussian_Distrib_Numeric  Gaussvar;    /**< Gaussian number generator for f-ARIMA's sample path generation    */
    long              maxnumber;           /**< Maximum number of samples to be generated in the sample path file */
    TGFLOAT           mean_value;          /**< Mean value parameter in FBM generation */
    TGFLOAT           variance;            /**< Standard deviation parameter in FBM generation */
    TGFLOAT           param_d;             /**< Parameter "d" in fractional ARIMA (0,d,0) process */
    TGFLOAT           tsc;                 /**< Time scale lentgh to be used in 'next_sample()' methods */
    char              filename[MAXSTRING]; /**< Name of the file wich holds the FBM samples */
    Sample_File       fARIMA_file;         /**< Sample file object to be asociated with the file named in 'filename' */
    int               total_ev_tsc;        /**< Number of events to be generated during the actual time scale */
    int               ev_tsc;              /**< Number of events already ocurred in the actual time scale */
    TGFLOAT           inter_time_tsc;      /**< Time between events in the actual time scale */
    int               init_OK;             /**< Indicates if the initialization/generation of FBM was successfull */
} FARIMA_Distrib;



#endif  /* __DISTRIBUTION_H__ */
