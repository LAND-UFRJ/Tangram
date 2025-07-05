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

/* ############################################################################

   Magnos Martinello   Data: 03/08/99
   Modulo de admissão de conexões 
   Ultima atualizacao 09/06/2000
   
 ###########################################################################*/

/*---------------------------------------------------------------------------
  Includes
  ---------------------------------------------------------------------------*/
#include <iostream>

using namespace std;

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "chained_list.h"
#include "connections_list.h"
#include "admission.h"

/* Default Constructor */
Admission::Admission(){
    /* Initialization of atributes */
    no_connect         = 0;
    efec_capacity      = 0;
    mean_agregate_rate = 0;  
    variance_agreg     = 0;
    fluid_capacity     = 0;
    s=Fk=eo=bo=E=w     = 0;

    /*    Connection_List *l; */

    /* List of admitted connections */
    table = new Connection_List();
    /*    l = new Connection_List(); */

    /* Default Required parameters */
    prob_loss     = LOSS_PROB ;
    buff_size     = BUFF_SIZE ;
    link_capacity = LINK_CAPACITY;
    
    /* constants */
    alpha        =  log( 1/prob_loss);
    gauss_aprox  =  sqrt ( (-2*log(prob_loss) - log(2 * M_PI) ) ) ;

}

/* Constructor with configurable parameters */
Admission::Admission(u_longlong_t cap, u_longlong_t buff, double prob){
    /* Initialization of atributes */
    no_connect         = 0;
    efec_capacity      = 0;
    mean_agregate_rate = 0;  
    variance_agreg     = 0;
    fluid_capacity     = 0;
    s=Fk=eo=bo=E=w     = 0;

    /* List of admitted connections */
    table = new Connection_List();

    /* Required parameters */
    prob_loss     = prob ;
    buff_size     = buff ;
    link_capacity = cap;
    
    /* constants */
    alpha        =  log( 1/prob_loss);
    gauss_aprox  =  sqrt ( (-2*log(prob_loss) - log(2 * M_PI) ) ) ;
    
}

/* Destructor */
Admission::~Admission(){
  delete table;
}


/* This method calculates the approximation based on large deviation theory to have a refinement */
   
double Admission::regul_sigma( void ){

  double aux1, aux2;
  
  aux1 = (1 - w) * w*eo*eo * exp(s * eo);
  aux2 = pow( ((1 -w) + eo * exp(s *w)), 2 );
  
  return sqrt(aux1 / aux2 );
}


/* This method calculates the probability of overflow for a specific 
on_off regulated source */

uint Admission::regul_calc_prob_overflow( double peak, double mean, double mbs )
{
  
  double log_loss_required, aux;

  /* Ton  - transmition average time  */
  /* Toff - idle average time  */
  /* Tbuff - maximum delay time of the buffer */
  double Ton, Toff, Tbuff, a;

  /* K is the total number of admited sources for just one service class 
     and a required loss probability */
  uint K;
  
    
  Ton = mbs/ (peak - mean);

  Toff = mbs / mean;
  
  Tbuff = (double) buff_size / link_capacity ;

  if ( mean < (double) ( mbs*link_capacity/buff_size) ){
    
    aux = (double) buff_size/link_capacity;
    aux = aux * (double) (peak-mean)/mbs;
    aux = aux + 1;
    eo = (double) (peak/aux) ;
  }
  else
    eo = mean;
  
  w = (double) mean / eo ;
       
  /* We choose the parameter more liberal, recommended if the margin of violation is small */
  bo = (double) mean / link_capacity;
  
  /* We have to each value of K, a value to s* for just one service class.
     We have also a new value to the function Fk(s*), that is the upper 
     bound to overflow probability*/
  log_loss_required = log(1/prob_loss);

  Fk = link_capacity/eo * log(1/w);

  if ( log(1/prob_loss) >= ( link_capacity/eo * log(1/w) ) )
    {
      K = (uint) (link_capacity/eo);
      no_connect         = K - 1;
      efec_capacity      = eo;
      return 0;
    }
  else
    {
      K = (uint) (link_capacity/eo) + 1;
      
      while ( (Fk > log_loss_required) && ( K < ((1/w)*(link_capacity/eo)) ) ){
	
	a = (double) link_capacity/(eo*K);
	
	s = (1/eo) * (log ( (a/(1-a)) * ((1-w)/w) ) );  
	
	Fk =  K * ( (a * log( (a/w) ) ) + ((1-a) * log( ( (1-a)/(1-w) ) ) ) ); 
	
	K++;
	
      }
      
      printf("\n\t Ton   %.5f",Ton);
      printf("\n\t Toff  %.5f",Toff);  
      printf("\n\t Tbuff %.5f",Tbuff);  
      printf("\n\t w     %.5f",w);  
      printf("\n\t eo    %.5f",eo);  
      printf("\n\t bo    %.5f",bo);  
      printf("\n\t s     %.5f",s);  
      printf("\n\t Fk    %.5f",Fk);  
      printf("\n\t Buff  %lld",buff_size);  
      printf("\n\t link  %lld",link_capacity);  
    }
  
  no_connect         = K - 1;
  efec_capacity      = eo;
  return 1;
}


/* This method shows the efective bandwidth to different Links Capacities */  
uint Admission::regul_calc_statistical_effective_band(u_longlong_t C ){
    
  E = log( 1 - w + w*exp(s*eo) ) /  (s + (double) log(prob_loss)/C);
  
  printf( "\n\t Efective_Band = %.5f ",E);
  
  return 1;
  
}

uint Admission::regul_calc_prob_loss ( void ){ 
  
  /* Using Large Deviations Approximations */
  double Ploss, aux;
  
  aux = s * regul_sigma();
  aux = aux * sqrt(2*M_PI);
  Ploss =  exp(-Fk) / aux;
 
  printf("\n \t Ploss based on large deviations approximations = %.10f \n",Ploss);
  
  return 1;
  
}
  

  /* This method calculates the quantity of bandwidth for a on_off source.
     This approach is extract of a fluid model proposed by Guerin
     at al in 1991  */
uint Admission::calc_effective_bandwidth( t_onoff newsource )
{
    
    t_parameters i_src;      /* parameters to the ith source */
    double i_effec_cap;      /* effective capacity of the ith source */
    double gauss_effec_cap;
    double aux1 = 0, aux2 = 0;
    
    /* Calculing the effective capacity of the ith source */
    i_src.mean_burst_size = 1/newsource.rate_on_off;
    
    i_src.utilization = 
	newsource.rate_off_on/ ( newsource.rate_on_off + newsource.rate_off_on );
    
    i_src.mean_rate = i_src.utilization * newsource.peak_rate;
    
    i_src.variance_rate = 
	i_src.utilization * newsource.peak_rate  * newsource.peak_rate 
	* ( 1 - i_src.utilization ) ;
    
    aux1= alpha * i_src.mean_burst_size * ( 1 - i_src.utilization )* newsource.peak_rate - buff_size ;
    
    aux2= pow ( aux1, 2);
    
    aux2 = aux2 + ( 4*buff_size*alpha*i_src.mean_burst_size*i_src.utilization * 
		    (1 - i_src.utilization) * newsource.peak_rate ) ;
    
    aux2 = sqrt (aux2);
    
    aux2 = aux2 + aux1;
    
    i_effec_cap = aux2 / ( 2 * alpha * i_src.mean_burst_size * 
			   ( 1 - i_src.utilization) ) ;
    
    fluid_capacity += i_effec_cap;

    /* Effective capacity using gaussian aproximation */
    mean_agregate_rate +=  i_src.mean_rate;
    variance_agreg     +=  i_src.variance_rate;
    
    gauss_effec_cap =  mean_agregate_rate + (sqrt(variance_agreg) * gauss_aprox);
    
    /* printf( " Fluid_capacity = %.2f \n", fluid_capacity );  
       printf( " Gauss_capacity = %.2f \n", gauss_effec_cap );   */
    
    /* Equivalent capacity is the minimum of aproximations */
    if ( gauss_effec_cap < fluid_capacity )
      efec_capacity  = gauss_effec_cap;
    else 
      efec_capacity = fluid_capacity;
    


      /* Admission decision */
    if ( efec_capacity <= link_capacity ){
      /* Insert the new connection in the table -> 1 based */
      
      no_connect++;
      
      add_connection( no_connect, i_effec_cap, i_src.mean_rate, i_src.variance_rate) ;
      return 1;
    }
    else
      {
	no_connect --;
	efec_capacity -= i_effec_cap;
	return 0;
      }
}


void Admission::add_connection( uint id, double cap, double mean, double variance ){
    /* Insert a connection in the table */
    table->add_connec(id,cap,mean,variance);
}

uint Admission::show_no_connect(){
  return no_connect;
}

uint Admission::remove_connection( uint id ){
    
    Connection *aux;
    double gauss_effec, effec_cap;
    
    if ( !table->query_connec( id ) )
	return 0;

    /* Remove the element of the list and subtract the agregate parameters */
    if (  (aux = table->del_elem_connec()) != NULL ){
      
      /* aux->print(); */
      
      /* Calculus of the gaussian aproximation */
      gauss_effec =  aux->show_mean() + (sqrt(aux->show_variance()) * gauss_aprox);
      effec_cap   = aux->show_capacity();
      
      fluid_capacity     -= effec_cap;
      mean_agregate_rate -= aux->show_mean();
      variance_agreg     -= aux->show_variance();
	
      if ( effec_cap < gauss_effec )
	efec_capacity -= effec_cap;
      else
	efec_capacity -= gauss_effec;
      
      no_connect--;
      return 1;
    }
    
    else{
	printf(" Connection %d not exist",id );
	return 0;
    }
    
}

void Admission::print(){
    
    printf("\n \t Number of admitted connections = %d\n",no_connect+1);
    printf("\t Agregated Efective Capacity = %.2f \n", efec_capacity );  
    return ;
}

