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

/**
*	@file distribution.cpp
*   @brief <b> Events distribution </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

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

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_desc.h"
#include "system_state.h"
#include "random.h"

#include "distribution.h"


//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
//  Constructor receives a seed parameter for random number generation
//------------------------------------------------------------------------------
Distribution_Numeric::Distribution_Numeric(Random_obj *random_obj)
{
  the_random_obj = random_obj;
  debug(4,"Distribution_Numeric::Distribution_Numeric(Random_obj *random_obj): creating object");
}
//------------------------------------------------------------------------------
// Destructor for Distribution numeric
//------------------------------------------------------------------------------
Distribution_Numeric::~Distribution_Numeric()
{
  /* do nothing */
  debug(4,"Distribution_Numeric::~Distribution_Numeric(): destroying object");
}
//------------------------------------------------------------------------------
// Constructor: Initializes the object an receives the seed for internal
// random number generation
//------------------------------------------------------------------------------
Gaussian_Distrib_Numeric::Gaussian_Distrib_Numeric (Random_obj *random_obj) : Distribution_Numeric (random_obj)
{
  mean     = 0;
  variance = 1;
  debug(4,"Gaussian_Distrib_Numeric::Gaussian_Distrib_Numeric(Random_obj *random_obj): creating object");
}
//------------------------------------------------------------------------------
// Destructor for Gaussian_Distrib_Numeric
//------------------------------------------------------------------------------
Gaussian_Distrib_Numeric::~Gaussian_Distrib_Numeric()
{
  debug(4,"Gaussian_Distrib_Numeric::~Gaussian_Distrib_Numeric(): destroying object");
}
//------------------------------------------------------------------------------
// Randval(): generates a random number in [0,1] range based on the
// seed previously passed in the constructor
//------------------------------------------------------------------------------
TGFLOAT Distribution_Numeric::randval()
{
  TGFLOAT value;
  
  value = the_random_obj->next_randval(); /* U[0,1] */
  
  return ( value );

}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Next_sample(): generates a new sample of a Gaussian distributed random variable
//------------------------------------------------------------------------------
TGFLOAT Gaussian_Distrib_Numeric::next_sample()
{
  TGFLOAT v1,v2,w, Gvar;

  do
    {
      v1 = 2 * randval() - 1;
      v2 = 2 * randval() - 1;
      w  = v1*v1 + v2*v2;
    }
  while(w>1);

  Gvar = v1*(sqrt(-2*(log(w)/w)));      /* Gaussian random variable (0,1) */

  Gvar = mean + sqrt( variance ) * Gvar;    /* Gaussian random variable (mean, variance) */

  return( Gvar );
}
//------------------------------------------------------------------------------
// Show_type(): Returns the type of distribution
//------------------------------------------------------------------------------
int Gaussian_Distrib_Numeric::show_type()
{
  return (GAUSSIAN_DIST_NUM);
}
//------------------------------------------------------------------------------
// Acessor method for the attribute mean
//------------------------------------------------------------------------------
int  Gaussian_Distrib_Numeric::ch_mean(TGFLOAT new_mean )
{
  mean = new_mean;
  return(0);
}
//------------------------------------------------------------------------------
// Acessor method for the attribute variance
//------------------------------------------------------------------------------
int  Gaussian_Distrib_Numeric::ch_variance(TGFLOAT new_variance)
{
  variance = new_variance;
  return(0);
}
//------------------------------------------------------------------------------
void Gaussian_Distrib_Numeric::print_dist()
{
  fprintf( stdout , "GAUSSIAN_DIST_NUM ");
  fprintf( stdout , "Mean: %.10E\n", mean);
  fprintf( stdout , "Variance: %.10E\n", variance);
}
//------------------------------------------------------------------------------



/*************************************************************/

/*
Constructor: Initializes the object
*/

Sample_File::Sample_File()
{
  sample_index     = 0;
  max_sample_index = 0;
  EOF_reached      = 0;
  last_cache_page  = 0; 
  sample_cache     = NULL;
  file_status      = CLOSED; 
  sample_cache     = NULL;
  cache_length     = 1000;
  fd               = NULL;
  
  debug(4,"Sample_File::Sample_File(): creating object");
}


/*
Destructor: Releases memory and closes the file descriptor
*/

Sample_File::~Sample_File()
{
  if (sample_cache)
    delete [] sample_cache;
  
  if (fd != NULL)
    fclose(fd);

  debug(4,"Sample_File::~Sample_File(): destroying object"); 

}


/*
Open_r(): open the sample file in READ mode. The method receives
the name of the file  and the cache length to be used by the 
object in future read operations.
*/

int Sample_File::open_r(char* new_name, int new_cache_length)
{
  if (new_name != NULL)
    sprintf(f_name,"%s", new_name);
  else
    return(-1);
  
  sample_index     = 0;
  max_sample_index = 0;
  EOF_reached      = 0;
  last_cache_page  = 0; 
  
  if (fd != NULL)
    fclose(fd);
  
  if((fd=fopen(f_name,"r")) == NULL)
    {
      debug(3, "Sample_File::open_r: Can't open file for reading");
      file_status = CLOSED;
      return(-1); 
    }
  else
    file_status = READ_MODE;
  
  if (sample_cache)
    delete [] sample_cache;
  
  if (cache_length > 0)
    cache_length = new_cache_length;
  
  sample_cache = new TGFLOAT[cache_length];
  
  return(0);
}


/*
Open_w(): open the sample file in WRITE mode. It receives
the name of the file to be created by the object for future
write operations.
*/

int Sample_File::open_w(char* new_name)
{
  if (new_name != NULL)
    sprintf(f_name,"%s", new_name);
  else
    return(-1);

 if (fd != NULL)
    fclose(fd);

  
  if((fd=fopen(f_name,"w")) == NULL)
    {
      debug(3, "Sample_File::open: Can't open file for writing");
      file_status = CLOSED;
      return(-1);
    }
  else
    file_status = WRITE_MODE;
  
  return(0);
}


/*
Read(): this method returns a new sample from the sample file (specified
in the 'open_r()' method). It manages the cache operation and end_of_file 
setting 
*/

TGFLOAT Sample_File::read()
{
  int i;
  TGFLOAT newvalue;
  
  if (file_status != READ_MODE)
    {
      debug(3, "Sample_File::read: Attempt to read a file that is not in READ_MODE ");
      return(-1);       
    }
  
  sample_index++;  
  
  if ((sample_index >  max_sample_index) || (max_sample_index==0))     
    {
      if (EOF_reached)
	return(0);

      for (i=0; i < cache_length; i++)
	{
	  if(fscanf(fd,"%lf",&newvalue) == EOF)
	    {
	      last_cache_page  = 1;
	      break;      
	    }
          else
            sample_cache[i] = newvalue;	  
        }
      
      max_sample_index = i - 1;      
      sample_index = 0;
    }
  
  if ((sample_index == max_sample_index) && (last_cache_page))
    EOF_reached = 1;
  
  return(sample_cache[sample_index]);     
  
}


/*
Write(): Writes a new sample in the sample file (specified
in the 'open_w()' method)
*/

int Sample_File::write(TGFLOAT new_sample)
{
  if (file_status != WRITE_MODE)
    {
      debug(3, "Sample_File::write: Attempt to write a file that is not in WRITE_MODE ");
      return(-1);       
    }
  
  fprintf(fd,"%f\n", new_sample);            
  return(0);
  
}


/*
Close(): Closes the sample file previously opened
*/

int Sample_File::close()
{
  if (fd != NULL)
  {
    fclose(fd);
    fd = NULL;
  }
  
  return(0);
}


/*
Delete(): Deletes the sample file 
*/


int Sample_File::delete_file()
{
  char cmd [MAXSTRING];

  sprintf(cmd, "\\rm -f %s", f_name);

  if(system(cmd) < 0)
    {
      debug(3, "Sample_File::delete_file: Error in file deletion ");
      return(-1);
    }

  close();
  
  return(0);  
  
}


/*
Get_file_mode(): returns the current file mode (READ_MODE, WRITE_MODE, CLOSED)
*/

int Sample_File::get_file_mode()
{
  return(file_status);
}


/*
Rewind(): Returns the "file pointer" to the beginning of the sample file.
Future read operations will be referred (again) to the beginning of file.
*/

int Sample_File::rewind_file()
{
  if (fd != NULL)
    rewind(fd);
  
  sample_index     = 0;
  max_sample_index = 0;
  EOF_reached      = 0;
  last_cache_page  = 0;

  return(0);
} 


/*
Eof(): returns 1 if the end of file was reached and 1 otherwise
*/

int Sample_File::eof()
{
  return(EOF_reached);
}






/*************************************************************/
/*************************************************************/
/*************************************************************/

/*
Constructor. Receives a seed parameter for random number generation 
*/

Distribution::Distribution(Random_obj *random_obj)
{
  the_random_obj = random_obj;
  debug(4,"Distribution::Distribution(Random_obj *random_obj): creating object");     
}


/* 
Destructor for Distribution numeric 
*/

Distribution::~Distribution()
{
  /* do nothing */
  debug(4,"Distribution::~Distribution(): destroying object");  
}


/* 
Randval(): generates a random number in [0,1] range
*/

TGFLOAT Distribution::randval()
{
  TGFLOAT value;
  
  value = the_random_obj->next_randval(); /* U[0,1] */
  
  return ( value );
}




/*************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Exponential_Distrib::Exponential_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
  rate = NULL;
  debug(4,"Exponential_Distrib::Exponential_Distrib(long): creating object");
}


/*
Destructor for Exponential_Distrib  
*/

Exponential_Distrib::~Exponential_Distrib()
{
  debug(4,"Exponential_Distrib::~Exponential_Distrib(): destroying object");
}


/*
Next_sample(): generates a new sample of an Exponential distributed random variable
*/

TGFLOAT Exponential_Distrib::next_sample(Object_State *obj_st) 
{
  TGFLOAT uniform;
  TGFLOAT rate_num;

  uniform = randval();
  rate_num = rate->evaluate_numeric(obj_st);

  return ( -1.0/rate_num * log (uniform) );
}


int Exponential_Distrib::show_type()
{
  return (EXPONENTIAL_DIST);
}


/*
Acessor method for the attribute rate
*/

Expression *Exponential_Distrib::show_rate()
{
  return (rate);
}


/*
Acessor method for the attribute rate
*/

int  Exponential_Distrib::ch_rate(Expression *new_rate)
{
  if (new_rate != NULL)
    rate = new_rate;
  else
    {
     debug(3, "Exponential_Distrib::ch_rate: Can't change rate to NULL pointer");
     return(-1);
    }

  return(0);
}


void Exponential_Distrib::print_dist()
{
   printf ("EXPONENTIAL_DIST - Rate: ");
   rate->print_expression();
}


void Exponential_Distrib::reset(int run)
{
    
    /* do nothing*/
    debug(4,"Exponential_Distrib::reset(): do nothing");
}



/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Erlang_m_Distrib::Erlang_m_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
  rate = NULL;
  stages = NULL;
  debug(4,"Erlang_m_Distrib::Erlang_m_Distrib(long): creating object");  
}


/*
Destructor for Erlang_m_Distrib  
*/

Erlang_m_Distrib::~Erlang_m_Distrib()
{
  debug(4,"Erlang_m_Distrib::~Erlang_m_Distrib(): destroying object");
}

/*
Next_sample(): generates a new sample of a Erlang-m distributed random variable
*/

TGFLOAT Erlang_m_Distrib::next_sample(Object_State *obj_st) 
{
   TGFLOAT rate_num;
   TGFLOAT uniform;
   TGFLOAT erlang;
   int    i, stages_num;  

   rate_num   =  rate->evaluate_numeric(obj_st);
   stages_num =  (int) stages->evaluate_numeric(obj_st);

   erlang = 0;
   for(i=1; i <= stages_num; i++)
     {
       uniform  = randval();
       erlang  += (-1.0/(rate_num * stages_num)) * log (uniform);
     }

   return(erlang);

}


/*
Show_type(): Returns the type of distribution
*/

int Erlang_m_Distrib::show_type()
{
  return (ERLANG_M_DIST);
}


/*
Acessor method for the attribute rate
*/

Expression *Erlang_m_Distrib::show_rate()
{
  return (rate);
}


/*
Acessor method for the attribute stages
*/

Expression *Erlang_m_Distrib::show_stages()
{
  return (stages);
}


/*
Acessor method for the attribute rate
*/

int  Erlang_m_Distrib::ch_rate(Expression *new_rate)
{
  if (new_rate != NULL)
    rate = new_rate;
  else
   {
    debug(3, "Erlang_m_Distrib::ch_rate: Can't change rate to NULL pointer");
    return(-1);
   }
  return(0);
}


/*
Acessor method for the attribute stages
*/

int  Erlang_m_Distrib::ch_stages(Expression *new_stages)
{
  if (new_stages != NULL)
    stages = new_stages;
  else
   {
    debug(3, "Erlang_m_Distrib::ch_rate: Can't change rate to NULL pointer");
    return(-1);
   }
  return(0);
}


void Erlang_m_Distrib::print_dist()
{
   printf ("ERLANG_M_DIST ");
   printf ("Rate: ");
   rate->print_expression();
   printf ("Stages: ");
   stages->print_expression();
}


void Erlang_m_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Erlang_m_Distrib::reset(): do nothing");
}


/*******************************************************************/

/*
Constructor: Initializes the object
*/

Deterministic_Distrib::Deterministic_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
  rate = NULL;
  debug(4,"Deterministic_Distrib::Deterministic_Distrib(long): creating object");
}


/*
Destructor for Deterministic_Distrib  
*/

Deterministic_Distrib::~Deterministic_Distrib()
{
  debug(4,"Deterministic_Distrib::~Deterministic_Distrib(): destroying object");
}


/*
Next_sample(): returns the (deterministic) value in 'rate_value' 
*/

TGFLOAT Deterministic_Distrib::next_sample(Object_State *obj_st)
{
  return ( 1.0/rate->evaluate_numeric(obj_st) );
}


/*
Show_type(): Returns the type of distribution
*/

int Deterministic_Distrib::show_type()
{
  return (DETERMINISTIC_DIST);
}


/*
Acessor method for the attribute
*/

Expression *Deterministic_Distrib::show_rate()
{
  return (rate);
}


/*
Acessor method for the attribute rate 
*/

int  Deterministic_Distrib::ch_rate(Expression *new_rate)
{
  if (new_rate != NULL)
    rate = new_rate;
  else
   {
    debug(3, "Deterministic_Distrib::ch_rate: Can't change rate to NULL pointer");
    return(-1);
   }

  return(0);
}


void Deterministic_Distrib::print_dist()
{
   printf ("DETERMINISTIC_DIST - Rate: ");
   rate->print_expression();
}


void Deterministic_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Deterministic_Distrib::reset(): do nothing");
}


/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Uniform_Distrib::Uniform_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
  lower = NULL;
  upper = NULL;
  debug(4,"Uniform_Distrib::Uniform_Distrib(long): creating object"); 
}


/*
Destructor for Uniform_Distrib  
*/

Uniform_Distrib::~Uniform_Distrib()
{
  debug(4,"Uniform_Distrib::~Uniform_Distrib(): destroying object");  
}


/*
Next_sample(): generates a new sample of an Uniform distributed random variable
*/

TGFLOAT Uniform_Distrib::next_sample(Object_State *obj_st)
{
   TGFLOAT upp_num, low_num;
   TGFLOAT uniform;

   uniform = randval();
   upp_num = upper->evaluate_numeric(obj_st);
   low_num = lower->evaluate_numeric(obj_st);

   return ( uniform * (upp_num - low_num) + low_num);
}


/*
Show_type(): Returns the type of distribution
*/

int Uniform_Distrib::show_type()
{
  return (UNIFORM_DIST);
}


/*
Acessor method for the attribute lower
*/

Expression *Uniform_Distrib::show_lower()
{
  return (lower);
}


/*
Acessor method for the attribute lower
*/

int  Uniform_Distrib::ch_lower(Expression *new_lower)
{
  if (new_lower != NULL)
    lower = new_lower;
  else
   {
    debug(3, "Uniform_Distrib::ch_lower: Can't change to NULL pointer");
    return(-1);
   }

  return(0);
}


/*
Acessor method for the attribute upper
*/

Expression *Uniform_Distrib::show_upper()
{
  return (upper);
}


/*
Acessor method for the attribute upper
*/

int  Uniform_Distrib::ch_upper(Expression *new_upper)
{
  if (new_upper != NULL)
    upper = new_upper;
  else
   {
    debug(3, "Uniform_Distrib::ch_upper: Can't change to NULL pointer");
    return(-1);
   }

  return(0);
}


void Uniform_Distrib::print_dist()
{
   fprintf( stdout , "UNIFORM_DIST ");
   fprintf ( stdout , "Lower: ");
   lower->print_expression();
   fprintf ( stdout , "Upper: ");
   upper->print_expression();
}


void Uniform_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Uniform_Distrib::reset(): do nothing");
}


/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Gaussian_Distrib::Gaussian_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{ 
  mean = NULL;
  variance = NULL;
  debug(4,"Guassian_Distrib::Gaussian_Distrib(Random_obj random_obj): creating object");
}



/*
Destructor for Gaussian_Distrib
*/

Gaussian_Distrib::~Gaussian_Distrib()
{
   debug(4,"Guassian_Distrib::~Gaussian_Distrib(): destroying object"); 
}



/*
Next_sample(): generates a new sample of a Gaussian distributed random variable
*/

TGFLOAT Gaussian_Distrib::next_sample(Object_State *obj_st) 
{
  TGFLOAT v1,v2,w, Gvar, actual_mean, actual_variance;
/*BRUNO BEGIN*/
  do
  {  
/*BRUNO END*/
      do
      {
          v1 = 2*randval() - 1;
          v2 = 2*randval() - 1;
          w  = v1*v1 + v2*v2;
      }
      while(w>1);    

      Gvar = v1*( sqrt( -2 * ( log( w ) / w ) ) );  /*  Gaussian random variable (0,1) */

      actual_mean = mean->evaluate_numeric(obj_st);
      actual_variance = variance->evaluate_numeric(obj_st);

      /* Gaussian random variable (mean, variance) */
      Gvar = actual_mean + sqrt( actual_variance ) * Gvar;    
/*BRUNO BEGIN*/
  }
  while ( Gvar < 0.0 );
/*BRUNO END*/
  return( Gvar );

}


/*
Show_type(): Returns the type of distribution
*/

int Gaussian_Distrib::show_type()
{
   return (GAUSSIAN_DIST);
}


/*
Acessor method for the attribute mean
*/ 

Expression * Gaussian_Distrib::show_mean()
{
  return (mean);
}


/*
Acessor method for the attribute mean
*/

int  Gaussian_Distrib::ch_mean(Expression * new_mean)
{
if (new_mean != NULL)
    mean = new_mean;
  else
   {
    debug(3, "Gaussian_Distrib::ch_mean: Can't change to NULL pointer");
    return(-1);
   }

return(0);
}



/*
Acessor method for the attribute variance
*/

Expression * Gaussian_Distrib::show_variance()
{
 return (variance);
}


/*
Acessor method for the attribute variance
*/

int  Gaussian_Distrib::ch_variance(Expression * new_variance)
{
  if (new_variance != NULL)
    variance = new_variance;
  else
    {
      debug(3, "Gaussian_Distrib::ch_variance: Can't change to NULL pointer");
      return(-1);
    }
  
  return(0);
}

void Gaussian_Distrib::print_dist()
{
   fprintf( stdout , "GAUSSIAN_DIST\n");
   fprintf( stdout , "Mean: ");
   mean->print_expression();
   fprintf( stdout , "Variance: ");
   variance->print_expression();
}


void Gaussian_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Gaussian_Distrib::reset(): do nothing");
}


/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Lognormal_Distrib::Lognormal_Distrib(Random_obj *random_obj) : Distribution(random_obj), Gaussvar(random_obj)
{
  mean = NULL;
  variance = NULL;
  debug(4,"Lognormal_Distrib::Lognormal_Distrib(long): creating object"); 
}


/*
Destructor for Lognormal_Distrib  
*/

Lognormal_Distrib::~Lognormal_Distrib()
{
  debug(4,"Lognormal_Distrib::~Lognormal_Distrib(): destroying object");  
}


/*
Next_sample(): generates a new sample of a Lognormal distributed random variable
*/
  
TGFLOAT Lognormal_Distrib::next_sample(Object_State *obj_st) 
{
 TGFLOAT a_mean, a_variance;
 TGFLOAT g_mean, g_variance;
 TGFLOAT Gvar;
 TGFLOAT constant;

 a_mean     =     mean->evaluate_numeric(obj_st);
 a_variance = variance->evaluate_numeric(obj_st);

 if (a_mean > 0)
  {
    constant   = 1 + (a_variance/(a_mean*a_mean));
    g_mean     = log( a_mean/sqrt(constant) );
    g_variance = log( constant );
  }
 else
  {
    debug(3, "Lognormal_Distrib::next_sample: Mean parameter isn't positive");
    return(0);
  }
 Gaussvar.ch_mean(g_mean);
 Gaussvar.ch_variance(g_variance);

 Gvar = Gaussvar.next_sample();

 return (  exp( Gvar )  );

}


/*
Show_type(): Returns the type of distribution
*/

int Lognormal_Distrib::show_type()
{
  return (LOGNORMAL_DIST);
}


/*
Acessor method for the attribute mean
*/

Expression * Lognormal_Distrib::show_mean()
{
  return (mean);
}


/*
Acessor method for the attribute mean
*/

int  Lognormal_Distrib::ch_mean(Expression * new_mean)
{
  if (new_mean != NULL)
    mean = new_mean;
  else
    {
      debug(3, "Lognormal_Distrib::ch_mean: Can't change to NULL pointer");
      return(-1);
    }
  
  return(0);
}


/*
Acessor method for the attribute variance
*/

Expression * Lognormal_Distrib::show_variance()
{
  return (variance);
}


/*
Acessor method for the attribute variance
*/

int  Lognormal_Distrib::ch_variance(Expression * new_variance)
{
  if (new_variance != NULL)
    variance = new_variance;
  else
    {
      debug(3, "Lognormal_Distrib::ch_variance: Can't change to NULL pointer");
      return(-1);
    }

  return(0);
}


void Lognormal_Distrib::print_dist()
{
  fprintf( stdout , "LOGNORMAL_DIST\n");
  fprintf( stdout , "Mean: ");
  mean->print_expression();
  fprintf( stdout , "Variance: ");
  variance->print_expression();
}


void Lognormal_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Lognormal_Distrib::reset(): do nothing");
}

/*******************************************************************/
/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

TruncLognormal_Distrib::TruncLognormal_Distrib( Random_obj *random_obj ) : Distribution( random_obj ), Gaussvar( random_obj )
{
  mean     = NULL;
  variance = NULL;
  minimum  = NULL;
  maximum  = NULL;
  debug( 4, "TruncLognormal_Distrib::TruncLognormal_Distrib(long): creating object" ); 
}
/*
Destructor for TruncLognormal_Distrib  
*/

TruncLognormal_Distrib::~TruncLognormal_Distrib()
{
  debug(4,"TruncLognormal_Distrib::~TruncLognormal_Distrib(): destroying object");  
}


/*
Next_sample(): generates a new sample of a Lognormal distributed random variable
*/
  
TGFLOAT TruncLognormal_Distrib::next_sample( Object_State *obj_st ) 
{
 TGFLOAT a_mean, a_variance;
 TGFLOAT a_minimum, a_maximum;
 TGFLOAT g_mean, g_variance;
 TGFLOAT Gvar;
 TGFLOAT constant;
 TGFLOAT sample;

 a_mean     =  mean->evaluate_numeric(obj_st);
 a_variance =  variance->evaluate_numeric(obj_st);
 a_minimum  =  minimum->evaluate_numeric(obj_st);
 a_maximum  =  maximum->evaluate_numeric(obj_st);

 if (a_mean > 0)
  {
    constant   = 1 + (a_variance/(a_mean*a_mean));
    g_mean     = log( a_mean/sqrt(constant) );
    g_variance = log( constant );
  }
 else
  {
    debug(3, "TruncLognormal_Distrib::next_sample: Mean parameter isn't positive");
    return(0);
  }
 Gaussvar.ch_mean(g_mean);
 Gaussvar.ch_variance(g_variance);


 Gvar = Gaussvar.next_sample();

 sample = exp( Gvar );
 if( sample < a_minimum )
     sample = a_minimum;
     
 if( sample > a_maximum )
     sample = a_maximum;




 return (sample);

}


/*
Show_type(): Returns the type of distribution
*/

int TruncLognormal_Distrib::show_type()
{
  return (TRUNCLOGNORMAL_DIST);
}


/*
Acessor method for the attribute mean
*/

Expression * TruncLognormal_Distrib::show_mean()
{
  return (mean);
}


/*
Acessor method for the attribute mean
*/

int  TruncLognormal_Distrib::ch_mean(Expression * new_mean)
{
  if (new_mean != NULL)
    mean = new_mean;
  else
    {
      debug(3, "TruncLognormal_Distrib::ch_mean: Can't change to NULL pointer");
      return(-1);
    }
  
  return(0);
}


/*
Acessor method for the attribute variance
*/

Expression * TruncLognormal_Distrib::show_variance()
{
  return (variance);
}


/*
Acessor method for the attribute variance
*/

int  TruncLognormal_Distrib::ch_variance(Expression * new_variance)
{
  if (new_variance != NULL)
    variance = new_variance;
  else
    {
      debug(3, "TruncLognormal_Distrib::ch_variance: Can't change to NULL pointer");
      return(-1);
    }

  return(0);
}

/*
Acessor method for the attribute minimum
*/
Expression * TruncLognormal_Distrib::show_minimum()
{
  return (minimum);
}



/*
Acessor method for the attribute minimum
*/
int  TruncLognormal_Distrib::ch_minimum(Expression * new_minimum)
{
    if( new_minimum != NULL )
        minimum = new_minimum;
    else
    {
        debug( 3, "TruncLognormal_Distrib::ch_minimum: Can't change to NULL pointer" );
        return( -1 );
    }

    return( 0 );
}

/*
Acessor method for the attribute maximum
*/
Expression * TruncLognormal_Distrib::show_maximum()
{
  return (maximum);
}



/*
Acessor method for the attribute maximum
*/
int  TruncLognormal_Distrib::ch_maximum(Expression * new_maximum)
{
    if( new_maximum != NULL )
        maximum = new_maximum;
    else
    {
        debug( 3, "TruncLognormal_Distrib::ch_maximum: Can't change to NULL pointer" );
        return( -1 );
    }
    return( 0 );
}


void TruncLognormal_Distrib::print_dist()
{
  fprintf( stdout , "LOGNORMAL_DIST\n");
  fprintf( stdout , "Mean: ");
  mean->print_expression();
  fprintf( stdout , "Variance: ");
  variance->print_expression();
  fprintf( stdout , "Minimum: ");
  minimum->print_expression();
  fprintf( stdout , "Maximum: ");
  maximum->print_expression();
}


void TruncLognormal_Distrib::reset(int run)
{
    /* do nothing*/
    debug( 4, "TruncLognormal_Distrib::reset(): do nothing" );
}

/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Weibull_Distrib::Weibull_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
    scale = NULL;
    shape = NULL;
    debug(4,"Weibull_Distrib::Weibull_Distrib(long): creating object");
}


/*
Destructor for Weibull_Distrib
*/

Weibull_Distrib::~Weibull_Distrib()
{
    debug(4,"Weibull_Distrib::~Weibull_Distrib(): destroying object");
}


/*
Next_sample(): generates a new sample of a Weibull distributed random variable
*/

TGFLOAT Weibull_Distrib::next_sample(Object_State *obj_st)
{
    TGFLOAT a_scale, a_shape;
    TGFLOAT uniform, sample;

    a_scale = scale->evaluate_numeric(obj_st);
    a_shape = shape->evaluate_numeric(obj_st);
    if( (a_scale > 0) && (a_shape > 0) )
    {
        uniform = randval();
        sample  = ( (a_scale)*( pow(-log(uniform),(1/a_shape)) )  );
    }
    else
    {
        debug(3, "Weibull_Distrib::next_sample: Scale and Shape parameters must be > 0");
        return( 0 );
    }

    return( sample );
}


/*
Show_type(): Returns the type of distribution
*/

int Weibull_Distrib::show_type()
{
    return (WEIBULL_DIST);
}


/*
Acessor method for the attribute scale
*/

Expression * Weibull_Distrib::show_scale()
{
    return (scale);
}


/*
Acessor method for the attribute scale
*/

int  Weibull_Distrib::ch_scale(Expression * new_scale)
{
    if( new_scale != NULL )
        scale = new_scale;
    else
    {
        debug(3, "Weibull_Distrib::ch_scale: Can't change to NULL pointer");
        return( -1 );
    }

    return( 0 );
}


/*
Acessor method for the attribute shape
*/

Expression * Weibull_Distrib::show_shape()
{
    return( shape );
}


/*
Acessor method for the attribute shape
*/

int  Weibull_Distrib::ch_shape(Expression * new_shape)
{
    if( new_shape != NULL )
        shape = new_shape;
    else
    {
        debug(3, "Weibull_Distrib::ch_shape: Can't change to NULL pointer");
        return( -1 );
    }

    return( 0 );
}


void Weibull_Distrib::print_dist()
{
    fprintf( stdout , "Weibull\n");
    fprintf( stdout , "Scale: ");
    scale->print_expression();
    fprintf( stdout , "Shape: ");
    shape->print_expression();
}


void Weibull_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Weibull_Distrib::reset(): do nothing");
}

/*******************************************************************/
/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Pareto_Distrib::Pareto_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
    shape = NULL;
    debug( 4,"Pareto_Distrib::Pareto_Distrib(long): creating object" );
}


/*
Destructor for Pareto_Distrib
*/

Pareto_Distrib::~Pareto_Distrib()
{
    debug( 4,"Pareto_Distrib::~Pareto_Distrib(): destroying object" );
}


/*
Next_sample(): generates a new sample of a Pareto distributed random variable
*/

TGFLOAT Pareto_Distrib::next_sample(Object_State *obj_st)
{
    TGFLOAT a_shape, a_scale;
    TGFLOAT uniform,sample;

    a_scale     = scale->evaluate_numeric(obj_st);
    a_shape     = shape->evaluate_numeric(obj_st);

    if ( (a_shape > 0) && (a_scale > 0) )
    {
        uniform  = randval();
        sample   = ( a_scale/( pow( uniform,(1/a_shape)) ) );
    }
    else
    {
        debug( 3, "Pareto_Distrib::next_sample:  Shape parameter must be > 0" );
        return( 0 );
    }


    return( sample );
}


/*
Show_type(): Returns the type of distribution
*/

int Pareto_Distrib::show_type()
{
    return (PARETO_DIST);
}


/*
Acessor method for the attribute scale
*/

Expression * Pareto_Distrib::show_scale()
{
    return (scale);
}


/*
Acessor method for the attribute scale
*/

int  Pareto_Distrib::ch_scale(Expression * new_scale)
{
    if( new_scale != NULL )
        scale = new_scale;
    else
    {
        debug(3, "Pareto_Distrib::ch_scale: Can't change to NULL pointer");
        return( -1 );
    }

    return( 0 );
}


/*
Acessor method for the attribute shape
*/

Expression * Pareto_Distrib::show_shape()
{
  return (shape);
}


/*
Acessor method for the attribute shape
*/

int  Pareto_Distrib::ch_shape(Expression * new_shape)
{
    if( new_shape != NULL )
        shape = new_shape;
    else
    {
        debug( 3, "Pareto_Distrib::ch_shape: Can't change to NULL pointer" );
        return( -1 );
    }

    return( 0 );
}


void Pareto_Distrib::print_dist()
{
  fprintf( stdout , "Pareto\n");
  fprintf( stdout , "Scale: ");
  scale->print_expression();
  fprintf( stdout , "Shape: ");
  shape->print_expression();
}


void Pareto_Distrib::reset(int run)
{
    /* do nothing*/
    debug(4,"Pareto_Distrib::reset(): do nothing");
}

/*******************************************************************/
/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

Trunc_Pareto_Distrib::Trunc_Pareto_Distrib (Random_obj *random_obj):Pareto_Distrib(random_obj)
{
    scale   = NULL;
    shape   = NULL;
	maximum = NULL;
    debug( 4,"Trunc_Pareto_Distrib::Trunc_Pareto_Distrib(long): creating object" );
}


/*
Destructor for Pareto_Distrib
*/

Trunc_Pareto_Distrib::~Trunc_Pareto_Distrib()
{
    debug( 4,"Trunc_Pareto_Distrib::~Trunc_Pareto_Distrib(): destroying object" );
}


/*
Next_sample(): generates a new sample of a truncated Pareto distributed
               random variable
*/

TGFLOAT Trunc_Pareto_Distrib::next_sample(Object_State *obj_st)
{
    TGFLOAT a_shape, a_scale, a_maximum;
    TGFLOAT uniform, sample;
    a_scale     = scale->evaluate_numeric(obj_st);
    a_shape     = shape->evaluate_numeric(obj_st);
    a_maximum   = maximum->evaluate_numeric(obj_st);
    if ( (a_shape > 0)&&(a_scale > 0) )
    {
        uniform  = randval();
        sample   = ( a_scale/( pow( uniform,(1/a_shape)) ) );
    }
    else
    {
        debug( 3, "Trunc_Pareto_Distrib::next_sample:  Shape, Scale and Maximum parameters must be > 0");
        return( 0 );
    }
    if (sample>a_maximum)
	    sample = a_maximum;
    return( sample );
}


/*
Show_type(): Returns the type of distribution
*/

int Trunc_Pareto_Distrib::show_type()
{
    return (TRUNC_PARETO_DIST);
}


/*
Acessor method for the attribute maximum
*/
Expression * Trunc_Pareto_Distrib::show_maximum()
{
  return (maximum);
}



/*
Acessor method for the attribute maximum
*/
int  Trunc_Pareto_Distrib::ch_maximum(Expression * new_maximum)
{
    if( new_maximum != NULL )
        maximum = new_maximum;
    else
    {
        debug( 3, "Trunc_Pareto_Distrib::ch_maximum: Can't change to NULL pointer" );
        return( -1 );
    }

    return( 0 );
}


void Trunc_Pareto_Distrib::print_dist()
{
  fprintf( stdout , "Truncated Pareto\n");
  fprintf( stdout , "Scale: ");
  scale->print_expression();
  fprintf( stdout , "Shape: ");
  shape->print_expression();
  fprintf( stdout , "Maximum value: ");
  maximum->print_expression();
}

/*************************************************************/
/*
Constructor: Initializes the object
*/

File_Distrib::File_Distrib (Random_obj *random_obj) : Distribution (random_obj)
{
  sprintf(f_name,"%s","");
  debug(4,"File_Distrib::File_Distrib(long): creating object");
}


/*
Destructor for File_Distrib
*/

File_Distrib::~File_Distrib()
{
  f_samples.close();
  debug(4,"File_Distrib::~File_Distrib(): destroying object");
}


/*
Next_sample(): 
 returns the next entry in the associated sample file. If the end of file is 
 reached, this method returns the file pointer to the beginning and starts again 
 to read the file.
*/


TGFLOAT File_Distrib::next_sample(Object_State *obj_st) 
{
    TGFLOAT f_value;          /* events between previous timestamp and actual timestamp */
    TGFLOAT f_time_interval;  /* timestamp */

    if (ev_tsc == total_ev_tsc)  /* Condition to read the file (again) */
    {
        do
        {
            if (f_samples.eof())
            {
                fprintf(stderr, "Simulation of FILE distribution reached the end of the given trace (%s).\n", f_name);
                fprintf(stderr, "SIMULATION ABORTED!\n");

                fprintf(stdout, "Simulation of FILE distribution reached the end of the given trace (%s).\n", f_name);
                fprintf(stdout, "SIMULATION ABORTED!\n");

                f_samples.close();
     	        exit(35);
            }

            f_time_interval = f_samples.read();
            f_value = f_samples.read();
        }
        while (rint(f_value) < 0.5);  /* To avoid zero number of events */

        ev_tsc = 0;
        total_ev_tsc = (int) rint(f_value);
        inter_time_tsc = f_time_interval/total_ev_tsc;
    }

    ev_tsc++;

    return(inter_time_tsc);
}


/*
Show_type(): Returns the type of distribution
*/

int File_Distrib::show_type()
{
  return (FILE_DIST);
}


/*
Acessor method for the attribute f_name
*/

void  File_Distrib::show_file(char* dest_f_name)
{
  if (dest_f_name != NULL)
    sprintf(dest_f_name,"%s",f_name);

}


/* 
Acessor method for the attribute f_name
*/

int  File_Distrib::ch_file(char* new_name, int run)
{
  char filename[500];


  if (new_name == NULL)
  {
     debug(0, "File_Distrib::ch_file: Can't change file to NULL pointer");
     fprintf(stdout, "SIMULATION ABORTED!\n");
     fprintf(stderr, "SIMULATION ABORTED!\n");
     exit(34);
  }

  f_samples.close();

  sprintf(f_name,"%s", new_name);
  sprintf(filename,"%s.r%d",f_name,run);

  if (f_samples.open_r(filename, CACHE_VALUE) == -1)
  {
     if( run > 0 )
     {
         fprintf( stdout, "WARNING:\n\tCould not open %s. Using the same trace file that was used for run 0\n", filename );
         fprintf( stderr, "WARNING:\n\tCould not open %s. Using the same trace file that was used for run 0\n", filename );
     }

     if (f_samples.open_r(f_name, CACHE_VALUE) == -1)
     {
        debug(0, "File_Distrib::ch_file: Can't open file %s or file %s for reading.", f_name, filename);

        fprintf(stdout, "Can't open file %s or file %s for reading.\n", f_name, filename);
        fprintf(stdout, "SIMULATION ABORTED!\n");

        fprintf(stderr, "Can't open file %s or file %s for reading.\n", f_name, filename);
        fprintf(stderr, "SIMULATION ABORTED!\n");
        exit( 30 );
     }
  }

  fprintf(stdout,"Opening %s for run %d\n", f_name, run);


  return(0);
}

void File_Distrib::print_dist()
{
  fprintf ( stdout , "FILE_DIST ");
  fprintf ( stdout , "File: %s", f_name);

}


void File_Distrib::reset(int run)
{

    debug(4,"File_Distrib::reset(): initializing new file: %s",f_name);

    fprintf(stderr,"Reseting for run %d\n", run);

    ch_file(f_name, run + 1);
}


/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

FBM_Distrib::FBM_Distrib (Random_obj *random_obj) : Distribution (random_obj), Gaussvar(random_obj)
{
  maxlevel = 10;
  mean_value = 1;
  stddev = 1;
  hurst = 0.5;
  tsc = 1;
  
  sprintf(filename,"%s","noname.fbm");
  debug(4,"FBM_Distrib::FBM_Distrib(long): creating object");

}


/*
Destructor for FBM_Distrib  
*/

FBM_Distrib::~FBM_Distrib()
{
  fbm_file.delete_file();
  debug(4,"FBM_Distrib::~FBM_Distrib(): destroying object");
}


/*
Next_sample():
 Generates a new sample related to the FBM sample path file 
 created in the 'generate()' method. 
 Each sample in the FBM file corresponds to a number of events during a 
 time scale period. These events are equally spaced in the time scale. 
 Once all events have occurred in the actual time scale, a new sample of 
 FBM file is read for the next time scale and so on. If the end of file 
 is reached, this method returns the file pointer to the beginning and 
 starts again to read the file.
*/

TGFLOAT FBM_Distrib::next_sample(Object_State *)
{
  TGFLOAT newvalue;
  int i;
  
  if (init_OK == FALSE)
    {
      debug(3, "FBM_Distrib::next_sample: FBM object was not (correctly) initialized");
      return(-1);
    }

  i = 0;
  if(ev_tsc==total_ev_tsc)  // Condition to read the file (again)
    {
      do
	{
	  if(fbm_file.eof())  
	    { 
          fprintf(stderr, "Simulation of FBM process reached the end of the generated trace (%s).\n", filename);
          fprintf(stderr, "SIMULATION ABORTED!\n");

          fprintf(stdout, "Simulation of FBM process reached the end of the generated trace (%s).\n", filename);
          fprintf(stdout, "SIMULATION ABORTED!\n");

          fbm_file.close();
	      exit(35);
	    }
	  newvalue=fbm_file.read();

	  i++;     
	} 
      while( rint(newvalue)< 0.5 );
      
      ev_tsc=0;
      total_ev_tsc = (int) rint(newvalue);
      inter_time_tsc = tsc/total_ev_tsc;
    }
  ev_tsc++;
  
  if(i<=1)
    return(inter_time_tsc);
  else
    return(inter_time_tsc + (i-1)*tsc);
}



/*
Show_type(): Returns the type of distribution
*/

int FBM_Distrib::show_type()
{
  return (FBM_DIST);
}


/*
Acessor method for the class attributes 
*/

int  FBM_Distrib::ch_param(char* new_name, int new_maxlevel, TGFLOAT new_mean, TGFLOAT new_variance, TGFLOAT new_hurst, TGFLOAT new_tsc)
{
  init_OK  = FALSE;

  if (new_name != NULL)
    sprintf(filename,"%s.FBM",new_name);   
  else
    {
      debug(3, "FBM_Distrib::ch_param: Can't change file to NULL pointer");
      return(-1);
    }
    
  if ((new_maxlevel > 0) && (new_maxlevel <=20))
    maxlevel = new_maxlevel;
  else
    {
      debug(3, "FBM_Distrib::ch_param: Parameter Maxlevel out of range [0,20]");
      return(-1);
    }
  mean_value = new_mean;

  if (new_variance >= 0)
    stddev = sqrt(new_variance);
  else
    {
      debug(3, "FBM_Distrib::ch_param: Parameter Variance is negative");
      return(-1);
    }     
  
  if ((new_hurst>=0) && (new_hurst<=1))
    hurst = new_hurst;
  else
    {
      debug(3, "FBM_Distrib::ch_param: Parameter Hurst out of range [0,1]");
      return(-1);
    }     

  if (new_tsc>=0)
    tsc = new_tsc;   
  else
    {
      debug(3, "FBM_Distrib::ch_param: Parameter Tsc is negative");
      return(-1);
    }     

  total_ev_tsc = 0;
  ev_tsc       = 0;
  
  if (generate() < 0)       /* Generation of fbm file */         
    {
      debug(3, "FBM_Distrib::ch_param: fBM generation failed");
      return(-1);
    }
  
  fbm_file.open_r(filename,CACHE_VALUE);
  init_OK  = TRUE;

  debug(4, "FBM_Distrib::ch_param");
  return(0);
}


/*
Generate(): 
  This method is responsible for the generation and storage 
  of an FBM sample path. It will be generated '2^maxlevel + 1' 
  samples. The FBM file is generated according to the successive
  random additions algorithm (see "The Science of Fractal Images",
  B.B. Mandelbrot, pp. 86). Then, the incremental process FGN is 
  computed, according to the approach of Norros (see I. Norros, 
  "On the Use of Fractional Brownian Motion in the Theory of 
  Connectionless Networks", IEEE J.S.A.C., vol. 13, No.6, 
  pp. 953-962, Aug. 1995). This process matches the  mean,
  variance and Hurst values previously specified.
*/

int FBM_Distrib::generate()
{
  TGFLOAT *delta = new TGFLOAT[maxlevel+1];  int     i,d,D,level;
  TGFLOAT  pk;   // Peakedness parameter ("a" parameter in Norros paper)
  TGFLOAT  Att;  // Incremental value of the FGN sample path 
  TGFLOAT  sigma_t; // End standard deviation value for standard fBm generation
  int     SIZE;
  TGFLOAT  *fbmvec;
// The following declarations lead to a execution error. It seems to crash the
// stack.
//  int    SIZE = (int) pow(2, maxlevel);
//  TGFLOAT fbmvec[SIZE + 1];

  SIZE   = (int) pow(2.0, maxlevel);
  fbmvec = (TGFLOAT *)malloc( sizeof( TGFLOAT ) * (SIZE + 1) );
  // Computation of std. dev. value at end time t of the fBM trace

  sigma_t = pow(((TGFLOAT)SIZE*tsc), hurst);

  // Generation of standard fBM in memory

  for(i=1; i < maxlevel+1; i++)
    delta[i] = sigma_t*sqrt(0.5)*pow(0.5, i*hurst)*sqrt(1 - pow(2,2*hurst-2));
  fbmvec[0]=0;
  
  fbmvec[SIZE] = sigma_t*Gaussvar.next_sample(); 
  D=SIZE;
  d=D/2;
  level=1;

  while(level<= maxlevel)
    {
      for(i=d; i<=SIZE-d; i+=D)
	fbmvec[i]= 0.5*(fbmvec[i-d]+fbmvec[i+d]);
      
      for(i=0; i<=SIZE ; i+=d)
	fbmvec[i]= fbmvec[i] + delta[level]*Gaussvar.next_sample();
      
      D=D/2;
      d=d/2;
      level++; 
    }
  
  // Calculation of the peakedness parameter

  pk = (stddev*stddev)/(mean_value*pow(tsc,2*hurst));

  // Generation of the incremental process (FGN) scaled by the pk parameter
  // note: one sample is lost

  for(i=0; i< SIZE; i++)
    {
      Att = mean_value*tsc + sqrt(pk*mean_value)*(fbmvec[i+1]-fbmvec[i]);
      fbmvec[i] = Att;
    }

  // Storage of FGN in disk

  fbm_file.open_w(filename);
  
  for(i=0; i< SIZE; i++)
    fbm_file.write(fbmvec[i]);

  fbm_file.close();

  free( fbmvec );
  return(1);
}

void FBM_Distrib::print_dist()
{
  fprintf( stdout , "FBM_DIST\n");
  fprintf( stdout , "mean: %.8e\n", mean_value);
  fprintf( stdout , "std dev: %.8e\n", stddev);
  fprintf( stdout , "hurst: %.8e\n", hurst);
  fprintf( stdout , "maxlevel: %d\n", maxlevel);
  fprintf( stdout , "timescale: %.8e\n", tsc);
}

void FBM_Distrib::reset(int run)
{
  debug(4,"FBM_Distrib::reset(): reseting FBM_Distribution");

  fbm_file.close();

  total_ev_tsc = 0;
  ev_tsc       = 0;
  
  if (generate() < 0)       /* Generation of fbm file */         
    {
      debug(3, "FBM_Distrib::reset: fBM generation failed");
      exit(-1);
    }
  
  fbm_file.open_r(filename,CACHE_VALUE);

  debug(4, "FBM_Distrib::reset: fBM reset and generation done");
}




/*******************************************************************/

/*
Constructor: Initializes the object an receives the seed for internal
random number generation
*/

FARIMA_Distrib::FARIMA_Distrib(Random_obj *random_obj) : Distribution (random_obj), Gaussvar(random_obj)
{
  maxnumber  = 1000;
  mean_value = 1;
  variance   = 1;
  param_d    = 0;    /* Parameter "d" in fractional ARIMA (0,d,0) process */
  tsc        = 1;

  sprintf(filename,"%s","noname.farima"); 
  debug(4,"FARIMA_Distrib::FARIMA_Distrib(long): creating object");
}


/*
Destructor for FARIMA_Distrib  
*/

FARIMA_Distrib::~FARIMA_Distrib()
{
   fARIMA_file.delete_file();
   debug(4,"FARIMA_Distrib::~FARIMA_Distrib(): destroying object");
}


/*
Next_sample():
 Generates a new sample related to the f-ARIMA sample path file 
 created in the 'generate()' method. 
 Each sample in the f-ARIMA file corresponds to a number of events during a 
 time scale period. These events are equally spaced in the time scale. 
 Once all events have occurred in the actual time scale, a new sample of 
 f-ARIMA file is read for the next time scale and so on. If the end of file
 is reached, this method returns the file pointer to the beginning and 
 starts again to read the file.
*/

TGFLOAT FARIMA_Distrib::next_sample(Object_State *)
{
  TGFLOAT newvalue;
  int i;
  double sample;
  
  if (init_OK == FALSE)
    {
      debug(3, "FARIMA_Distrib::next_sample: FARIMA object was not (correctly) initialized");
      return(-1);
    }
  do 
  {
      i = 0;
      if(ev_tsc==total_ev_tsc)  // Condition to read the file (again)
        {
          do
	    {
	      if(fARIMA_file.eof())  
	        { 
              fprintf(stderr, "Simulation of FARIMA process reached the end of the generated trace (%s).\n", filename);
              fprintf(stderr, "SIMULATION ABORTED!\n");

              fprintf(stdout, "Simulation of FARIMA process reached the end of the generated trace (%s).\n", filename);
              fprintf(stdout, "SIMULATION ABORTED!\n");

              fARIMA_file.close();      
	          exit(35);	       
	        }
	      newvalue=fARIMA_file.read();

	      i++;     
	    }
          while( rint(newvalue)< 0.5 );

          ev_tsc=0;
          total_ev_tsc = (int) rint(newvalue);
          inter_time_tsc = tsc/total_ev_tsc;
        }
      ev_tsc++;
      if( i < 1 )
          sample = inter_time_tsc;
      else
          sample = ( inter_time_tsc + (i-1)*tsc );
  } while( sample < 0  );
    
  if(i<=1)
    return(inter_time_tsc);
  else
    return(inter_time_tsc + (i-1)*tsc);
  
  
}


/*
Show_type(): Returns the type of distribution
*/

int FARIMA_Distrib::show_type()
{
    return (FARIMA_DIST);
}


/*
Acessor method for the class attributes
*/

int  FARIMA_Distrib::ch_param(char* new_name, int new_maxnumber, TGFLOAT new_mean, TGFLOAT new_variance, TGFLOAT new_hurst, TGFLOAT new_tsc)
{
   init_OK  = FALSE;
   if (new_name != NULL)
     sprintf(filename,"%s.FARIMA",new_name);   
   else
      {
        debug(3, "FARIMA_Distrib::ch_param: Can't change file to NULL pointer");
	return(-1);
      }

   if (new_maxnumber > 0)
      maxnumber = new_maxnumber;
    else
      {
        debug(3, "FARIMA_Distrib::ch_param: Parameter Maxnumber is negative");
	return(-1);
      }

   mean_value = new_mean;

   if (new_variance >= 0)
     variance = new_variance;
   else
      {
        debug(3, "FARIMA_Distrib::ch_param: Parameter Variance is negative");
	return(-1);
      }     

   if ((new_hurst>=0) && (new_hurst<=1))
       param_d = new_hurst - 0.5;
   else
      {
        debug(3, "FARIMA_Distrib::ch_param: Parameter Hurst out of range [0,1]");
	return(-1);
      }     

   if (new_tsc>=0)
      tsc = new_tsc;   
   else
      {
        debug(3, "FARIMA_Distrib::ch_param: Parameter Tsc is negative");
	return(-1);
      }     
      
   total_ev_tsc = 0;
   ev_tsc = 0;
   
   if (generate() < 0)       /* Generation of f-ARIMA file */         
     {
       debug(3, "FARIMA_Distrib::ch_param: f-ARIMA generation failed");
       return(-1);
     }
   
   fARIMA_file.open_r(filename,CACHE_VALUE);
   init_OK  = TRUE;
   return(0);
}


/*
Generate():
  This method is responsible for the generation and storage 
  of an f-ARIMA sample path. It will be generated 'maxnumber+1'
  samples. The f-ARIMA sample path  is generated according to 
  the Hosking's algorithm (see "Analysis, Modeling and Generation 
  of Self-Similar VBR Video Traffic", Mark Garret and Walter 
  Wilinger, ACM SigComm, 1994). 
*/

int FARIMA_Distrib::generate()
{
  TGFLOAT* Rho_vector;
  TGFLOAT  N_k;
  TGFLOAT  D_k;
  TGFLOAT* Phy_vector[2];
  TGFLOAT  mean_k;
  TGFLOAT  variance_k;
  TGFLOAT* Sample_Vector;
  long    k,j;
  int     vec_select, old_vec_select;
  TGFLOAT  aux_sum;
  
  /* Memory allocation for the vectors */
  
  Rho_vector       = new TGFLOAT[maxnumber+1];
  Phy_vector[0]    = new TGFLOAT[maxnumber+1];
  Phy_vector[1]    = new TGFLOAT[maxnumber+1];
  Sample_Vector    = new TGFLOAT[maxnumber+1];
  
  
  /* Initialization */
  
  N_k   = 0;
  D_k   = 1;
  mean_k = 0;
  variance_k = variance;  
  Rho_vector[0] = 1;
  Gaussvar.ch_mean(0);
  Gaussvar.ch_variance(variance);
  Sample_Vector[0]=Gaussvar.next_sample();
  
  /* Main loop */
  
  for( k=1 ; k <= maxnumber ; k++ )
    { 
      vec_select = k % 2;
      old_vec_select = (k+1) % 2;
      
      /* Calculation of new rho */
      Rho_vector[k] = ((k-1+param_d)/(k-param_d))*Rho_vector[k-1];    
      
      /* Calculation of new D_k */
      D_k = D_k - ((N_k*N_k)/D_k);
      
      /* Calculation of new N_k */
      aux_sum = 0;
      for(j = 1 ; j <= (k-1) ; j++)
	{
	  aux_sum += Phy_vector[old_vec_select][j]*Rho_vector[k-j] ;
	}
      N_k = Rho_vector[k] - aux_sum;
      
      /* Calculation of the new Phy_vector */
      Phy_vector[vec_select][k] = N_k/D_k;

      for(j=1 ; j<=(k-1) ; j++)
	{
	  Phy_vector[vec_select][j]=Phy_vector[old_vec_select][j] - Phy_vector[vec_select][k]*Phy_vector[old_vec_select][k-j];
	}

      /* Calculation of mean_k and variance_k */
      aux_sum = 0;
      for(j = 1 ; j <= k ; j++)
	{
	  aux_sum += Phy_vector[vec_select][j]*Sample_Vector[k-j] ;
	}
      mean_k = aux_sum;
      
      variance_k = (1 - pow(Phy_vector[vec_select][k],2))*variance_k;
      
      /* Obtaining the new sample */
      Gaussvar.ch_mean(mean_k);
      Gaussvar.ch_variance(variance_k);
      Sample_Vector[k] = Gaussvar.next_sample();
    } 
  
  
  // Storage of fARIMA in memory and/or disk
  
  fARIMA_file.open_w(filename);
  
  for(j=0; j<=maxnumber; j++)
    {
      Sample_Vector[j] += mean_value;
      fARIMA_file.write(Sample_Vector[j]);
    }

  fARIMA_file.close();      
  
  /* Releasing memory */
  
  delete [] Rho_vector;
  delete [] Phy_vector[0];      
  delete [] Phy_vector[1];
  delete [] Sample_Vector;
  
  return(1);
}

void FARIMA_Distrib::print_dist()
{
  fprintf ( stdout , "FARIMA_DIST\n");
  fprintf ( stdout , "mean: %.8e\n", mean_value);
  fprintf ( stdout , "variance: %.8e\n", variance);
  fprintf ( stdout , "hurst: %.8e\n", param_d);
  fprintf ( stdout , "maxnumber: %ld\n", maxnumber);
  fprintf ( stdout , "timescale: %.8e\n", tsc);
}

void FARIMA_Distrib::reset(int run)
{
   debug(4,"FARIMA_Distrib::reset(): reseting distribution");

   fARIMA_file.close();

   total_ev_tsc = 0;
   ev_tsc = 0;

   if (generate() < 0)       /* Generation of f-ARIMA file */
     {
       debug(3, "FARIMA_Distrib::reset: f-ARIMA generation failed");
       exit(-1);
     }

   fARIMA_file.open_r(filename,CACHE_VALUE);
   debug(4, "FARIMA_Distrib::reset: f-ARIMA reset and generation done");
}
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
//  Rew_Reach_Distrib (pseudo distribution)
//------------------------------------------------------------------------------
Rew_Reach_Distrib::Rew_Reach_Distrib(Random_obj *random_obj) : Distribution (random_obj)
{
    delta_time = INFINITE;
    cr = 0;
    ir = 0;
    level = 0;
    skip_event = FALSE;
    need_evaluation = TRUE;
    trigger_now = FALSE;
    stamp_list = new Stamp_List();
    rr_touch_list = new RR_Touch_List();
    debug(4,"Rew_Reach_Distrib::Rew_Reach_Distrib(long): creating object");
}
//------------------------------------------------------------------------------
/* Destructor for File_Distrib */
Rew_Reach_Distrib::~Rew_Reach_Distrib()
{
    delete stamp_list;
    delete rr_touch_list;
    debug(4,"Rew_Reach_Distrib::Rew_Reach_Distrib(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
/* next_sample(): Returns the amount of time necessary to some reward reach the
    level specified. This time is always the smaller one among the conditions.
    see simulator.ccp method will_reach_reward.                               */
TGFLOAT Rew_Reach_Distrib::next_sample(Object_State *obj_st)
{
    return( delta_time );
}
//------------------------------------------------------------------------------
TGFLOAT Rew_Reach_Distrib::show_delta_time()
{
    return (delta_time);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_delta_time(TGFLOAT new_delta_time)
{
    delta_time = new_delta_time;
}
//------------------------------------------------------------------------------
TGFLOAT Rew_Reach_Distrib::show_cr()
{
    return (cr);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_cr(TGFLOAT new_cr)
{
    cr = new_cr;
}
//------------------------------------------------------------------------------
TGFLOAT Rew_Reach_Distrib::show_ir()
{
    return (ir);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_ir(TGFLOAT new_ir)
{
    ir = new_ir;
}
//------------------------------------------------------------------------------
TGFLOAT Rew_Reach_Distrib::show_level()
{
    return (level);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_level(TGFLOAT new_level)
{
    level = new_level;
}
//------------------------------------------------------------------------------
int Rew_Reach_Distrib::show_skip_event()
{
    return (skip_event);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_skip_event(int new_val)
{
    skip_event = new_val;
}
//------------------------------------------------------------------------------
int Rew_Reach_Distrib::show_need_evaluation()
{
    return (need_evaluation);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_need_evaluation(int new_val)
{
    need_evaluation = new_val;
}
//------------------------------------------------------------------------------
int Rew_Reach_Distrib::show_trigger_now()
{
    return (trigger_now);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::ch_trigger_now(int new_val)
{
    trigger_now = new_val;
}
//------------------------------------------------------------------------------
Stamp_List *Rew_Reach_Distrib::show_stamp_list(int access)
{
    Stamp_List  *stamp_list_aux;
    if (access == LIST_RW)
        return (stamp_list);
    else
    {
        stamp_list_aux = new Stamp_List(stamp_list);
        return (stamp_list_aux);
    }
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::add_stamp(Stamp *new_stamp)
{
    if (new_stamp != NULL)
        stamp_list->add_tail_stamp( new_stamp );
    else
        debug(3,"Rew_Reach_Distrib::add_rew_stamp: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::clear_stamp_list()
{
    Stamp *stamp_aux;

    stamp_aux = stamp_list->get_1st_stamp();
    while ( stamp_aux != NULL )
    {
        delete stamp_aux;
        stamp_aux = stamp_list->get_1st_stamp();
    }
}
//------------------------------------------------------------------------------
RR_Touch_List *Rew_Reach_Distrib::show_rr_touch_list(int access)
{
    RR_Touch_List  *rr_touch_list_aux;
    if (access == LIST_RW)
        return (rr_touch_list);
    else
    {
        rr_touch_list_aux = new RR_Touch_List(rr_touch_list);
        return (rr_touch_list_aux);
    }
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::add_rr_touch(RR_Touch *new_rr_touch)
{
    if (new_rr_touch != NULL)
        rr_touch_list->add_tail_rr_touch( new_rr_touch );
    else
        debug(3,"Rew_Reach_Distrib::add_rr_touch: Can't add NULL pointer");
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::clear_rr_touch_list()
{
    RR_Touch *rr_touch_aux;

    rr_touch_aux = rr_touch_list->get_1st_rr_touch();
    while ( rr_touch_aux != NULL )
    {
        delete rr_touch_aux;
        rr_touch_aux = rr_touch_list->get_1st_rr_touch();
    }
}
//------------------------------------------------------------------------------
/* Show_type(): Returns the type of distribution */
int Rew_Reach_Distrib::show_type()
{
    return (REW_REACH_DIST);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::print_dist()
{
    fprintf ( stdout , "REW_REACH_DIST ");
    fprintf ( stdout , "Time: %f", delta_time);
}
//------------------------------------------------------------------------------
void Rew_Reach_Distrib::reset(int run=0)
{
    delta_time = INFINITE;
    cr = 0;
    ir = 0;
    level = 0;
    skip_event = FALSE;
    clear_stamp_list();
}
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
//  Stamp  <>  class used by pseudo distribution Rew_Reach_Distrib
//------------------------------------------------------------------------------
Stamp::Stamp(TGFLOAT a_d_time, TGFLOAT a_cr_target, int a_rew_id, int a_skip) : Chained_Element()
{
    delta_time = a_d_time;
    cr_target  = a_cr_target;
    rew_id  = a_rew_id;
    skip_flag = a_skip;
    debug(4,"Stamp::Stamp(TGFLOAT, TGFLOAT, int, int): creating object");
}
//------------------------------------------------------------------------------
Stamp::~Stamp()
{
    debug(4,"Stamp::Stamp(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
TGFLOAT Stamp::show_delta_time()
{
    return ( delta_time );
}
//------------------------------------------------------------------------------
void Stamp::ch_delta_time(TGFLOAT new_d_time)
{
    delta_time = new_d_time;
}
//------------------------------------------------------------------------------
int Stamp::show_rew_id()
{
    return ( rew_id );
}
//------------------------------------------------------------------------------
void Stamp::ch_rew_id(int new_id)
{
    rew_id = new_id;
}
//------------------------------------------------------------------------------
int Stamp::show_skip_flag()
{
    return ( skip_flag );
}
//------------------------------------------------------------------------------
void Stamp::ch_skip_flag(int new_skip)
{
    skip_flag = new_skip;
}
//------------------------------------------------------------------------------
TGFLOAT Stamp::show_cr_target()
{
    return ( cr_target );
}
//------------------------------------------------------------------------------
void Stamp::ch_cr_target(TGFLOAT new_cr_target)
{
    cr_target = new_cr_target;
}
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
Stamp_List::Stamp_List() : Chained_List()
{
}
//------------------------------------------------------------------------------
Stamp_List::Stamp_List(Stamp_List *stamp_list) :
                 Chained_List( (Chained_List *)stamp_list )
{
}
//------------------------------------------------------------------------------
Stamp_List::~Stamp_List()
{
    Stamp *stamp_aux;

    if (access == LIST_RW)
        while (!is_empty())
        {
            stamp_aux = get_1st_stamp();
            delete stamp_aux;
        }
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
int Stamp_List::add_tail_stamp(Stamp *new_stamp)
{
    return ( add_tail( (Chained_Element *)new_stamp) );
}
//------------------------------------------------------------------------------
Stamp *Stamp_List::show_1st_stamp()
{
    return ( (Stamp *)show_head() );
}
//------------------------------------------------------------------------------
Stamp *Stamp_List::get_1st_stamp()
{
    return ( (Stamp *) del_head() );
}
//------------------------------------------------------------------------------
Stamp *Stamp_List::get_curr_stamp()
{
    return ( (Stamp *) del_elem() );
}
//------------------------------------------------------------------------------
Stamp *Stamp_List::show_next_stamp()
{
    return ( (Stamp *)show_next() );
}
//------------------------------------------------------------------------------
Stamp *Stamp_List::show_curr_stamp()
{
    return ( (Stamp *)show_curr() );
}
//------------------------------------------------------------------------------
int Stamp_List::query_stamp(int rew_id)
{
    Stamp *stamp_aux;

    stamp_aux = show_1st_stamp();
    while (stamp_aux != NULL)
    {
        if ( stamp_aux->show_rew_id() == rew_id)
            return (1);
        stamp_aux = show_next_stamp();
    }
    return (0);
}
//------------------------------------------------------------------------------
int Stamp_List::have_to_skip()
{
    /* if there is at least one skip_flag==FALSE then the event is real and   */
    /* this method retuns 0 else returns 1                                    */
    Stamp *stamp_aux;

    stamp_aux = show_1st_stamp();
    while (stamp_aux != NULL)
    {
        if ( stamp_aux->show_skip_flag() == FALSE)
            return (0);
        stamp_aux = show_next_stamp();
    }
    return (1);
}
//------------------------------------------------------------------------------
int Stamp_List::show_no_stamp()
{
    return ( show_no_elem() );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
//  RR_Touch  <>  class used by pseudo distribution Rew_Reach_Distrib
//------------------------------------------------------------------------------
RR_Touch::RR_Touch( int a_rew_type, TGFLOAT a_expr_value, int a_direction,
                    Reward_Measure *a_rew_meas) : Chained_Element()
{
    rew_type   = a_rew_type;   // GET_CR_TYPE or GET_CR_SUM_TYPE
    expr_value = a_expr_value;
    direction  = a_direction;

    rew_meas   = a_rew_meas;
    rate_rew_desc = NULL;
    rate_rew_sum = NULL;

    debug(4,"RR_Touch::RR_Touch(TGFLOAT, int): creating object");
}
//------------------------------------------------------------------------------
RR_Touch::~RR_Touch()
{
    debug(4,"RR_Touch::RR_Touch(): destroying object");
}
//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
int RR_Touch::show_rew_type()
{
    return ( rew_type );
}
//------------------------------------------------------------------------------
void RR_Touch::ch_rew_type( int new_type )
{
    rew_type = new_type;
}
//------------------------------------------------------------------------------
int RR_Touch::show_direction()
{
    return ( direction );
}
//------------------------------------------------------------------------------
void RR_Touch::ch_direction( int new_direction )
{
    direction = new_direction;
}
//------------------------------------------------------------------------------
TGFLOAT RR_Touch::show_expr_value()
{
    return ( expr_value );
}
//------------------------------------------------------------------------------
void RR_Touch::ch_expr_value(TGFLOAT new_expr)
{
    expr_value = new_expr;
}
//------------------------------------------------------------------------------
Reward_Measure *RR_Touch::show_rew_meas()
{
    return ( rew_meas );
}
//------------------------------------------------------------------------------
void RR_Touch::ch_rew_meas(Reward_Measure *new_rew_meas)
{
    rew_meas = new_rew_meas;
}
//------------------------------------------------------------------------------
Rate_Reward_Desc *RR_Touch::show_rate_rew_desc()
{
    return ( rate_rew_desc );
}
//------------------------------------------------------------------------------
void RR_Touch::ch_rate_rew_desc(Rate_Reward_Desc *new_rate_rew_desc)
{
    rate_rew_desc = new_rate_rew_desc;
}
//------------------------------------------------------------------------------
Rate_Reward_Sum *RR_Touch::show_rate_rew_sum()
{
    return ( rate_rew_sum );
}
//------------------------------------------------------------------------------
void RR_Touch::ch_rate_rew_sum(Rate_Reward_Sum *new_rate_rew_sum)
{
    rate_rew_sum = new_rate_rew_sum;
}
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
RR_Touch_List::RR_Touch_List() : Chained_List()
{
}
//------------------------------------------------------------------------------
RR_Touch_List::RR_Touch_List(RR_Touch_List *rr_touch_list) :
                 Chained_List( (Chained_List *)rr_touch_list )
{
}
//------------------------------------------------------------------------------
RR_Touch_List::~RR_Touch_List()
{
    RR_Touch *rr_touch_aux;

    if (access == LIST_RW)
        while (!is_empty())
        {
            rr_touch_aux = get_1st_rr_touch();
            delete rr_touch_aux;
        }
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
int RR_Touch_List::add_tail_rr_touch(RR_Touch *new_rr_touch)
{
    return ( add_tail( (Chained_Element *)new_rr_touch) );
}
//------------------------------------------------------------------------------
RR_Touch *RR_Touch_List::show_1st_rr_touch()
{
    return ( (RR_Touch *)show_head() );
}
//------------------------------------------------------------------------------
RR_Touch *RR_Touch_List::get_1st_rr_touch()
{
    return ( (RR_Touch *) del_head() );
}
//------------------------------------------------------------------------------
RR_Touch *RR_Touch_List::get_curr_rr_touch()
{
    return ( (RR_Touch *) del_elem() );
}
//------------------------------------------------------------------------------
RR_Touch *RR_Touch_List::show_next_rr_touch()
{
    return ( (RR_Touch *)show_next() );
}
//------------------------------------------------------------------------------
RR_Touch *RR_Touch_List::show_curr_rr_touch()
{
    return ( (RR_Touch *)show_curr() );
}
//------------------------------------------------------------------------------
int RR_Touch_List::show_no_rr_touch()
{
    return ( show_no_elem() );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R   ||||   L I S T
//------------------------------------------------------------------------------
Init_Distrib::Init_Distrib( Random_obj *random_obj ):Distribution( random_obj )
{
    debug( 4, "Init_Distrib::Init_Distrib( long ): creating object" ); 
}

Init_Distrib::~Init_Distrib()
{
    debug( 4, "Init_Distrib::~Init_Distrib(): destroying object" );  
}
//------------------------------------------------------------------------------
//     M E T H O D S   |||   L I S T
//------------------------------------------------------------------------------
TGFLOAT Init_Distrib::next_sample( Object_State * )
{
    return init_event_time;
}

int Init_Distrib::show_type()
{
    return INIT_DIST;
}

void Init_Distrib::print_dist()
{
    // do nothing
}

void Init_Distrib::reset(int)
{
    // do nothing
    debug( 4, "Init_Distrib::reset(): do nothing" );
}
