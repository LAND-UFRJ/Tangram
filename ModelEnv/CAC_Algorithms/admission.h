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

#ifndef __ADMISSION_H__
#define __ADMISSION_H__

/* Some constants */
#define LOSS_PROB        0.00001        /* probabilidade de perda desejada */
#define BUFF_SIZE        3145728         /* 3 Mbits */
#define LINK_CAPACITY    162529280       /* 155 Mbits/s */

#ifndef __UINT__
#define __UINT__
typedef unsigned int uint;
#endif

#ifndef __ULONGLONG__
#define __ULONGLONG__
typedef unsigned long long u_longlong_t;
#endif

#ifndef __TONOFF__
#define __TONOFF__

/* traffic descriptor of  Markov type with 2 states*/
typedef struct{
    double rate_off_on;              /*Taxa de nascimento */
    double rate_on_off;               /*Taxa de morte */
    double peak_rate;                /*Taxa de pico */
} t_onoff;
#endif


/* Parameters used at admission decision */
typedef struct{
    double mean_rate;                /* mean_rate */
    double variance_rate;            /* variance_rate */
    double mean_burst_size;
    double utilization;
} t_parameters;

/* Class Admission which implements admission decision to a Markovian source */
class Admission{

/* The parameters s and Fk are calculetes to just a single service class */

    double s;      
/* s is the point which the function Fk is maximized */
/*    s = (1/eo) * (log( ( (a/1-a) / (1-w/w) ) ) ); */

/* Parameter s* maximizes the function to upper bound of 
     overflow probability Fk(s*) following the notation of Elwalid at al */

    double Fk;  
/* Fk is the function to estimate the buffer overflow */
/*    Fk =  K * ( (a * log( (a/w) ) ) + ((1-a) * log( ( (1-a)/(1-w) ) ) ) ); */

    double eo, bo, w, E;
/* eo is a fraction of total capacity C with lossless  multiplexing */
/* E is a fraction of total capacity C with statistical multiplexing */
/* bo is a fraction of total buffer size X */
/* w is a m/e0 */ 
/* w is the fraction of system buzy */


    uint   no_connect;          /* Number of admitted connections */
    double efec_capacity;        /* Efective Capacity */ 
    double mean_agregate_rate;  
    double variance_agreg;
    double fluid_capacity;       /* Efective capacity for Fluid aproximation */
    double alpha;                /* alpha = ln ( 1/ Loss_Probability ) */
    double gauss_aprox;          /* Parameter to gaussian aproximaxion */
    
    double       prob_loss;      /* Qos - Required Lost probability  */
    u_longlong_t buff_size;      /* Size of ATM buffer node */
    u_longlong_t link_capacity;  /* Transmission capacity of link */
    
    Connection_List *table;      /* Table of admitted connections */

 public:
    Admission();
    Admission(u_longlong_t, u_longlong_t, double);
    ~Admission();

    /*This method uses guerin_91 approach to get the equivalent capacity and bandwidth allocatin in High-Speed Networks*/ 
    uint calc_effective_bandwidth( t_onoff );

    
    /*These methods use Elwalid and Mitra_95 approach to heterogeneous regulated traffics by leaky bucket in an ATM Node*/
    uint   regul_calc_prob_overflow ( double, double, double );   
    double regul_sigma( void );
    uint   regul_calc_statistical_effective_band( u_longlong_t );
    uint   regul_calc_prob_loss ( void );

    void add_connection( uint, double, double, double );
    uint remove_connection( uint );
    uint show_no_connect(); 
    void print();
};


    

#endif /*  __ADMISSION_H__ */
