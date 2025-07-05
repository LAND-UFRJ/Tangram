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

/******************************************************************************
	
	Author: Antonio Augusto Rocha
	Tool: Traffic Generator
	LAND - Laboratorio de Analise e Desempenho - UFRJ/COPPE/COS
        Last Update: 12/01/2004

******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #ifndef MAXDOUBLE
    #define MAXDOUBLE DBL_MAX
  #endif
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include "linear.h"



/* global variables */

/* flag for number of parameters */
int num_p;

/* name of traces' file */
char   file1[MAXSTRING], file2[MAXSTRING];

/* files VAR to analize */
FILE *fp, *outfp;

char strfile[MAXLINE];

/*calc loss packet pair*/
char opt_loss[MAXLINE];

long int size_trace;

t_aix *aix;
t_aix *stack;

/* get to return of linear function*/
double linear_y1, linear_y2;



int usage(char *filename)
{
    printf ("Traffic Generator v3.0 - Copyright (C) 1999-2009\n");
    printf ("Federal University of Rio de Janeiro - UFRJ/COPPE/COS\n");
    printf ("LAND - Laboratory for Modeling and Development of Networks \n");
    printf ("Usage: \n");
    printf ("linear [-P|L] <file1> [<file2>]\n");
    printf ("where:\n");
    printf ("\t [-P|L]   \t\t Used for Packet pair or Loss pair mode\n  ");
    printf ("\t <file1>  \t\t File with first trace\n");
    printf ("\t[<file2>] \t\t File with second trace\n");    
    
    return (1);
}


int parse_args(char **argv, int num_arg) 
{
  int i;
  
  i = 1;

  if ( num_arg == 2 )
  {
    strcpy (file1, argv[i]);
  }
  if ( num_arg == 3 )
  {
    strcpy (file1, argv[i]);
    i++;
    strcpy (file2, argv[i]);
  }
  if ( num_arg == 4 )
  {
    strcpy (opt_loss, argv[i]);
    i++;
    strcpy (file1, argv[i]);
    i++;
    strcpy (file2, argv[i]); 
  }
    
  return (1);
}

/* Functions used to calculate Y Below*/
/*-----------------------------------------------------------------*/
int below( long int top_stack, long int cont_trace)
{
   double result_y = 0, result_m = 0;
   
   
   result_m = (long double)( stack[top_stack-2].y_aix -
   stack[top_stack-1].y_aix) / (long double) ( stack[top_stack-2].x_aix - stack[top_stack-1].x_aix );
  
     
   result_y = ( ((long double) result_m * (long double)( aix[cont_trace-1].x_aix - stack[top_stack-1].x_aix )) + stack[top_stack-1].y_aix ); 

   if (result_y > aix[cont_trace-1].y_aix)
   {
     return (1);
   }
   else
   {
     return (-1);
   }
   

}

/* Functions used to calculate Y Above*/
/*-----------------------------------------------------------------*/
int above( long int top_stack, long int cont_trace)
{
   double result_y = 0, result_m = 0;
   
   
   result_m = (long double)( stack[top_stack-2].y_aix - stack[top_stack-1].y_aix) / (long double) ( stack[top_stack-2].x_aix - stack[top_stack-1].x_aix );
  
     
   result_y = ( ((long double) result_m * (long double)( aix[cont_trace-1].x_aix - stack[top_stack-1].x_aix )) + stack[top_stack-1].y_aix ); 

   if (result_y < aix[cont_trace-1].y_aix)
   {
     return (1);
   }
   else
   {
     return (-1);
   }
   

}


/* Functions used to pop from the stack */
/*-----------------------------------------------------------------*/
int pushStack( long int top_stack, long int cont_trace)
{ 

  if( ( stack = (t_aix *)realloc( stack, top_stack * sizeof(t_aix) ) ) == NULL )
  {
      fprintf( stderr, "\n\n\tError: Memory allocation!" );
      return( -1 );
  }
  stack[top_stack-1].z_aix = aix[cont_trace-1].z_aix;  
  stack[top_stack-1].x_aix = aix[cont_trace-1].x_aix;
  stack[top_stack-1].y_aix = aix[cont_trace-1].y_aix;
  
  return 0;

}  

/* Functions used to push in the stack */
/*-----------------------------------------------------------------*/
int popStack( long int top_stack )
{

  if( ( stack = (t_aix *)realloc( stack, top_stack * sizeof(t_aix) ) ) == NULL )
  {
      fprintf( stderr, "\n\n\tError: Memory allocation2!" );
      return( -1 );
      
  }
  return 0;
}  

/* Functions used to remove offset from traces*/
/*-----------------------------------------------------------------*/
int remOffset( char file[MAXSTRING], double offset, int flag)
{

   long long int x_aux, y_aux;
   int z_aux;
   char aux_file[MAXSTRING], result_file[MAXSTRING];
   int cont_trace, cont_file;


  size_trace=0; cont_trace=0; cont_file=0; 

  for (cont_file=1; cont_file<=4; cont_file++)
  {
     strcpy(aux_file, "");
     strncat(aux_file, file, strlen(file)-7);
     strncat(aux_file, "xxxx", cont_file);

     /* Opening input file for reading */
     if( ( fp = fopen(aux_file,"r") ) == NULL )
     {
	 fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", aux_file );
	 return( -1 );
     } 

     if ( num_p <= 3 )
     {
	strcpy(aux_file, "");
	strncat(aux_file, file, strlen(file)-11); 
	strcpy(result_file, "");
	sprintf( result_file, "%s%i.trace", aux_file, cont_file); 
     } else
     if ( num_p == 4 )
     {

	if ((strcmp(opt_loss,"-l")==0)||(strcmp(opt_loss,"-L")==0))
	{
	   strcpy(aux_file, "");
	   strncat(aux_file, file, strlen(file)-16);
	   strcpy(result_file, "");
	   sprintf( result_file, "%s%i.loss", aux_file, cont_file); 	
	}
	if ((strcmp(opt_loss,"-p")==0)||(strcmp(opt_loss,"-P")==0))
	{
	   strcpy(aux_file, "");
	   strncat(aux_file, file, strlen(file)-14);
	   strcpy(result_file, "");
	   sprintf( result_file, "%s%i.pair", aux_file, cont_file); 	
	}     
     
     }

     /* Opening file for output */
     if( ( outfp = fopen(result_file,"w+") ) == NULL )
     {
	 fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", aux_file );
	 return( -1 );
     } 

     while( ! feof( fp ) )
     {
	 if( fgets( strfile, MAXLINE, fp ) != NULL )
	 {
	    if( (sscanf(strfile, "%d %lld %lld", &z_aux, &x_aux, &y_aux )) == 3 )
	    {
	       fprintf(outfp, "%d \t", z_aux);
	       fprintf(outfp, "%lld \t", x_aux);

	       if ( flag == 1 )
	       {
                  fprintf(outfp, "%.0lf \n", y_aux - offset);
               }
	       if ( flag == 2 )
	       {
                  fprintf(outfp, "%.0lf \n", y_aux + offset);
               }

            }
         }
     }
     fclose( outfp );
     fclose( fp );
  }
  return (0);
}


/* Functions used to get linear equation */
/*-----------------------------------------------------------------*/
int getLine( char file[MAXSTRING], int argc )
{

   long long int x_aux, y_aux;
   int z_aux;
   long int cont_trace=0, top_stack=0;
   int i;
   double obj=0;
   int num_eq;
   char aux_file[MAXSTRING];


    num_eq     = 0;
    size_trace = 0;
    strcpy( aux_file, file );

    /* Opening input file for reading */
    if( ( fp = fopen(file,"r") ) == NULL )
    {
        fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
        return( -1 );
    } 

    aix = NULL;  

  while( ! feof( fp ) )
  {
      if( fgets( strfile, MAXLINE, fp ) != NULL )
      {
	   if( (sscanf(strfile, "%d %lld %lld", &z_aux, &x_aux, &y_aux )) == 3 )
	   {
	     size_trace++;
             if( ( aix = (t_aix *)realloc( aix, size_trace * sizeof(t_aix) ) ) == NULL )
             {
        	 fprintf( stderr, "\n\n\tError: Memory allocation!" );
        	 return( -1 );
             }
             aix[size_trace-1].x_aix = x_aux;
             aix[size_trace-1].y_aix = y_aux;
	     aix[size_trace-1].z_aix = z_aux;
	 }
      }
  }
  
  fclose( fp );


  stack = NULL;
  cont_trace++;
  top_stack++;

  pushStack( top_stack, cont_trace );
    
  cont_trace++;
  top_stack++;
  
  pushStack( top_stack, cont_trace );

  cont_trace++;

  while ( cont_trace <= size_trace )
  {
    if ( above( top_stack, cont_trace ) > 0)
    {
      top_stack++;
      pushStack( top_stack, cont_trace );
      cont_trace++;
    } else
    {
      while ((below( top_stack, cont_trace ) > 0)&&( top_stack > 1 ))
      {
	 top_stack--;     
	 popStack( top_stack );
      }
      top_stack++;
      pushStack( top_stack, cont_trace );
      cont_trace++;
    }
  }



  strncat(file, ".stack", 6);

  /* creating the stack file*/
  if( ( fp = fopen(file,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  }
  
  if ( argc == 2 )
  {
     obj = ( (double)( aix[0].x_aix + aix[size_trace-1].x_aix ) ) / 2;
     for ( i=1; i<=top_stack; i++)
     {
       if ( ( obj > stack[i-1].x_aix ) && ( obj < stack[i].x_aix ) )
       {
	 num_eq = i;
       }
       fprintf(fp, "%lld\t%lld\n", stack[i-1].x_aix ,stack[i-1].y_aix );
     }
  }
  else if ( argc >= 3 )
  {
     obj = ( (double)( aix[0].z_aix + aix[size_trace-1].z_aix ) ) / 2;
     for ( i=1; i<=top_stack; i++)
     {
       if ( ( obj > stack[i-1].z_aix ) && ( obj < stack[i].z_aix ) )
       {
	 num_eq = i;
       }
       fprintf(fp, "%d\t%lld\n", stack[i-1].z_aix ,stack[i-1].y_aix );
     }
  }
  fclose(fp);
  
  strcpy(file, aux_file);
  
  strncat(file, ".eq", 3);

  /* creating the equation file*/
  if( ( fp = fopen(file,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  }

  if ( argc == 2 )
  {
     fprintf(fp, "%lld\t%lld\n", stack[num_eq-1].x_aix ,stack[num_eq-1].y_aix );
     fprintf(fp, "%lld\t%lld\n", stack[num_eq].x_aix ,stack[num_eq].y_aix );  
  }
  else if ( argc == 3 )
  {
     fprintf(fp, "%d\t%lld\n", stack[num_eq-1].z_aix ,stack[num_eq-1].y_aix );
     fprintf(fp, "%d\t%lld\n", stack[num_eq].z_aix ,stack[num_eq].y_aix );  
  }
  fclose(fp);  

  strcpy(file, aux_file);

  return (num_eq); 
  
}
/*-----------------------------------------------------------------*/


/* Functions used to get linear equation */
/*-----------------------------------------------------------------*/
int remSkew( char file[MAXSTRING], int num_eq )
{

  double result_y = 0, result_m = 0;
  double coord_y = 0, new_y = 0;
  long int cont_trace;
  char aux_file[MAXSTRING];
  

   
  strncat(file, ".skew", 5);
  /* Creating output file for writing */
  if( ( fp = fopen(file,"w+") ) == NULL )
  {
      fprintf( stderr, "\n\tError: Can't open file '%s' or file don't exist\n\n", file );
      return( -1 );
  }
  for ( cont_trace = 1; cont_trace <= size_trace; cont_trace++ )
  {
    result_m = (double)( stack[num_eq-1].y_aix - stack[num_eq].y_aix ) / (double) ( stack[num_eq-1].x_aix - stack[num_eq].x_aix );
    result_y = ( ((double) result_m * (double)( aix[cont_trace-1].x_aix - stack[num_eq].x_aix )) + stack[num_eq].y_aix ); 
    coord_y = ( ((double) result_m * (double)( aix[0].x_aix - stack[num_eq].x_aix )) + stack[num_eq].y_aix ); 
    
    //new_y = (((double) aix[cont_trace-1].y_aix) - ((double) result_y ) + ((double) coord_y ));
    new_y = aix[cont_trace-1].y_aix - ((double)(aix[cont_trace-1].x_aix - aix[0].x_aix) * (double)result_m);


    fprintf( fp, "%d \t %lld \t %.0f\n", aix[cont_trace-1].z_aix, aix[cont_trace-1].x_aix, new_y ); 

  }

  fprintf( stdout, "start_x: %lld\n", aix[0].x_aix );
  fprintf( stdout, "alfa: %1.10f\n", result_m );
  
  fclose(fp);
  strcpy(file, aux_file);
  
  return ( 0 );

}

/* Functions used to get linear equation */
/*-----------------------------------------------------------------*/
double getYAIX( int num_eq )
{

  double result_m = 0, coord_y = 0;

  result_m = (long double)( stack[num_eq-1].y_aix - stack[num_eq].y_aix ) / (long double) ( stack[num_eq-1].z_aix - stack[num_eq].z_aix );
  coord_y = ( ((long double) result_m * (long double)( 0 - stack[num_eq].z_aix )) + stack[num_eq].y_aix ); 

  return ( coord_y );

}

int main (int argc, char **argv)
{

   int result_get;
   double offset=0;
   char off_file[MAXLINE];
   
   
  /* check for the number of arguments */
  if ( argc < 2 ) {
    usage(argv[0]);
    return (-1);
  }

  /* parse the arguments */
  if (parse_args(argv, argc) < 0) {
    usage(argv[0]);
    return (-1);
  }

  num_p = argc;

  if ( argc == 2 )
  {

    result_get = getLine( file1, argc );
    remSkew ( file1, result_get );
    free( aix );    
  }
  else if ( argc == 3 )
  {
    result_get = getLine( file1, argc );
    linear_y1 = getYAIX ( result_get );
    free( aix );
    result_get = getLine( file2, argc );
    linear_y2 = getYAIX ( result_get );
    free( aix );
    
    if ( linear_y1 > linear_y2 )
    {
       offset = (long double)((long double) linear_y1 - (long double) linear_y2 ) / 2;
       remOffset( file1, offset, 1);
       remOffset( file2, offset, 2);
    }
    else
    {
       offset = (long double)((long double) linear_y2 - (long double) linear_y1 ) / 2;
       remOffset( file2, offset, 1);
       remOffset( file1, offset, 2);
    }

    printf( "offset: %.0f \n", offset);
        
  }
  else if ( argc == 4 )
  {

     if ((strcmp(opt_loss,"-p")==0)||(strcmp(opt_loss,"-P")==0))
     {

	 result_get = getLine( file1, argc );
	 linear_y1 = getYAIX ( result_get );
	 free( aix );
	 result_get = getLine( file2, argc );
	 linear_y2 = getYAIX ( result_get );
	 free( aix );

	 if ( linear_y1 > linear_y2 )
	 {
	    offset = (long double)((long double) linear_y1 - (long double) linear_y2 ) / 2;
	    remOffset( file1, offset, 1);
	    remOffset( file2, offset, 2);
	 }
	 else
	 {
	    offset = (long double)((long double) linear_y2 - (long double) linear_y1 ) / 2;
	    remOffset( file2, offset, 1);
	    remOffset( file1, offset, 2);
	 }

     } else

     if ((strcmp(opt_loss,"-l")==0)||(strcmp(opt_loss,"-L")==0))
     {
            
	result_get = getLine( file1, argc );
	linear_y1 = getYAIX ( result_get );
	free( aix );
	result_get = getLine( file2, argc );
	linear_y2 = getYAIX ( result_get );
	free( aix );

	if ( linear_y1 > linear_y2 )
	{
	   offset = (long double)((long double) linear_y1 - (long double) linear_y2 ) / 2;
	   num_p = 3; 
	   remOffset( file1, offset, 1);
	   remOffset( file2, offset, 2);
	   num_p = 4; 
	   strcpy (off_file, "");
	   strncat( off_file, file1, strlen(file1)-7);
	   strncat( off_file, ".loss.offset", 12 );
	   remOffset( off_file, offset, 1);
	   strcpy (off_file, ""); 
	   strncat( off_file, file2, strlen(file2)-7);
	   strncat( off_file, ".loss.offset", 12 );	  
	   remOffset( off_file, offset, 2);       

	}
	else
	{
	   offset = (long double)((long double) linear_y2 - (long double) linear_y1 ) / 2;
	   num_p = 3; 
	   remOffset( file2, offset, 1);
	   remOffset( file1, offset, 2);
	   num_p = 4; 
	   strcpy (off_file, "");
	   strncat( off_file, file2, strlen(file2)-7);
	   strncat( off_file, ".loss.offset", 12 );
	   remOffset( off_file, offset, 1);
	   strcpy (off_file, "");
	   strncat( off_file, file1, strlen(file1)-7);
	   strncat( off_file, ".loss.offset", 12 );		  	      
	   remOffset( off_file, offset, 2);      
	}
     }
  }
     
  return( 0 );
}
