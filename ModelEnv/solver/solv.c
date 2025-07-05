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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* Prototypes */

#define DEFAULT_PRECISION  (1e-06)
#define MAXSTRING          255


int interface_Gauss_Siedel(char *, double, double);
int interface_Jacobi(char *, double,double);
int interface_SOR(char *, double, double);
int interface_Power(char *, double, double);
int interface_GTH_no_blocks(char *);
int interface_GTH(char *);
int interface_point_probability(char *, double);
int interface_breward(char *, char *, int, double);
int interface_omega(char *, char *, double);
int interface_det_event(char *, double, char, double);
int interface_expected_transient_reward(char *, char *, double);
int interface_approximation_cumulative_reward_direct(char *,int, char *,int);
int interface_approximation_cumulative_reward_iterative(char *, char *,int, double,int,double);
int interface_espa_direct(char *, int, int ,char *);
int interface_espa_iterative(char *,int, char *,double, int, double);
int interface_transient_impulse_reward( char *, char *, double );

void usage_main()
{
    printf("\nUsage: solv <model name> <method> [parameters of solution method]\n");
    printf("model name: base model name;\n");

    printf("method:     1 - Gauss-Siedel, 2 - Jacobi, 3 - SOR, 4 - Power Method,\n");
    printf("            5 - GTH no block, 6 - GTH block elimination,\n");
    printf("            7 - Uniformization\n");
    printf("            8 - Bounded cumulative reward,\n");
    printf("            9 - Fraction of time the accumulated reward is above a level,\n");
    printf("           10 - Operational time and related measures,\n");
    printf("           11 - Non-Markovian models (deterministic events),\n");
    printf("           12 - Expected cumulative reward;\n");
    printf("           13 - Cumulative Rate Reward - Direct Technique\n");
    printf("           14 - Cumulative Rate Reward - Iterative Technique\n");
    printf("           15 - Efficient Transient State Prob Approximation - Direct\n");
    printf("           16 - Efficient Transient State Prob Approximation - Iterative\n\n");
    printf("           17 - Expected impulse reward;\n");
}  

   


void usage_iterative()
{
   printf("\nUsage        : solv <model name> <method> <precision> <max iterations>\n");
   printf("model name     : base model name\n");
   printf("method         : 1 - Gauss-Siedel, 2 - Jacobi, 3 - SOR, 4 - Power Method\n");
   printf("precision      : error bound\n");
   printf("max iterations : max  number of iterations\n\n");   
}  


void usage_uniformization()
{
   printf("\nUsage   : solv <model name> <method> <precision>\n");
   printf("model name: base model name\n");
   printf("method    : 7 - Uniformization\n");
   printf("precision : error bound\n\n");
}


void usage_transient()
{
   printf("\nUsage    : solv <model name> <method> [reward] <precision>\n");
   printf("model name : base model name\n");   
   printf("methods    : 8 - Bounded cumulative reward\n");
   printf("             9 - Fraction of time the accumulated reward is above a level\n");
   printf("            10 - Operational time and related measures\n");
   printf("            12 - Expected cumulative reward\n"); 
   printf("            17 - Expected impulse reward;\n");
   printf("reward     : <object name>.<reward name> (only 8, 10, 12 and 17)\n");
   printf("precision  : error bound\n\n");
}   

void usage_Non_markovian()
{
   printf("\nUsage        : solv <model name> <method> <precision> <solution> [max iterations]\n");
   printf("model name     : base model name\n");
   printf("method         : 11 - Non-Markovian models (deterministic events)\n");
   printf("precision      : error bound\n");
   printf("solution       : solution method: g = GTH, p = Power, s = SOR\n");
   printf("max iterations : max number of iterations (case solution = SOR or Power)\n\n");  
}
   

void usage_reward_direct()
{
   printf("\nUsage      : solv <model name> <method> <erlang stages> <reward> <measure_opt>\n");
   printf("model name   : base model name\n");
   printf("method       : 13 - Cumulative Rate Reward - Direct Technique\n");
   printf("erlang stages: total number of erlang stages in each observation interval\n");    
   printf("reward       : <object name>.<reward name>\n");
   printf("measure_opt  : measure of interest: 1= State Reward, 2= Set Reward\n");
}   

void usage_reward_iterative()
{
   printf("\nUsage        : solv <model name> <method> <reward> <measure_opt> <solution>\n");
   printf("                 <precision><max iterations> \n"); 
   printf("model name     : base model name\n");
   printf("method         : 14 - Cumulative Rate Reward - Iterative Technique\n"); 
   printf("reward         : <object name>.<reward name>\n");
   printf("measure_opt    : measure of interest: 1 = State Reward, 2 = Set Reward\n");
   printf("solution       : solution method: 1 = SOR, 2 = Gauss, 3 =  Jacobi, 4 = Power.\n");
   printf("precision      : error bound\n");
   printf("max iterations : max number of iterations\n\n");
}    
  
void usage_espa_direct()
{
   printf("\nUsage        : solv <model name> <method> <erlang stages>  <measure_opt> [state_var]\n");
   printf("model name     : base model name\n");
   printf("method         : 15 - Efficient Transient State Prob Approximation - Direct\n");
   printf("erlang stages  : total number of erlang stages in each observation interval\n");
   printf("measure_opt    : measure of interest: 1 = State Probability, 2 = Set Probability");
   printf("                 3 = Expected Value\n");   
   printf("state_var      : <object name>.<reward name> (if measure_opt = 3) \n");
}

void usage_espa_iterative()
{
   printf("\nUsage        : solv <model name> <method> <measure_opt> [state_var]\n");
   printf("                 <solution> <precision> <max iterations> \n"); 
   printf("model name     : base model name\n");
   printf("method         : 16 - Efficient Transient State Prob Approximation - Iterative\n"); 
   printf("measure_opt    : measure of interest: 1 = State Probability, 2 = Set Probability");
   printf("                 3 = Expected Value\n");
   printf("state_var      : <object name>.<reward name> (if measure_opt = 3) \n");
   printf("solution       : solution method: 1 = SOR, 2 = Gauss, 3 =  Jacobi, 4 = Power.\n");
   printf("precision      : error bound\n");   
   printf("max iterations : max number of iterations\n\n");
} 


int main(int argc, char **argv)
{
    char   basename[MAXSTRING];
    char   rewardname[MAXSTRING];
    char   method[MAXSTRING];
    char   state_var[MAXSTRING];
    int    choice;
    double precision;
    int    status;
    char   emb_sol;
    double iterations;
    int    R;
    int    measure_opt;
    int    solution;
    
   
  

    /**********************************************************************/
    /* Opcoes: 1- Gauss-Siedel, 2- Jacobi, 3- SOR, 4- Power Method        */
    /*         5- GTH sem particao, 6- GTH com particao, 7- Uniformizacao */
    /*         8- Bounded Cumulative Reward                               */
    /*         9- Fraction of time the accumulated reward is above a level*/
    /*        10- Operational time and related measures                   */
    /*        11- Non-Markovian models                                    */
    /*        12- Expected Cumulative Reward                              */
    /*        13- Cumulative Rate Reward - Direct Technique               */
    /*        14- Cumulative Rate Reward - Iterative Technique            */
    /*        15- Efficient Transient State Prob Approximation - Direct   */
    /*        16- Efficient Transient State Prob Approximation - Iterative*/
    /*        17- Expected Impulse Reward                                 */
    /**********************************************************************/     
    
  
    R = 0;
    measure_opt = 0;
    solution = 0;
    
      
    /* Caso a chamada esteja errada */
    
    iterations = 0.0;
    if (argc < 3) {
    	usage_main();
    	return(20);
    }
    
    strcpy(basename, argv[1]);
    choice    = atoi (argv[2]);
    precision = DEFAULT_PRECISION;
    emb_sol   = 'G';

    if ( (choice == 1) || (choice == 2) || (choice == 3) ||
         (choice == 4)) {
       if (argc < 5){   
          usage_iterative();
          return(20);
       }             
      precision = atof (argv[3]);
      iterations = atof (argv[4]);   
    }
  
    
    if (choice == 7){
       if (argc < 4){
          usage_uniformization();
          return(20);
       }
       precision = atof (argv[3]);
    }       
    
        
    if ( (choice == 8) || (choice == 9) || (choice == 10) || (choice == 12) || (choice == 17)) {
       if (argc < 4) {
          usage_transient();
          return(20);
       } 
       strcpy (rewardname, argv[3]);
       if (argc >= 5)
          precision = atof (argv[4]);
    }
    
    
    if ( (choice == 11) ) {
       if (argc <= 4) {
          usage_Non_markovian();
          return(20);
       } 
       precision = atof( argv[3] );
       emb_sol = *(argv[4]);
       
       if(emb_sol == 'g')
          iterations = 0.0;
       else
          iterations = atof(argv[5]);   
      }
    
 
    if (choice == 13){
      if (argc != 6){
         usage_reward_direct();
	 return(20);
      }
      R = atoi(argv[3]);
      strcpy (rewardname, argv[4]);
      measure_opt = atoi(argv[5]);
     } 
     
    if (choice == 14){
      if (argc != 8){
         usage_reward_iterative();
	 return(20);
      }
      strcpy (rewardname, argv[3]);
      measure_opt = atoi(argv[4]);
      solution    = atoi (argv[5]);
      precision   = atof (argv[6]);
      iterations  = atof (argv[7]);
     }
     
    if (choice == 15){
      if (argc <= 4){
         usage_espa_direct();
	 return(20);
      }
      R = atoi(argv[3]);
      measure_opt = atoi(argv[4]);
      if ((measure_opt == 3)&&(argc != 6))
        {
	    usage_espa_direct();
	    return(20);
	 }   
      if ((measure_opt == 3)&&(argc == 6))
 	 strcpy(state_var, argv[5]);  
      if (measure_opt !=3)
         strcpy(state_var, "");  
      } 
    

     if (choice == 16){
       if (argc <= 6){
          usage_espa_iterative();
          return (20);
       }
     measure_opt = atoi(argv[3]);
     if ((measure_opt == 3)&&(argc != 8))
        {
          usage_espa_iterative();
          return (20);
  	}   
     if ((measure_opt == 3)&&(argc ==8))  
	{
           strcpy(state_var, argv[4]);
           solution   = atoi(argv[5]);
           precision  = atof(argv[6]);
           iterations = atof(argv[7]);
	 }  
    if ((measure_opt != 3)&&(argc!=7))
        {
          usage_espa_iterative();
          return (20);
	}  	   
    if((measure_opt != 3) && (argc ==7))  
	{
           strcpy(state_var, " ");
	   solution   = atoi(argv[4]);
           precision  = atof(argv[5]);
           iterations = atof(argv[6]);
	}      
    }    
                       
           
    /* Chamando as interfaces */
    
    switch(choice)
    {
         case  1: status = interface_Gauss_Siedel(basename,precision,iterations);
                  strcpy(method,"Gauss-Siedel");
                  break;
            
         case  2: status = interface_Jacobi(basename, precision,iterations);
                  strcpy(method,"Jacobi");
                  break;
            
         case  3: status = interface_SOR(basename, precision,iterations);
                  strcpy(method,"SOR");
                  break;
            
         case  4: status = interface_Power(basename, precision,iterations);
                  strcpy(method,"Power");
                  break;
            
         case  5: status = interface_GTH_no_blocks(basename);
                  strcpy(method,"GTH no blocks");
                  break;
            
         case  6: status = interface_GTH(basename);
                  strcpy(method,"GTH block elimination");
                  break;
                 
         case  7: status = interface_point_probability(basename, precision);
                  strcpy(method,"Uniformization");
                  break;
   
         case  8: status = interface_breward(basename, rewardname, 8, precision);
                  strcpy(method,"Bounded Cumulative Reward");
                  break;

         case  9: status = interface_breward(basename, rewardname, 9, precision);
                  strcpy(method,"Fraction of time the accumulated reward is above a level");
                  break;
            
         case 10: status = interface_omega(basename, rewardname, precision);
                  strcpy(method,"Operational time and related measures");
                  break;

         case 11: status = interface_det_event(basename, precision, emb_sol,iterations);
                  strcpy(method,"Non-Markovian models");
                  break;
            
         case 12: status = interface_expected_transient_reward(basename, rewardname, precision);
                  strcpy(method,"Expected Cumulative Reward");
                  break;
                  
	 case 13: status = interface_approximation_cumulative_reward_direct(basename, R, rewardname, measure_opt);
                  strcpy(method,"Cumulative Rate Reward - Direct Technique");
                  break;
	
        case 14: status = interface_approximation_cumulative_reward_iterative(basename,rewardname,measure_opt,precision,solution,iterations);
                  strcpy(method,"Cumulative Rate Reward - Iterative Technique");
                  break;
                  
         case 15: status = interface_espa_direct(basename, R, measure_opt,state_var);         		  
                  strcpy(method,"Efficient Transient State Probability  Approximation - Direct Technique");
                  break;
                  
         case 16: status = interface_espa_iterative(basename, measure_opt, state_var, precision, solution, iterations);         
                  strcpy(method,"Efficient Transient State Probability  Approximation - Iterative Technique");
                  break;
  
         case 17: status = interface_transient_impulse_reward(basename, rewardname, precision);
                  strcpy(method,"Expected Impulse Reward");
                  break;


         default: usage_main();
                  return (21);
    }
    
    if (status != 0)
    {
        fprintf(stderr, "\n\n%s solution method did not work. Check previous stderr messages.\n\n", method);
    }
    else
    {
        fprintf(stderr, "\n\n%s solution method ended with success.\n\n", method);
        status = 0;
    }

    return( status );
}

