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


/************************************************************************/
/*									*/
/*		Routine for the calculation of the Poisson 		*/
/*		distribution.						*/
/*									*/
/*              Joao Guedes UCLA CSD 2/12/94				*/
/*									*/
/************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sparseMatrix.h"
#include "bounds.h"


/********************************************************************/
/* 								    */
/* evaluates minimum N such that there is no under/over flow        */
/*								    */
/********************************************************************/
void lo_bound(n, val, rate_t)
int    *n;
double *val,rate_t;
{
    int    left, right;
    double e, i, prod,num;


    num = MAX(0, ceil(rate_t - 10.0 * sqrt(rate_t)));
    *n = (int)num;
    
    if (*n == 0)
        *val=exp(-rate_t);
    else
    {
        e = rate_t * exp(-rate_t/(*n+1));
        
        i = ceil(e);
        left = (int)i;
        right = (int)i;
        prod = e/i;
        
        while ((left > 1) && (right < *n))
	{
	    while ((prod <= 1) && (left > 1))
	    {
	        left--;
	        prod *= e/left;
	    }
	    
	    while ((prod >= 1) && (right < *n))
	    {
	        right++;
	        prod *= e/right;
	    }
	}
	
        while (left > 1)
	{
	    left--;
	    prod *= e/left;
	}
	
        while (right < *n)
	{
	    right++;
	    prod *= e/right;
	}
	
        *val = exp(-rate_t/(*n+1)) * prod;
    }
}


/********************************************************************/
/* 								    */
/* Evaluates minimum N such that there is no under/over flow.       */
/* 								    */
/* Updated by Sidney Lucena to allow other precision errors.        */
/* Used in the evaluation of N_min for descriptors functions        */
/* evaluation.                                                      */
/*								    */
/********************************************************************/
void lo_bound2(n,val,rate_t,error)
int *n;
double *val,rate_t;
double error;
{
int left,right;
double e,i,prod,num;
double Z_err = 0.0;

if (error >= 1.0e-33)
    Z_err = 12.0; 
else
if ((error >= 1.0e-39) && (error < 1.0e-33))
    Z_err = 13.0; 
else
if ((error >= 1.0e-45) && (error < 1.0e-39))
    Z_err = 14.0; 
else
if ((error >= 1.0e-51) && (error < 1.0e-45))
    Z_err = 15.0; 
else
if ((error >= 1.0e-58) && (error < 1.0e-51))
    Z_err = 16.0; 
else
if ((error >= 1.0e-65) && (error < 1.0e-58))
    Z_err = 17.0; 
else
if ((error >= 1.0e-73) && (error < 1.0e-65))
    Z_err = 18.0; 
else
if (error < 1.0e-73)
{
    fprintf(stderr,"Error is too much low! Try to reescale rewards.\n");
    exit (82);
}

num = MAX(0,ceil(rate_t - Z_err * sqrt(rate_t)));

*n = (int)num;

if (*n==0)
   *val=exp(-rate_t);
else
    {
    e=rate_t*exp(-rate_t/(*n+1));
    i=ceil(e);
    left=(int)i;
    right=(int)i;
    prod=e/i;
    while ((left>1) && (right<*n))
	  {
	  while ((prod<=1) && (left>1))
		{
		left--;
		prod*=e/left;
		}
	  while ((prod>=1) && (right<*n))
		{
		right++;
		prod*=e/right;
		}
	  }
    while (left>1)
	  {
	  left--;
	  prod*=e/left;
	  }
    while (right<*n)
	  {
	  right++;
	  prod*=e/right;
	  }
    *val=exp(-rate_t/(*n+1))*prod;
    }
}



/************************************************************************/
/*									*/
/*		Input:							*/
/*			expo, epsilon.					*/
/*		Output:							*/
/*			N such that					*/
/*									*/
/*	sum_{n=0}^N exp(-expo) (expo)^n/n! > 1 - epsilon		*/
/*									*/
/************************************************************************/
int partial_poisson_sum(expo, epsilon)
double expo;
double epsilon;
{
	void lo_bound();
	int n_min;
	double val_n_min;
	int N;					/* number of terms one needs */
	double sum;				/* partial sum               */
	double prod;			        /* typical term              */
	double poisson = exp(-expo);		/* common exponent factor    */
	double tolerance = 1.0 - epsilon;		/* least Poisson sum we need */

	
#ifdef DEBUG_DESC
printf ("Epsilon = %.6e  Tolerance = %.6e\n", epsilon, tolerance);
printf ("Exponential term = %.6e\n", expo);
#endif
	
	if (poisson == 1) 
	{	/* the sum will not converge, ever */
		fprintf(stderr, "Time interval may be invalid. ");
		fprintf(stderr, "Check if it is zero\n");
		exit(88);
	}
	
	if (tolerance >= 1)
	{	/* the sum will not reach it */
		fprintf(stderr, "---> Precision value cannot be achieved!!! It must be raised.\n");
		exit(82);
	}

	lo_bound(&n_min,&val_n_min,expo);
	sum = val_n_min;
	prod = val_n_min;
	N = n_min;
	while (sum < tolerance)
	{
	    if (N == 30000)
	    {
	        fprintf(stderr, "---> Maximum number of iterations is too big! Precision value must be raised\n");
	        exit(83);
  	    }
  	    
            N++;
            
	    prod *= expo/N;
	    sum += prod;

#ifdef DEBUG_DESC
printf("Sum = %.6e\n", sum);
#endif
	}

	return (N);         /* return the number of terms (N >= 0) needed */
}


