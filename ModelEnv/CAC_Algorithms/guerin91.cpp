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

#include <iostream>

using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "chained_list.h"
#include "connections_list.h"
#include "admission.h"

t_onoff   *src;             /* Source description */
Admission *admission;       /* Admission object */ 

int usage(char *name)
{

  printf ("Connection Admission Control (CAC) module \n");
  printf ("Usage: ");
  printf ("%s -b <buffer> -c <capacity> -l <lost> > <peak_rate> <rate_off_on> <rate_on_off> \n", name);
  
  printf ("where: \n");
  printf ("\t -b <buffer>\t is the buffer size \n");
  printf ("\t -c <capacity>\t is the link capacity \n"); 
  printf ("\t -l <lost>\t is the lost probability \n"); 
  printf ("\t<peak_rate>\t Peak rate of the connection\n");
  printf ("\t<rate_off_on>\t Lambda - Transition rate from off to on\n");    
  printf ("\t<rate_on_off>\t Mi     - Transition rate from on to off\n");
  return 1;
}



int parse_args(char **argv) 
{
  double       lostprob;
  u_longlong_t buffsize;
  u_longlong_t linkcap;
  int i = 1; 
       
  if ( !(strcmp(argv[i], "-b")) ){
    i++;
    if ( (buffsize = (u_longlong_t) (atol(argv[i]) ) ) < 0) {
      printf("\n\t Error buffer size < 0 \n");
      return(-1);
    }
    i++;
  }
  else{
    printf("\n\t Error buffer size not specified \n");
    return (-1);
  }
       
  if ( !(strcmp(argv[i], "-c")) ){
    i++;
    if ( (linkcap = (u_longlong_t) (atol(argv[i]) ) ) < 0) {
      printf("\n\t Error link capacity < 0 \n");
      return(-1);
    }
    i++;
  }
  else{
    printf("\n\t Error link capacity not specified \n");
    return (-1);
  }
  

  if ( !(strcmp(argv[i], "-l")) ){
    i++;
    if ( (lostprob = (double) (atof(argv[i]) ) ) < 0) {
      printf("\n\t Error lost probability < 0 \n");
      return(-1);
    }
    i++;
  }
  else{
    printf("\n\t Error lost probability < 0 \n");
    return (-1);
  }
  
  src = (t_onoff*)malloc(sizeof(t_onoff));
  
  src->peak_rate   = (double) atof(argv[i]);     /* Pico */
  i++;
  src->rate_off_on = (double) atof(argv[i]);     /* Lambda */  
  i++;
  src->rate_on_off = (double) atof(argv[i]);     /* Mi     */
  
  if ( (admission = new Admission(linkcap, buffsize, lostprob ) ) == NULL )
    {
      perror("new");
      return (-1);
    }
  return (1);
  
}


/*---------------------------------------------------------------------------
MAIN()
---------------------------------------------------------------------------*/
  
int main(int argc, char* argv[])
{

  if ( argc < 9 ){
    usage(argv[0]);
    return -1;
  }
  
   /* check for the number of arguments */
  if ( parse_args(argv ) < 0 ) {
    return (-1);
  }
 
  
  while ( (admission->calc_effective_bandwidth ( *src )) != 0 )
    ;
  
  
  admission->print();

  return 1;
 
}

