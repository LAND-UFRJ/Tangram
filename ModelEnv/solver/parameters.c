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
#include <ctype.h>
#include <errno.h>
#include "sparseMatrix.h"
#include "parameters.h"

/***************************************************************/
/* Get intervals of time for transient probability evaluation  */
/***************************************************************/
double *get_intervals( f_intervals, num_intervals )
FILE *f_intervals;
int  *num_intervals;
{
   int    i, count_pnts;
   double dj;
   char   line[100];             /* line read from intervals's file         */
   double *intervals;            /* ptr time intervals, vector              */
   double init_time;             /* initial time of a group of intervals    */
   double final_time;            /* final time of a group of intervals      */
   double interval_size;         /* size of a group of intervals            */
   int    num_points;            /* number of points of an interval         */
   double delta;                 /* equal space between interval times      */



    fscanf(f_intervals, "%d", num_intervals);

    if (*num_intervals <= 0)
        return(NULL);


    intervals = (double *)my_calloc(*num_intervals, sizeof(double));


    /* Reading intervals */
    for (i = 0; i < (*num_intervals); )
    {
        fscanf(f_intervals, "%s", line);
        
        /* Caso de definicao por blocos de mesmo tamanho */
        if (line[0] == 'n')
        {
            fscanf(f_intervals, "%lf", &init_time);
            fscanf(f_intervals, "%lf", &final_time);
            fscanf(f_intervals, "%d", &num_points);
            
            interval_size = final_time - init_time;
            delta = interval_size / (double)(num_points - 1);
            
            /* teste */
            /*
            printf("Interval Size: %f  Number of points: %d Initial Time: %f Final time: %f\n",
                                               interval_size, num_points, init_time, final_time);
           */
            
            for (dj = init_time, count_pnts = 1; count_pnts < num_points; dj += delta, count_pnts++)
            {
                intervals[i] = dj;
                
                i++;
            }
	    
	    intervals[i] = final_time;
	    i++;
        }
        else
        {
            if( !isdigit( (int)line[ 0 ] ) )
                return( NULL );

            sscanf(line, "%lf", &(intervals[i]));

            /* teste */
            /* printf("Interval size: %.6e\n", intervals[i]); */
            
            i++;
        }
    }
    
    return(intervals);
}


/***************************************************************/
/* Get initial probability distribution                        */
/***************************************************************/
double *get_initial_pi(f_init_prob, num_states)
FILE *f_init_prob;
int  num_states;
{
   int    j;
   char   line[100];        /* line read from partition's file    */
   double *pi1;            /* ptr to initial prob. distrib.      */
   int    begin_state;     /* first state of an equiprobable set */
   int    end_state;       /* end state of an equiprobable set   */
   int    state;           /* state being considered             */
   double equal_prob;      /* state probability                  */
   double value;           /* probability value                  */



    pi1 = (double *)my_calloc(num_states, sizeof(double));
    memset(pi1, 0, num_states * sizeof(double));


    /* Reading states */
    if (fscanf(f_init_prob, "%s", line) != 1)
        return(NULL);

    while (!feof(f_init_prob))
    {
        /* teste                 */
        /* printf("%s\n", line); */
        
        /* Caso de todos os estados serem equiprovaveis */
        if (!strcmp(line, "e_All"))
        {
            for (j = 0; j < num_states; j++)
                pi1[j] = 1.0/num_states;
            
            return(pi1);
        } 
        
        /* Caso de definicao por blocos de mesmo tamanho */
        if (line[0] == 'e')
        {
            fscanf(f_init_prob, "%d", &begin_state);
            fscanf(f_init_prob, "%d", &end_state);
            fscanf(f_init_prob, "%lf", &equal_prob);
            
            /* teste */
            /* printf("First state of set: %d, End state of set: %d  Equal prob. of set: %f\n",
                                                         begin_state, end_state, equal_prob);   */
            
            for (j = begin_state - 1; j <= end_state - 1; j++)
                pi1[j] = equal_prob;
        }
        else
            if( isdigit( (int)line[0] ) )
            {
                sscanf(line, "%d", &state);
                fscanf(f_init_prob, "%lf", &value);
                pi1[state - 1] = value;

                /* teste */
                /* printf("State %d prob: %.6e\n", state, pi1[state - 1]); */
            }
            else
                return(NULL);
            
        fscanf(f_init_prob, "%s", line);
    }
    
    return(pi1);
}


/***********************************************************/
/* Get partitions for block elimination GTH algorithm      */
/***********************************************************/
int *get_partitions(f_partition, max_state)
FILE *f_partition;
int  max_state;
{
   int    i, j;
   char   line[100];              /* line read from partition's file         */
   int    *partition_vector;     /* ptr to cont memory space; partition vec */
   int    curr_state;            /* current state while setting partitions  */
   int    end_state;             /* end_state of partition                  */
   int    partition_size;        /* size of each partition                  */
   int    num_partitions;        /* number of partitions                    */



    fscanf(f_partition, "%d", &num_partitions);
 
    partition_vector = (int *)my_calloc(1 + num_partitions, sizeof(int));


    curr_state = 1;
    
    /* Reading partitions */
    for (i = 1; i <= num_partitions; )
    {
        fscanf(f_partition, "%s", line);
        
        /* Caso de definicao por blocos de mesmo tamanho */
        if (line[0] == 's')
        {
            fscanf(f_partition, "%d", &partition_size);
            fscanf(f_partition, "%d", &end_state);
            
            /* teste */
            /* printf("Partition Size: %d  Part. end state: %d\n",
                                       partition_size, end_state); */
            
            for (j = curr_state; j < end_state; j += partition_size)
            {
                partition_vector[i] = partition_size;
                
                i++;
            }
            
            curr_state = end_state+1;
        }
        else
        {
            if( !isdigit( (int)line[0] ) )
                return( NULL );

            sscanf(line, "%d", &end_state);

            /* teste */
            /* printf("Partition end state: %d\n", end_state); */
            
            partition_vector[i] = (end_state - curr_state) + 1;
            i++;
            
            curr_state = end_state+1;
        }
    }
    
    partition_vector[0] = num_partitions;

    if (curr_state != max_state+1)
    {
        fprintf(stderr,"GTH block elimination: Invalid partitions, last state must be the maximum state!\n");
        return(NULL);
    }

    return(partition_vector);
}


/*****************************************************************************/
/* double *get_rewards(f_rewards, size)                                      */
/*                                                                           */
/* Reads the reward vector from reward file. (the reward vector is 0-based)  */
/*****************************************************************************/
double *get_rewards(f_rewards, size)
FILE *f_rewards;              /* ptr. to reward file   */
int  size;                    /* number of states      */
{
    int    state;
    double reward_value;
    double *reward;
    char   low_bound[100];
    char   up_bound[100];
    int    status;
    
    
    reward = (double *)my_calloc(size, sizeof(double));
    
    /* Jumping bounds description */
    if (fscanf(f_rewards, "%s %s\n", low_bound, up_bound) != 2)
        return(NULL);
    
    /* Como o Carlos faz:          */
    /* fgets(line, MAXSTRING, fd); */

    /* Reading rewards */
    while (!feof(f_rewards)) 
    {
        status = fscanf(f_rewards, "%d %lf\n", &state, &reward_value);
        if (status != 2) 
	{
            /* perror("fscanf"); */
            fprintf (stderr, "Rate reward description: Invalid file format.\n");
            return(NULL);
        }
        reward[state - 1] = reward_value;       
    }
    
    return(reward);
}
/*****************************************************************************/
/* double *get_destination_states(f_dest)                                    */
/*                                                                           */
/* Reads the destination states in the dest file.                            */
/*****************************************************************************/
int *get_destination_states(f_dest)
FILE *f_dest;                  /* ptr. to dest file   */
{
    int    number_of_dest_states;
    int    status;
    int    state;
    int    count;
    char   low_bound[100];
    char   up_bound[100];
    int *dest;
    char c;
    double value;

    
    
    
    /* Jumping bounds description */
    fscanf(f_dest, "%s %s\n", low_bound, up_bound);
    
    number_of_dest_states = 0;
    /*Counting the total number of destination states*/
    c = fgetc(f_dest);
    while (c != EOF)
      {
          c = fgetc(f_dest);
          if (c == '\n')
             number_of_dest_states = number_of_dest_states + 1;
      }
    if (number_of_dest_states <= 0)
       {
          fprintf(stderr,"%s", "Number of destination states is equal zero.\n");
	  return(NULL);
       }    
    dest = (int *)my_calloc((number_of_dest_states + 1), sizeof(int)); 
    dest[0]= number_of_dest_states;
     
    rewind(f_dest);
     
    /* Jumping bounds description */
    fscanf(f_dest, "%s %s\n", low_bound, up_bound); 
     
    /* Reading destination states */
    for (count = 1; count <=number_of_dest_states; count++)
    {
        status = fscanf(f_dest, "%d %lf\n", &state, &value);
        if (status != 2) 
	{
            /* perror("fscanf"); */
            fprintf (stderr, "Destination State description: Invalid file format.\n");
            return(NULL);
        }
        dest[count] = state;       
    }
    
    return(dest);
}


/*****************************************************************************/
/* double *get_rewards_2(f_rewards, size)                                    */
/*                                                                           */
/* Reads the reward vector from reward file. (the reward vector is 0-based)  */
/* Number of total states in the first position                              */  
/*****************************************************************************/
double *get_rewards_2(f_rewards, size)
FILE *f_rewards;              /* ptr. to reward file   */
int  size;                    /* number of states      */
{
    int    state;
    double reward_value;
    double *reward;
    char   low_bound[100];
    char   up_bound[100];
    int    status;
    
    
    reward = (double *)my_calloc(size + 1, sizeof(double));
    
    /* Jumping bounds description */
    fscanf(f_rewards, "%s %s\n", low_bound, up_bound);
    
 
    /* Reading rewards */
    reward[0] = size;
    while (!feof(f_rewards)) 
    {
        status = fscanf(f_rewards, "%d %lf\n", &state, &reward_value);
        if (status != 2) 
	{
            /* perror("fscanf"); */
            fprintf (stderr, "Rate reward description: Invalid file format.\n");
            return(NULL);
        }
        reward[state] = reward_value;       
    }
    
    return(reward);
}

/***************************************************************/
/* Get initial probability distribution                        */
/* Total number of states in the first position.               */ 
/***************************************************************/
double *get_initial_pi_2(f_init_prob, num_states)
FILE *f_init_prob;
int  num_states;
{
   int    j;
   char   line[100];        /* line read from partition's file    */
   double *pi1;            /* ptr to initial prob. distrib.      */
   int    begin_state;     /* first state of an equiprobable set */
   int    end_state;       /* end state of an equiprobable set   */
   int    state;           /* state being considered             */
   double equal_prob;      /* state probability                  */
   double value;           /* probability value                  */



    pi1 = (double *)my_calloc(num_states + 1, sizeof(double));
    memset(pi1, 0, (num_states + 1) * sizeof(double));


    /* Reading states */
    fscanf(f_init_prob, "%s", line);
    while (!feof(f_init_prob))
    {
        /* All states are equiprobable*/
        if (!strcmp(line, "e_All"))
        {
            for (j = 1; j <= num_states; j++)
                pi1[j] = 1.0/num_states;
            
            return(pi1);
        } 
        
        /* Blocks with same size */
        if (line[0] == 'e')
        {
            fscanf(f_init_prob, "%d", &begin_state);
            fscanf(f_init_prob, "%d", &end_state);
            fscanf(f_init_prob, "%lf", &equal_prob);
            
            
            for (j = begin_state; j <= end_state ; j++)
                pi1[j] = equal_prob;
        }
        else
            if( isdigit( (int)line[0] ) )
            {
                sscanf(line, "%d", &state);
                fscanf(f_init_prob, "%lf", &value);
                pi1[state] = value;

            }
            
        fscanf(f_init_prob, "%s", line);
    }
    
    return(pi1);
}

/***************************************************************/
/* Get intervals of time for transient probability evaluation  */
/***************************************************************/
double *get_intervals_R(f_intervals, num_intervals,set_of_intervals,set_of_R)
FILE *f_intervals;
int  num_intervals;
int  set_of_intervals;
info_intervals *set_of_R;
{
   int    i, count_pnts;
   double dj;
   char   line[100];              /* line read from partition's file         */
   double *intervals;            /* ptr time intervals, vector              */
   double init_time;             /* initial time of a group of intervals    */
   double final_time;            /* final time of a group of intervals      */
   double interval_size;         /* size of a group of intervals            */
   int    num_points;            /* number of points of an interval         */
   double delta;                 /* equal space between interval times      */
   int    R;                     /* number of stages in subinterval         */
   int    k;                     /*auxiliary variable                       */


    
    intervals = (double *)my_calloc((num_intervals + 1), sizeof(double));
    intervals[0] = num_intervals; 
    delta = 0;
    k = 0;
   
    /* Reading intervals */
	for (i = 1; i <= (num_intervals); )
	{
	fscanf(f_intervals, "%s", line);

	/* Caso de definicao por blocos de mesmo tamanho */
	if (line[0] == 'n')
	{
	    fscanf(f_intervals, "%lf", &init_time);
	    fscanf(f_intervals, "%lf", &final_time);
	    fscanf(f_intervals, "%d",  &num_points);
	    fscanf(f_intervals, "%d",  &R);

	    interval_size = (double)final_time - init_time;
	    delta = interval_size / (double)(num_points);

	    
	    set_of_R[k].R = R;
	    set_of_R[k].step = delta;
	    set_of_R[k].total_points = num_points; 

	    for (dj = init_time + delta, count_pnts = 1; count_pnts <= num_points; dj += delta, count_pnts++)
	    {
        	intervals[i] = dj;

        	i++;
	    }
      k++;  
	}
	}
    
    return(intervals);
}

