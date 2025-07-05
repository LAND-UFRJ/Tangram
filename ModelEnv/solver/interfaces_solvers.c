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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "sparseMatrix.h"
#include "det_event.h"
#include "bounds.h"
#include "parameters.h"
#include "ImpMatrix.h"
#include "functions_espa_esra.h"

/*
int    check_diagonal();   * in module Matrix.c         *
int    *get_partitions();  * in module parameters.c     *
double *get_intervals();   * in module parameters.c     *
double *get_rewards();     * in module parameters.c     *
double *get_initial_pi();  * in module parameters.c     *
*/


double *gauss_siedel();    
double *jacobi();
double *sor();
double *power();
double *GTH_no_blocks();
double *GTH();
int    uniformization_method();
double *expected_trans_reward();
double *expected_trans_imp_reward();


/**********************************************************/
/* normalize vector to sum up 1                           */
/**********************************************************/
void normalize_vector(pi, dim)
double *pi;  /* vector to norm. contiguous memory */
int    dim;  /* vector's size                     */
{
    int i;
    double sum;

    sum = 0.0;
    for (i = 0; i < dim; i++) 
    {
        if (pi[i] < 1e-25)
            pi[i] = 0.0;
        else 
        {
           if (pi[i] > 1e10)
               pi[i] = 1.0;
        }
    }
    
    for (i = 0; i < dim; i++) 
        sum = sum + pi[i];
        
    for (i = 0; i < dim; i++) 
        pi[i] = pi[i] / sum;
}



/******************************************************/
/* Gauss-Siedel method interface                      */
/******************************************************/
int interface_Gauss_Siedel(basename, precision,no_max_iterations)
char   *basename;
double precision;
double no_max_iterations;
{
    int i;
    int N;                 /* matrix size */
    char mat_type;         /* matrix type */
    char tpname[1024];     /* the name of input file                      */
    char outname[1024];    /* the name of an output file                  */
    FILE *f_tp, *f_out;    /* ptr to in/out files                         */
    double *pi;            /* ptr to steady state probabilities           */
    double *x;             /* ptr to initial steady state probabilities   */
    double *b;             /* ptr to rem: xA = b; pi.Q = 0; pi = x; b = 0 */
    Matrix *M;             /* ptr to rate matrix                          */
    int no_of_iterations;  /* no. of iterations to converge               */
    int stop_condition;



    /* name of output and input files */
    sprintf(outname, "%s.SS.gauss", basename);
    sprintf(tpname, "%s.generator_mtx", basename);

    /* reads rate matrix */
    if (! (f_tp = fopen(tpname, "r")) ) 
    {
        fprintf(stderr,"Gauss-Siedel method: Error while opening file %s\n",tpname);
        return(30);
    }

 
    M = get_trans_matrix(f_tp);

    fprintf(stderr,"\nGauss-Siedel method: Using %s file. Assuming it is a rate matrix.\n",tpname);

    mat_type = 'Q';
    generate_diagonal(mat_type, M);
 

    /* allocate vectors x = pi_0 b = 0 */
    x = (double *)my_calloc(M->num_col,sizeof(double));
    b = (double *)my_calloc(M->num_col,sizeof(double));


    /* set initial values */
    for (i = 0; i < M->num_col; i++) 
        x[i] = (1.0/(M->num_col));   /* pi_i = 1/N */
     
    for (i = 0; i < M->num_col; i++) 
        b[i] = 0.0;                  /* b = 0 */ 
     
     
    /* writing */
    if (!(f_out = fopen(outname, "w"))) 
    {  
        fprintf(stderr,"Gauss-Siedel method: Error while opening file %s\n",outname);
        return(31);
    }


    /* nill vector or matrix is ignored */
    if (!x || !b)	
        return(40);
        
    if (!M)
        return(50);


    N = M->num_col;             /* dim. of M, and both b and x must have N elements */

    if (!check_diagonal(M, N)) 
    {                           /* some disgonal element is missing */
        fprintf(stderr, "Gauss-Siedel method: Missing diagonal, Gauss-Seidel method aborted!\n");
        return(50);
    }


    /* Gauss_Siedel(A, x, b) solves xA = b */
    pi = gauss_siedel(M, x, b, precision, &no_of_iterations, no_max_iterations,&stop_condition);	

    if (pi == NULL)             /* problems with the solution method */
    {
        fclose(f_tp);
        fclose(f_out);
        return(85);
    }

   

    /* normalization step */
    normalize_vector(pi,M->num_col);


    /* Printing step */
    fprintf(f_out,"#Solution Method: GAUSS_SIEDEL alg.\n");
    fprintf(f_out,"#No. of iterations: %d\n",no_of_iterations);
    
    /* pi[0] = 1st probability component */
    for (i = 0; i < M->num_col; i++) 
        fprintf(f_out, "%d %.10e\n", i+1, pi[i]);
     
    fprintf(f_out,"\n#**************************\n");
    
    
    fclose(f_tp);
    fclose(f_out);

    free(b);
    free(x);
    free_Matrix(1,M);
    
    if (stop_condition == MAX_ITERATIONS)
      {
        return (89);
      }
    
    return(0);
}



/******************************************************/
/* Jacobi method interface                            */
/******************************************************/
int interface_Jacobi(basename, precision,no_max_iterations)
char   *basename;
double precision;
double no_max_iterations;
{
    int i;
    int N;                 /* matrix size */
    char mat_type;         /* matrix type */
    char tpname[1024];     /* the name of input file                      */
    char outname[1024];    /* the name of an output file                  */
    FILE *f_tp, *f_out;    /* ptr to in/out files                         */
    double *pi;            /* ptr to steady state probabilities           */
    double *x;             /* ptr to initial steady state probabilities   */
    double *b;             /* ptr to rem: xA = b; pi.Q = 0; pi = x; b = 0 */
    Matrix *M;             /* ptr to rate matrix                          */
    int no_of_iterations;  /* no. of iterations to converge               */
    int stop_condition;


    /* name of output and input files */
    sprintf(outname, "%s.SS.jacobi", basename);
    sprintf(tpname, "%s.generator_mtx", basename);

    /* reads rate matrix */
    if (! (f_tp = fopen(tpname, "r")) ) 
    {
        fprintf(stderr,"Jacobi method: Error while opening file %s\n",tpname);
        return(30);
    }
 
 
    M = get_trans_matrix(f_tp);

    fprintf(stderr,"\nJacobi method: Using %s file. Assuming it is a rate matrix.\n",tpname);

    mat_type = 'Q';
    generate_diagonal(mat_type, M);
 

    /* allocate vectors x = pi_0 b = 0 */
    x = (double *)my_calloc(M->num_col,sizeof(double));
    b = (double *)my_calloc(M->num_col,sizeof(double));


    /* set initial values */
    for (i = 0; i < M->num_col; i++) 
        x[i] = (1.0/(M->num_col));   /* pi_i = 1/N */
     
    for (i = 0; i < M->num_col; i++) 
        b[i] = 0.0;                  /* b = 0 */ 
     
     
    /* writing */
    if (!(f_out = fopen(outname, "w"))) 
    {  
        fprintf(stderr,"Jacobi method: Error while opening file %s\n",outname);
        return(31);
    }


    /* nill vector or matrix is ignored */
    if (!x || !b)	
        return(40);
        
    if (!M)
        return(50);


    N = M->num_col;             /* dim. of M, and both b and x must have N elements */

    if (!check_diagonal(M, N)) 
    {                           /* some disgonal element is missing */
        fprintf(stderr, "Jacobi method: Missing diagonal, Jacobi method aborted!\n");
        return(50);
    }


    /* Jacobi(A, x, b) solves xA = b */
    pi = jacobi(M, x, b, precision, &no_of_iterations,no_max_iterations,&stop_condition);	

    if (pi == NULL)             /* problems with the solution method */
    {
        fclose(f_tp);
        fclose(f_out);
        return(85);
    }


    /* normalization step */
    normalize_vector(pi,M->num_col);


    /* Printing step */
    fprintf(f_out,"#Solution Method: JACOBI alg.\n");
    fprintf(f_out,"#No. of iterations: %d\n",no_of_iterations);
    
    /* pi[0] = 1st probability component */
    for (i = 0; i < M->num_col; i++) 
        fprintf(f_out, "%d %.10e\n", i+1, pi[i]);
         
    fprintf(f_out,"\n#**************************\n");
    
    fclose(f_tp);
    fclose(f_out);

    free(b);
    free(x);
    free_Matrix(1,M);
    
    if (stop_condition == MAX_ITERATIONS)
      {
        return (89);
      }
    
    return(0);
}



/******************************************************/
/* SOR method interface                               */
/******************************************************/
int interface_SOR(basename, precision,no_max_iterations)
char   *basename;
double precision;
double no_max_iterations;
{
    int i;
    int N;                /* matrix size */
    char mat_type;        /* matrix type */
    char tpname[1024];    /* the name of input file                      */
    char outname[1024];   /* the name of an output file                  */
    FILE *f_tp, *f_out;   /* ptr to in/out files                         */
    double *pi;           /* ptr to steady state probabilities           */
    double *x;            /* ptr to initial steady state probabilities   */
    double *b;            /* ptr to rem: xA = b; pi.Q = 0; pi = x; b = 0 */
    Matrix *M;            /* ptr to rate matrix                          */
    int no_of_iterations; /* no. of iterations to converge               */
    int stop_condition;


    stop_condition = 0;
    /* name of output and input files */
    sprintf(outname, "%s.SS.sor", basename);
    sprintf(tpname, "%s.generator_mtx", basename);

    /* reads rate matrix */
    if (! (f_tp = fopen(tpname, "r")) ) 
    {
        fprintf(stderr,"SOR method: Error while opening file %s\n",tpname);
        return(30);
    }
 
 
    M = get_trans_matrix(f_tp);

    fprintf(stderr,"\nSOR method: Using %s file. Assuming it is a rate matrix.\n",tpname);

    mat_type = 'Q';
    generate_diagonal(mat_type, M);
 

    /* allocate vectors x = pi_0 b = 0 */
    x = (double *)my_calloc(M->num_col,sizeof(double));
    b = (double *)my_calloc(M->num_col,sizeof(double));


    /* set initial values */
    for (i = 0; i < M->num_col; i++) 
        x[i] = (1.0/(M->num_col));   /* pi_i = 1/N */
     
    for (i = 0; i < M->num_col; i++) 
        b[i] = 0.0;                  /* b = 0      */ 

     
    /* writing */
    if (!(f_out = fopen(outname, "w"))) 
    {  
        fprintf(stderr,"SOR method: Error while opening file %s\n",outname);
        return(31);
    }


    /* nill vector or matrix is ignored */
    if (!x || !b)	
        return(40);
        
    if (!M)
        return(50);


    N = M->num_col;             /* dim. of M, and both b and x must have N elements */

    if (!check_diagonal(M, N)) 
    {                           /* some disgonal element is missing */
        fprintf(stderr, "SOR method: Missing diagonal, SOR method aborted!\n");
        return(50);
    }


    /* SOR(A, x, b) solves xA = b */
    pi = sor(M, x, b, precision, &no_of_iterations,no_max_iterations,&stop_condition);	

    if (pi == NULL)             /* problems with the solution method */
    {
        fclose(f_tp);
        fclose(f_out);
        unlink( outname );
        return(85);
    }


    /* normalization step */
    normalize_vector(pi, M->num_col);


    /* Printing step */
    fprintf(f_out,"#Solution Method: SOR alg.\n");
    fprintf(f_out,"#No. of iterations: %d\n",no_of_iterations);
    
    /* pi[0] = 1st probability component */
    for (i = 0; i < M->num_col; i++) 
    {
        fprintf(f_out, "%d %.10e\n", i+1, pi[i]);
    }   
         
    fprintf(f_out,"\n#**************************\n");
    
    
    fclose(f_tp);
    fclose(f_out);

    free(b);
    free(x);
    free_Matrix(1,M);
    
 
   
   if (stop_condition == MAX_ITERATIONS)
    {
        return (89);
    }

    
    return(0);
}



/******************************************************/
/* Power method interface                             */
/******************************************************/
int interface_Power(basename, precision,no_max_iterations)
char   *basename;
double precision;
double no_max_iterations;
{
    int i;
    int N;                /* matrix size */
    char mat_type;        /* matrix type */
    char tpname[1024];    /* the name of input file                    */
    char outname[1024];   /* the name of an output file                */
    FILE *f_tp, *f_out;   /* ptr to in/out files                       */
    double *pi;           /* ptr to steady state probabilities         */
    double *x;            /* ptr to initial steady state probabilities */
    Matrix *M;            /* ptr to rate matrix                        */
    Matrix *P;            /* ptr to probability matrix                 */
    double lambda;        /* uniformization rate                       */
    int no_of_iterations; /* no. of iterations to converge             */
    int stop_condition;


    stop_condition = 0;
    /* name of output and input files */
    sprintf(outname, "%s.SS.power", basename);
    sprintf(tpname, "%s.generator_mtx", basename);

    /* reads rate matrix */
    if (! (f_tp = fopen(tpname, "r")) ) 
    {
        fprintf(stderr,"Power method: Error while opening file %s\n",tpname);
        return(30);
    }
 
 
    M = get_trans_matrix(f_tp);

    fprintf(stderr,"\nPower method: Using %s file. Assuming it is a rate matrix.\n",tpname);

    mat_type = 'Q';
    generate_diagonal(mat_type, M);
 

    /* allocate vectors x = pi_0 b = 0 */
    x = (double *)my_calloc(M->num_col,sizeof(double));


    /* set initial values */
    for (i = 0; i < M->num_col; i++) 
        x[i] = (1.0/(M->num_col));   /* pi_i = 1/N */

          
    /* writing */
    if (!(f_out = fopen(outname, "w"))) 
    {  
        fprintf(stderr,"Power method: Error while opening file %s\n",outname);
        return(31);
    }


    /* nill vector or matrix is ignored */
    if (!x)	
        return(40);
        
    if (!M)
        return(50);


    N = M->num_col;             /* dim. of M and x must have N elements */

    if (!check_diagonal(M, N)) 
    {                           /* some disgonal element is missing */
        fprintf(stderr, "Power method: Missing diagonal, Power method aborted!\n");
        return(50);
    }
	
	
    /* Uniformizing matrix */
    P = uniformize_matrix(M, &lambda);


    /* Power(P, x) solves xP = x */
    pi = power(P, x, precision, &no_of_iterations,no_max_iterations,&stop_condition);	


    /* normalization step */
    normalize_vector(pi,M->num_col);


    /* Printing step */
    fprintf(f_out,"#Solution Method: Power Method\n");
    fprintf(f_out,"#No. of iterations: %d\n",no_of_iterations);
    
    /* pi[0] = 1st probability component */
    for (i = 0; i < M->num_col; i++) 
        fprintf(f_out, "%d %.10e\n", i+1, pi[i]);
            
    fprintf(f_out,"\n#**************************\n");
    
    
    fclose(f_tp);
    fclose(f_out);

    free(x);
    free_Matrix(1,M);
    free_Matrix(1,P);
    
   
   
   
   if (stop_condition == MAX_ITERATIONS)
    {
        return (89);
    }

    return(0);
}


/******************************************************/
/* Interface to Grassman/T./Heymann without partition */
/******************************************************/
int interface_GTH_no_blocks(basename)
char   *basename;
{
    int i;
    char mat_type;              /* matrix type                       */
    char tpname[1024];          /* the name of input file            */
    char outname[1024];         /* the name of an output file        */
    FILE *f_tp, *f_out;         /* ptr to in/out files               */
    double lambda;              /* uniformization rate               */
    Matrix *M;                  /* ptr to rate matrix                */
    Matrix *P;                  /* ptr to probability matrix         */
    double *pi;                 /* ptr to steady state probabilities */


    /* name of output and input files */
    sprintf(outname, "%s.SS.gth", basename);
    sprintf(tpname, "%s.generator_mtx", basename);

    /* reads rate matrix */
    if (! (f_tp = fopen(tpname, "r")) ) 
    {
        fprintf(stderr,"GTH no block method: Error while opening file %s\n",tpname);
        return(30);
    }
 

    M = get_trans_matrix(f_tp);

    fprintf(stderr,"\nGTH no block method: Using %s file. Assuming it is a rate matrix.\n",tpname);

    mat_type = 'Q';
    generate_diagonal(mat_type, M);

 
    /* Uniformizing matrix */
    P = uniformize_matrix(M, &lambda);


    /* GTH solution method without partition */
    pi = GTH_no_blocks(P);


    /* writing */
    if (!(f_out = fopen(outname, "w"))) 
    {  
        fprintf(stderr,"GTH no block method: Error while opening file %s\n",outname);
        return(31);
    }

    /* Printing step */
    fprintf(f_out,"#Solution Method: Stationary point probability (Grassman/Heymann alg. with no partitions)\n");
    fprintf(f_out,"#steady state vector:\n");
 
    /* pi[1] = 1st probability component */
    for (i = 1; i <= P->num_col; i++) 
        fprintf(f_out,"%d %.10e\n",i,pi[i]);
        
    fprintf(f_out,"\n#**************************\n");
    
    
    fclose(f_tp);
    fclose(f_out);
    
    free(pi);
    free_Matrix(1, P);
    free_Matrix(1, M);
    
    return(0);
}



/***********************************************************/
/* Interface to Grassman/T./Heymann block elimination alg. */
/***********************************************************/
int interface_GTH(basename)
char   *basename;
{
   int    i;
   char   mat_type;              /* matrix type                             */
   char   tpname[1024];          /* the name of input file                  */
   char   partition_name[1024];  /* the name of input file                  */
   char   outname[1024];         /* the name of an output file              */
   FILE   *f_tp, *f_out;         /* ptr to in/out files                     */
   FILE   *f_partition;          /* ptr to partition descritption file      */
   double lambda;                /* uniformization rate                     */
   Matrix *M,*P;                 /* ptr to rate and prob matrices           */
   double *pi;                   /* ptr to steady state probabilities       */
   int    *partition_vector;     /* ptr to cont memory space; partition vec */


    /* name of output and input files */
    sprintf(outname, "%s.SS.gthb", basename);
    sprintf(tpname, "%s.generator_mtx", basename);
    sprintf(partition_name, "%s.partition", basename);


    /* reads partition blocks */
    if (! (f_partition = fopen(partition_name, "r")) ) 
    {
        fprintf(stderr,"GTH block elim. method: Error while opening file %s\n", partition_name);
        return(30);
    }
  

    /* reads rate matrix */
    if (! (f_tp = fopen(tpname, "r")) ) 
    {
        fprintf(stderr,"GTH block elim. method: Error while opening file %s\n",tpname);
        return(30);
    }

    M = get_trans_matrix(f_tp);

    fprintf(stderr,"\nGTH block elim. method: Using %s file.\nAssuming it is a rate matrix.\n",tpname);

    mat_type = 'Q';
    generate_diagonal(mat_type, M);

 
    /* Uniformizing matrix */
    P = uniformize_matrix(M, &lambda);


    /* set partitions */            

    partition_vector = get_partitions(f_partition, (int)P->num_col);
   
    if (partition_vector == NULL)
    {
        fprintf(stderr,"GTH block elim. method: Problems with the partitions description\n");
        fclose(f_partition);
        fclose(f_tp);
 
        return(53);
    }
 
    
    /* debug information */
    /* for (i = 0; i <= partition_vector[0]; i++)
           printf("p[%d] = %d\n", i, partition_vector[i]); */
        

    /* GTH block elimination method */   
    pi = GTH(P, partition_vector);
    
    
    if (pi == NULL)
    {
        fprintf(stderr,"GTH block elim. method: trans. matrix is diagonal. Algorithm will not work: steady state vector is zero\n");
        fclose(f_partition);
        fclose(f_tp);

        return(86);
    }


    /* writing */
    if (!(f_out = fopen(outname, "w"))) 
    {  
        fprintf(stderr,"GTH block elim. method: Error while opening file %s\n",outname);
        return(31);
    }

    /* Printing Step */
    fprintf(f_out,"#Solution Method: Stationary point probability (Grassman/Heymann block elimnation alg.)\n");
    
    /* pi[0] = size */
    for (i = 1; i <= M->num_col; i++) 
        fprintf(f_out,"%d %.10e\n",i,pi[i]);
        
    fprintf(f_out,"\n#**************************\n");
    
    
    fclose(f_partition);
    fclose(f_tp);
    fclose(f_out);
    
    free((char *)pi);
    free_Matrix(1, P);
    free_Matrix(1, M);
    
    return(0);
}



/******************************************************/
/* Interface to transient probabilities               */
/* Uses the uniformization method                     */
/******************************************************/
int interface_point_probability(basename, precision)
char   *basename;
double precision;
{
   char   mat_type;             /* matrix type                            */
   char   tpname[1024];         /* name of prob. matrix input file        */
   char   init_prob_name[1024]; /* name of initial prob. input file       */
   char   intervals_name[1024]; /* name of intervals def. input file      */
   char   outname[1024];        /* the name of an output file             */
   int    i, n;                 /* temporary variables                    */
   int    num_intervals;        /* number of intervals                    */
   int    *N_min, *N_max;       /* up and low bounds                      */
   double total;                /* sum pis                                */
   double *intervals;           /* interval time values                   */
   double lambda;               /* uniformization rate                    */
   double *pi1;                 /* ptr to init. prob., reused as aux. ptr */
   double **pi2;                /* ptr to transient probabilities         */
   FILE   *f_tp, *f_out;        /* ptr to in/out files                    */
   FILE   *f_intervals;         /* ptr to interval description file       */
   FILE   *f_init_prob;         /* ptr to initial prob. desc. file        */
   Matrix *M;                   /* ptr to input matrix                    */
   Matrix *P;                   /* ptr to probabilities matrix            */
   int    status;


   /* name input files */
   sprintf(tpname, "%s.generator_mtx", basename);
   sprintf(init_prob_name, "%s.init_prob", basename);
   sprintf(intervals_name, "%s.intervals", basename);


   /* reads intervals vector */
   if (! (f_intervals = fopen(intervals_name, "r")) ) 
   {
       fprintf(stderr,"Uniformization method: Error while opening file %s\n", intervals_name);
       return(30);
   }
  

   /* reads initial prob. vector */
   if (! (f_init_prob = fopen(init_prob_name, "r")) ) 
   {
       fprintf(stderr,"Uniformization method: Error while opening file %s\n", init_prob_name);
       return(30);
   }
  

   /* reads rate matrix */
   if (! (f_tp = fopen(tpname, "r")) ) 
   {
       fprintf(stderr,"Uniformization method: Error while opening file %s\n",tpname);
       return(30);
   }
 

   M = get_trans_matrix(f_tp);

   fprintf(stderr,"\nUniformization method: Using %s file. Assuming it is a rate matrix.\n",tpname);

   mat_type = 'Q';
   generate_diagonal(mat_type, M);


#ifdef DEBUG   
   fprintf(stderr, "Uniformization method: Generated diagonal\n");
#endif

   
   /* Uniformizing matrix */
   P = uniformize_matrix(M, &lambda);

   
#ifdef DEBUG
   fprintf(stderr, "Uniformization method: Uniformized matrix\n");
#endif


   /* read initial pi */
   pi1 = get_initial_pi(f_init_prob, P->num_col);

   if (pi1 == NULL)
   {
       fprintf(stderr, "Uniformization method: Invalid initial probability description\n");
       fclose(f_tp);
       fclose(f_init_prob);
       fclose(f_intervals);
       return(55);
   }
   
   
#ifdef DEBUG
   for (i = 0; i < P->num_col; i++)
       printf("Uniformization method: pi1[%d]= %.6e\n", i, pi1[i]);
#endif


   /* read intervals */
   intervals = get_intervals(f_intervals, &num_intervals);
  
   if (intervals == NULL)
   {
       fprintf(stderr, "Uniformization method: Invalid intervals description\n");
       fclose(f_tp);
       fclose(f_init_prob);
       fclose(f_intervals);
       return(59);
   }

#ifdef DEBUG
   for (i = 0; i < num_intervals; i++)
       printf("Uniformization method: Interval[%d]= %.6e\n", i, intervals[i]);
#endif


   /* allocate vectors */
   N_min = (int     *)my_calloc(num_intervals, sizeof(int));
   N_max = (int     *)my_calloc(num_intervals, sizeof(int));
   pi2   = (double **)my_calloc(num_intervals, sizeof(double));
    
   for (n = 0; n < num_intervals; n++) 
       pi2[n] = (double *)my_calloc(P->num_col, sizeof(double));
 
 
   status = uniformization_method(num_intervals, intervals, lambda, precision, 
                                                   N_min, N_max, pi1, pi2, P); 

   free(pi1);
   
   if (status != 0)             /* problems with the solution method */
   {
       fclose(f_tp);
       fclose(f_init_prob);
       fclose(f_intervals);
       
       return(status);
   }


   for (n = 0; n < num_intervals; n++) 
   {

       /* name of output file */
       sprintf(outname, "%s.TS.pp.%.5e", basename, intervals[n]);
       
       /* writing */
       if (!(f_out = fopen(outname, "w"))) 
       {  
           fprintf(stderr,"Uniformization method: Error while opening file %s\n",outname);
           return(31);
       }

       /* print input values */
       fprintf(f_out,"#Solution method: Transient point probability (Uniformization method)\n");
       fprintf(f_out,"#Time = %.5e\n", intervals[n]);
       fprintf(f_out,"#Lambda = %.6e, Precision = %.6e, N_min = %d, N_max = %d\n", lambda, precision, N_min[n], N_max[n]);

       total = 0;
       pi1 = pi2[n];     
       for (i = 0; i < P->num_col; i++) 
       {
           fprintf(f_out,"%d %.10e\n", i+1, pi1[i]);
           total += pi1[i];
       }
       
       fprintf(f_out,"#       --------------------\n");
       fprintf(f_out,"# total = %.10e\n\n", total);
       fprintf(f_out,"#**************************\n");

       fclose(f_out);
       
       /* releasing each pi vector */
       free(pi1);
   }

   
   free_Matrix(1,P);
   free_Matrix(1,M);
   free(pi2);
   free(intervals);
   
   fclose(f_init_prob);
   fclose(f_intervals);
   fclose(f_tp);
   
   return(0);
}


/******************************************************/
/* Interface to Deterministic Event solver            */
/* (non-Markovian models)                             */
/******************************************************/
int interface_det_event(basename, precision,emb_sol,no_max_iterations)
char   *basename;
double precision;
char emb_sol;
double no_max_iterations;

{
   int stop_condition;
   
   
   stop_condition = 0;
   embedded_markov_chain(basename,precision,emb_sol,no_max_iterations,&stop_condition);

   if (stop_condition == MAX_ITERATIONS)
    {
        return (89);
    }

   return(0);
}



/******************************************************/
/* interface expected transient reward                */
/* interface to calculate E[CR(t)/t]                  */
/******************************************************/
int interface_expected_transient_reward(basename, reward_name, precision)
char   *basename;
char   *reward_name;
double precision;

{

 int i,n;                      /* temporary variables */
 FILE *f_tp, *f_out;           /* ptr to in/out files */
 FILE *f_rwd;                  /* ptr to reward file */
 FILE *f_intervals;            /* ptr to interval description file       */
 FILE *f_init_prob;            /* ptr to initial prob. desc. file        */
 char mat_type;                /* Matrix type */
 char outname[1024];           /* the name of an output file */
 char inp_mtx_name[1024];      /* the name of input matrix file */
 char rwd_name[1024];           /* rate reward filename */
 char intervals_name[1024];    /* name of intervals def. input file      */
 char init_prob_name[1024];    /* name of initial probabilities def.input file*/
 double LAMBDA;                /* uniformization const */
 Matrix *M,*P;                 /* ptr to rate and prob matrices */
 int num_t_points;             /* num of intervals to evaluate */
 double epsilon;               /* error upperbound in summation  */
 double *time_points;          /* ptr to t values */
 double *exp_rew;              /* ptr to expected reward */
 double *pi1;                   /* ptr to initial probabilities */
 int    number_of_states;      /* number of states */
 double *reward_vec;           /* reward vector */
 double lamb_t;                /* product LAMBDA * t */
 double *Min_poisson_val;      /* Poisson evaluated at Nmin for a give time */
 int    *N_min;               /* ptr to low bound (poisson) for each time */
 int    *N_max; /* ptr to up bound (truncation point for posiion) for each time */
 
   
 epsilon = precision;

 sprintf(inp_mtx_name,"%s.generator_mtx",basename);
 sprintf(init_prob_name, "%s.init_prob", basename);
 sprintf(intervals_name, "%s.intervals", basename);
 sprintf(rwd_name,"%s.rate_reward.%s",basename,reward_name);
 /* name of output file */
 sprintf(outname, "%s.TS.exptr", basename);

 /* Opens basename.generator_mtx */
 if (! (f_tp = fopen(inp_mtx_name,"r")) ) {
    printf("Expected transient reward method: Error while opening file %s\n",inp_mtx_name);
    return(30);
 }

 /* Read matrix */
 M  = get_trans_matrix(f_tp);
 number_of_states = M->num_col;

 fprintf(stderr,"\nExpected transient reward methods: Using %s file. Assuming it is a rate matrix.\n",inp_mtx_name);

 mat_type = 'Q';
 generate_diagonal(mat_type, M);
 P = uniformize_matrix(M,&LAMBDA);

 /* reads vector with time points */
 if (! (f_intervals = fopen(intervals_name, "r")) )
 {
    fprintf(stderr,"Expected transient reward method: Error while opening file %s\n",
intervals_name);
    return(30);
 }
 time_points = get_intervals(f_intervals, &num_t_points);

 if (time_points == NULL)
 {
    fprintf(stderr, "Expected transient reward method: Invalid intervals description\n");
    return(59);
 }

#ifdef DEBUG
   for (i = 0; i < num_t_points; i++)
   {
      printf("Expected transient reward method: num_t_points = %d, t[%d] = %.6e\n", num_t_points, i, time_points[i]);
   }
#endif 

 /* read initial prob. vector, pi1 */
 if (! (f_init_prob = fopen(init_prob_name, "r")) )
 {
    fprintf(stderr,"Operational time methods: Error while opening file %s\n", init_prob_name);
    return(30);
 }
 pi1 = get_initial_pi(f_init_prob, number_of_states);

 if (pi1 == NULL)
 {
    fprintf(stderr, "Expected transient reward method: Invalid initial probability description\n");
    return(55);
 }

#ifdef DEBUG
   for (i = 0; i <  number_of_states; i++) {
      printf("\nExpected transient reward method: pi1[%d] = %f, \n", i, pi1[i]);
   }
#endif

 /* Read rate reward file */
 /* Opens rate reward file */
 if (! (f_rwd = fopen(rwd_name,"r")) ) {
    printf("Expected transient reward time methods: Error while opening file %s\n",rwd_name);
    return(30);
 }
 reward_vec = get_rewards(f_rwd, number_of_states);

 if (reward_vec == NULL)
 {
    fprintf(stderr, "Expected transient reward method: Invalid rewards description\n");
    return(54);
 }

 /* allocates vectors */
 N_min  = (int    *) my_calloc(num_t_points * sizeof(int), sizeof(int));
 N_max  = (int    *) my_calloc(num_t_points * sizeof(int), sizeof(int));
 Min_poisson_val = (double *) my_calloc(num_t_points, sizeof(double));

 for (i = 0; i < num_t_points; i++) {
    lamb_t = LAMBDA * time_points[i];
    lo_bound(&(N_min[i]), &(Min_poisson_val[i]), lamb_t);
    N_max[i] = partial_poisson_sum(lamb_t, epsilon);
 }
#ifdef DEBUG
 for (i = 0; i < num_t_points; i++) {
    printf("Expected transient reward method: N_min[%d] = %d, N_max[%d] = %d, Min_poisson_val[%d] = %f \n",
       i, N_min[i], i, N_max[i], i, Min_poisson_val[i]);
 }
#endif

 /*evaluates measure */
 exp_rew = expected_trans_reward( pi1, P, LAMBDA, num_t_points, time_points,
                                  reward_vec, N_min, N_max, Min_poisson_val );
   
 /* write results */
 if (!(f_out = fopen(outname, "w"))) 
 {
    fprintf(stderr,"Expected transient reward method: Error while opening file %s\n",outname);
    return(31);
 }

 /* print input values */
 fprintf(f_out,"#Solution method: Expected Transient Reward (Uniformization)\n");
 fprintf(f_out,"#Lambda = %.6e, Precision = %.6e \n\n", LAMBDA, precision);

 fprintf(f_out,"#Time (t)             E[ECR(t)] \n");
 for (n = 0; n < num_t_points; n++) {

     fprintf(f_out,"#Time = %.5e, N_min = %d, N_max = %d\n", time_points[n], N_min[n], N_max[n]);

     fprintf(f_out,"%.5e   %.10e\n", time_points[n], exp_rew[n]);
       
 }

 fprintf(f_out,"#**************************\n");

 fclose(f_tp);
 fclose(f_out);
 fclose(f_rwd);
 fclose(f_intervals);
 fclose(f_init_prob);

 free((Matrix *)M);
 free((Matrix *)P);
 free((double *)pi1);

 free((int *) N_min);
 free((int *) N_max);
 free((double *)Min_poisson_val);

 free((double *)time_points);
 free((double *)reward_vec);
 free((double *)exp_rew);

 return(0);

}


/************************************************/
/*  interface to Rate Cumulative Reward         */
/*  based in approximation transient method     */
/************************************************/

int interface_approximation_cumulative_reward_direct(basename, R, name_of_reward,measure_option)

char *basename;
int R;                /*total stages in approximation method*/
char *name_of_reward;
int measure_option;  /*1-state reward; 2-set reward*/
{

   
   char gen_matrix_name[1024];   /* name of file with generator mtx.       */
   char reward_name[1024];       /* name of file with reward               */
   char partition_name[1024];    /* name of file with partitions           */
   char init_prob_name[1024];    /* name of file with init_prob            */
   char dest_states_name[1024];  /* name of file with destination_states   */
   char intervals_name[1024];    /* name of file with observation intervals*/
   char out_name[1024];          /* name of file with results              */
    
   FILE *f_gen_matrix;           /* Generator Matrix file descriptor       */   
   FILE*f_reward;                /*Reward rates file descriptor            */    
   FILE *f_partition;            /*Partition file descriptor               */  
   FILE *f_out;                  /*Result file descriptor                  */    
   FILE *f_dest;                 /*Destination states file descriptor      */  
   FILE *f_init_prob;            /*Initial probability vector file         */
                                 /* descriptor                             */   
   FILE *f_intervals;            /*Interval vector file descriptor         */    
   
   Matrix *Q;                    /*Generator Matrix                        */
   Matrix *P;                    /*Probability Transition Matrix           */
   int number_of_states;         /*Total number of states in the model     */
   int num_t_points;             /* num of intervals to evaluate           */
   double uniformization_rate;   /*Uniformization rate                     */
   double final_time_observation;/*Last time of interval                   */
   int *dvec;                    /*vector with destination states          */
   int *pvec;                    /*part vec: no. of partition,size of each */
   int *rvec;                    /*vector 1j, with 1's or 0's              */
   double *initial_pi;           /*Initial probability vector              */
   double *vreward;              /*vector with the reward rates            */
   double *Cj_set;
   double **Cj;
   int status;                   /*auxiliary variable                      */
   int count;                    /*auxiliary variable                      */
   int i;
   double time;
   
   
   f_dest = NULL;
   rvec   = NULL; 
   Cj_set = NULL; 
   Cj     = NULL; 
   
   sprintf(gen_matrix_name,"%s.generator_mtx",basename);
   sprintf(init_prob_name, "%s.init_prob", basename);
   sprintf(intervals_name, "%s.int_rewd", basename);
   sprintf(reward_name,"%s.rate_reward.%s",basename,name_of_reward);
   sprintf(partition_name,"%s.partition",basename);
   
   /*Open files*/   
   if (!(f_gen_matrix = fopen(gen_matrix_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward Error while opening file%s.\n",gen_matrix_name); 	
         return (30);   
      }	 

   if(!(f_reward = fopen(reward_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",reward_name);
	 return (30);
      }	 

   if(!(f_partition = fopen(partition_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",partition_name);
	 return (30);
      }	 


   if(measure_option == SET_REWARD)
      {
         sprintf(dest_states_name,"%s.rate_reward.GlobalReward.states",basename);
         if(!(f_dest = fopen(dest_states_name,"r")))
            {
               fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",dest_states_name);
	       return (30);
            }
      }	     
	    	 
   
   if(!(f_intervals = fopen(intervals_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",intervals_name);
	 return(30);
      }	 
   
   if(!(f_init_prob = fopen(init_prob_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",init_prob_name);
	 return (30);
      }	    	
    
   /*get generator matrix from file*/ 
   status = fscanf(f_gen_matrix,"%d",&number_of_states);
   if (status != 1)
      {
         fprintf(stderr,".generator_mtx : Invalide file format.\n");
	 return (32);
      }	    
   if ((Q = get_trans_matrix2(f_gen_matrix,number_of_states)) == NULL)
      {
	 fprintf(stderr,"Error in Generator Matrix\n");
	 return(32);
      }

   fprintf(stderr,"\n Rate Cumulative reward method (DIRECT): Using %s file. Assuming it is a rate matrix.\n",gen_matrix_name);

  /* set diagonal elements of Q to fit format*/
  generate_diagonal('Q', Q);
  /*unifor. the .generator_mtx*/  
  P = uniformize_matrix(Q,&uniformization_rate);
	

  
  /*get total number of subintervals and total time of observation*/
  status = fscanf(f_intervals,"%d",&num_t_points);
  if (status != 1)
     {
        /* perror("fscanf"); */
        fprintf(stderr,"Intervals Description : Invalide file format.\n");
        return (32);
     }
  status = fscanf(f_intervals,"%lf",&final_time_observation);
  if (status != 1)
     {
        /* perror("fscanf"); */
        fprintf(stderr,"Intervals Description : Invalide file format.\n");
        return (32);
     }     
       
  
 /*get all destination states*/
  if(measure_option == SET_REWARD) 
    {
        dvec = get_destination_states(f_dest);
        fclose(f_dest);
    }    
  else
    {
       dvec = (int *)my_calloc((number_of_states + 1), sizeof(int)); 
       dvec[0]= number_of_states;
       for (count = 1; count <= number_of_states; count ++)
           dvec[count] = count;
    }	   
	   

 /*get all partitions*/
  pvec = get_partitions(f_partition,number_of_states); 
  
 /*get the reward rate values*/
  vreward = get_rewards_2(f_reward,number_of_states);   

 /*if measure option == SET_REWARD, allocates the 1 vector with 1 in dest states*/
  if((measure_option == SET_REWARD))
     {
        /*allocate vector 1j*/	  
         rvec = (int *)my_calloc((number_of_states + 1),sizeof(int));
         rvec[0] = number_of_states;
	 for(count = 1; count<= dvec[0]; count ++)
	   {
	       i = dvec[count];
	       rvec[i]=1;
	   }
     }
     

   
 /*get the initial probability vector */
  initial_pi = get_initial_pi_2(f_init_prob,number_of_states);
  fclose(f_init_prob);
    
        
 /*calculates the cumulative rate reward*/
 if (measure_option == SET_REWARD)
		     		      
    Cj_set = ESPA_dir_set_reward(P,R,final_time_observation,num_t_points,initial_pi,pvec,
		            rvec,vreward,uniformization_rate);
		 
 /* Solving the cumulative reward- Destination States*/
 if (measure_option == STATE_REWARD)
    Cj = ESPA_dir_reward(P,R,final_time_observation,num_t_points,initial_pi,pvec,
		   dvec,vreward,uniformization_rate);	      


/*Creating the result file*/	    
 switch (measure_option)
   {
      case (SET_REWARD):

			    sprintf(out_name,"%s.TS.DIRECT.Cumulative_SET_Reward",basename);
		            if ((f_out = fopen(out_name,"w+")) == NULL)
		               {
			           fprintf(stderr,"%s","Error in output file creation\n");
			           return(31);
			        }	
			   fprintf(f_out,"#Solution Method:    Cumulative Rate Reward (Approximation Method).\n");
			   fprintf(f_out,"#Uniformization Rate:%.6e  Total Number of Erlang Stages: %d \n",uniformization_rate,R);
			   fprintf(f_out,"#Reward:             %s \n",name_of_reward);
			   fprintf(f_out,"#Cumulative Set Reward given the initial probability\n");
			   fprintf(f_out,"#Time                       Cumulative Reward\n"); 	  
			   fprintf(f_out,"#--------------------------------------------\n");
			   /*loop through all instants*/
			   for (count = 1;count<=num_t_points;count++)
		             fprintf(f_out,"%.10g                        %10e\n",count*final_time_observation/num_t_points,Cj_set[count]);
	                   fclose(f_out);
		    break;
	case (STATE_REWARD):	      		     
		          for(count = 1;count<=num_t_points;count++)
			     {
				  time = count*(final_time_observation/num_t_points);
				  sprintf(out_name,"%s.TS.DIRECT.Cumulative_STATE_Reward.%10e",basename,count*final_time_observation/num_t_points);
				    if ((f_out = fopen(out_name,"w+")) == NULL)
				       {
					   fprintf(stderr,"%s","Error in output file creation\n");
					   return(31);
					}
				
			       fprintf(f_out,"#Solution Method:    Cumulative Rate Reward (Approximation Method).\n");
				   fprintf(f_out,"#Uniformization Rate:%.6e  Total Number of Erlang Stages: %d \n",uniformization_rate,R);
				   fprintf(f_out,"#Reward:             %s \n",name_of_reward);
				   fprintf(f_out,"#Cumulative State Reward given the initial probability \n");
				   fprintf(f_out,"#Time:%f\n",time);
				   fprintf(f_out,"# Destination State                  Cumulative Reward \n"); 	  
				   fprintf(f_out,"#--------------------------------------------------------- \n");
				   for (i=1; i <= dvec[0] ; i++)
			              fprintf(f_out,"  %d                                   %.10e\n",dvec[i], Cj[count][i]);		
			           fclose(f_out);
			     }
		 break;
   }
 /* Closing all files */ 
  fclose(f_gen_matrix);
  fclose(f_reward);
  fclose(f_partition);
  fclose(f_intervals);




/* rel. mem. alloc.*/
  if (measure_option == STATE_REWARD)
	{
	   for (count = 0; count <=  num_t_points ; count++)
		/* release memory allocated for Cj[count]	*/
		free(Cj[count]);	
	   free(Cj);	/* free mem. alloc. for Cj	*/
	}
			
   else	/* measure option == SET REWARD	*/
	free(Cj_set);	/* free memory alloc for Cj_set	*/
		
	
  
/* rel. mem. alloc.*/
  free(pvec);
  free(vreward);
  free(dvec);
  free(initial_pi); 


  if (measure_option == SET_REWARD)
     free(rvec);
 
  return(0);

}     
    		 	     	       
/************************************************/
/*  interface to Rate Cumulative Reward         */
/*  based in approximation transient method     */
/************************************************/
int interface_approximation_cumulative_reward_iterative(basename, name_of_reward,measure_option,precision,solution_option,no_max_iterations)
char *basename;
char *name_of_reward;
int measure_option;         /*1-state reward; 2-set reward*/
double precision;           /*precision of solution*/
int solution_option;        /*1- SOR_METHOD; 2 - GAUSS_METHOD; 3 - JACOBI_METHOD; 4 POWER_METHOD*/
double no_max_iterations;   /*stop condition*/

{
   char gen_matrix_name[1024];   /* name of file with gen. mtx.              */
   char reward_name[1024];       /* name of file with reward                 */
   char init_prob_name[1024];    /* name of file with init_prob              */
   char dest_states_name[1024];  /* name of file with destination_states     */
   char intervals_name[1024];    /* name of file with observation interval   */
   char out_name[1024];          /* name of file with results                */
    
   FILE *f_gen_matrix;           /*Generator Matrix file descriptor          */   
   FILE *f_reward;               /*Reward rates file descriptor              */
   FILE *f_out;                  /*Result file descriptor                    */
   FILE *f_dest;                 /*Destination states file descriptor        */
   FILE *f_init_prob;            /*Initial probability vector file descriptor*/
   FILE *f_intervals;            /*Interval vector file descriptor           */
   
   Matrix *Q;                    /*Generator Matrix                          */
   Matrix *P;                    /*Probability Transition Matrix             */
   int number_of_states;         /*Total number of states in the model       */
   int num_t_points;             /* num of intervals to evaluate             */
   double uniformization_rate;   /*Uniformization rate                       */
   int *dvec;                    /*vector with destination states            */
   int *rvec;                    /*vector 1j, with 1's or 0's                */
   int set_of_intervals;
   double *intervals;            /*vector wiht intervals                     */
   double *initial_pi;           /*Initial probability vector                */
   double *vreward;              /*vector with the reward rates              */
   double *Cj_set;
   double **Cj;
   int status;                   /*auxiliary variable                        */
   int count;                    /*auxiliary variable                        */
   int i;
   int stop_condition;          /*stop_condition 0 - Max_Iterations 1- Conv  */
   info_intervals *set_of_R;    /*intervals and number of Erlang stages      */


   
   f_dest = NULL;
   rvec   = NULL; 
   Cj_set = NULL; 
   Cj     = NULL; 
   

   sprintf(gen_matrix_name,"%s.generator_mtx",basename);
   sprintf(init_prob_name, "%s.init_prob", basename);
   sprintf(intervals_name, "%s.int_rewi", basename);
   sprintf(reward_name,"%s.rate_reward.%s",basename,name_of_reward);


   /*Open files*/   
   if (!(f_gen_matrix = fopen(gen_matrix_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward:Error while opening file%s.\n",gen_matrix_name); 	
         return (30); 
      }	 

   if(!(f_reward = fopen(reward_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",reward_name);
	 return (30);
      }	 
   if(measure_option == SET_REWARD)
      {
         sprintf(dest_states_name,"%s.rate_reward.GlobalReward.states",basename);
         if(!(f_dest = fopen(dest_states_name,"r")))
            {
               fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",dest_states_name);
	       return (30);
            }
      }	     
   
   if(!(f_intervals = fopen(intervals_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",intervals_name);
	 return(30);
      }	 
   
   if(!(f_init_prob = fopen(init_prob_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",init_prob_name);
	 return (30);
      }
    
   /*get generator matrix from file*/ 
   status = fscanf(f_gen_matrix,"%d",&number_of_states);
   if (status != 1)
      {
         fprintf(stderr,"generator_mtx : Invalide file format.\n");
	 return (32);
      }	    
   if ((Q = get_trans_matrix2(f_gen_matrix,number_of_states)) == NULL)
      {
	 fprintf(stderr,"Error in Generator Matrix\n");
	 return(32);
      }
 
   fprintf(stderr,"\n Rate Cumulative reward method (ITERATIVE): Using %s file. Assuming it is a rate matrix.\n",gen_matrix_name);

  /* set diagonal elements of Q to fit format*/
   generate_diagonal('Q', Q);
  /*unifor. the .generator_mtx*/
   P = uniformize_matrix(Q,&uniformization_rate);
 

 /*get all subintervals*/
   status = fscanf (f_intervals, "%d", &num_t_points);
   if (status != 1)
     {
        fprintf(stderr,"int_rew : Invalide file format.\n");
        return (32);
     }	
 
   status = fscanf (f_intervals,"%d", &set_of_intervals);
   if (status != 1)
     {
        fprintf(stderr,"int_rew : Invalide file format.\n");
        return (32);
     } 


 /* fill struct with information about subset*/

   set_of_R  = (info_intervals*)my_calloc((set_of_intervals),sizeof(info_intervals));
   intervals = get_intervals_R(f_intervals,num_t_points,set_of_intervals,set_of_R);

 
 /*get all destination states*/
  if(measure_option == SET_REWARD) 
    {
       dvec = get_destination_states(f_dest);
       fclose(f_dest);
    }    
  else
   {
       dvec = (int *)my_calloc((number_of_states + 1), sizeof(int)); 
       dvec[0]= number_of_states;
       for (count = 1; count <= number_of_states; count ++)
           dvec[count] = count;
   }	   
	   
   
 /*get the reward rate values*/
  vreward = get_rewards_2(f_reward,number_of_states);   

 /*if measure option == SET_REWARD, allocates the 1 vector with 1 in dest states*/
 if((measure_option == SET_REWARD))
    {
        /*allocate vector 1j*/	  
        rvec = (int *)my_calloc((number_of_states + 1),sizeof(int));
        rvec[0] = number_of_states;
	for(count = 1; count<= dvec[0]; count ++)
	  {
	      i = dvec[count];
	      rvec[i]=1;
	   }
     }
     	   
 /*get the initial probability vector */
 initial_pi = get_initial_pi_2(f_init_prob,number_of_states);
 fclose(f_init_prob);
        
 /*calculates the cumulative rate reward - Iterative Technique*/

 switch (measure_option)
   {
     case (SET_REWARD):
                        Cj_set = ESPA_itr_set_reward(P,set_of_R,num_t_points,set_of_intervals,
			initial_pi,vreward,rvec,uniformization_rate,solution_option,precision,no_max_iterations,&stop_condition);
                        break;
 
     case (STATE_REWARD):
                        Cj = ESPA_itr_reward(P,set_of_R,num_t_points,set_of_intervals,initial_pi,
		         dvec,vreward,uniformization_rate,solution_option,precision,no_max_iterations,&stop_condition);
                        break;
   }        
 
 
 /*printting the result */
 switch (measure_option)
   {
      case (SET_REWARD):
		    sprintf(out_name,"%s.TS.ITERATIVE.Cumulative_SET_Reward",basename);
		            if ((f_out = fopen(out_name,"w+")) == NULL)
		               {
			           fprintf(stderr,"%s","Error in output file creation\n");
			           return(31);
			        }	
			   fprintf(f_out,"#Solution Method:    Cumulative Rate Reward (Approximation Method).\n");
			   fprintf(f_out,"#Uniformization Rate:%.6e  \n",uniformization_rate);
			   fprintf(f_out,"#Reward:             %s \n",name_of_reward);
			   fprintf(f_out,"#Cumulative Set Reward given the initial probability\n");
			   fprintf(f_out,"#Time                       Cumulative Reward\n"); 	  
			   fprintf(f_out,"#--------------------------------------------\n");
			   /*loop through all instants*/
			   for (count = 1;count<=num_t_points;count++)
		             fprintf(f_out,"%.8f\t\t\t%10e\n",intervals[count],Cj_set[count]);
	                   fclose(f_out);
		    break;
	case (STATE_REWARD):	      		     
		          for(count = 1;count<=num_t_points;count++)
			     {
				  sprintf(out_name,"%s.TS.ITERATIVE.Cumulative_STATE_Reward.%.10e",basename,intervals[count]);
				    if ((f_out = fopen(out_name,"w+")) == NULL)
				       {
					   fprintf(stderr,"%s","Error in output file creation\n");
					   return(31);
					}
				
			       fprintf(f_out,"#Solution Method:    Cumulative Rate Reward (Approximation Method).\n");
				   fprintf(f_out,"#Uniformization Rate:%.6e  \n",uniformization_rate);
				   fprintf(f_out,"#Reward:             %s \n",name_of_reward);
				   fprintf(f_out,"#Cumulative State Reward given the initial probability \n");
				   fprintf(f_out,"# Destination State                  Cumulative Reward \n"); 	  
				   fprintf(f_out,"#--------------------------------------------------------- \n");
				   for (i=1; i <= dvec[0] ; i++)
			              fprintf(f_out,"  %d                                   %.10e\n",dvec[i], Cj[count][i]);		
			           fclose(f_out);
			     }
		 break;
   }
      
       	   	      

 /* Closing all files */ 
  fclose(f_gen_matrix);
  fclose(f_reward);
  fclose(f_intervals);
  
/* rel. mem. alloc.*/
  if (measure_option == STATE_REWARD)
	{
	   for (count = 0; count <=  num_t_points ; count++)
	    /* release memory allocated for Cj[count]*/
	       free(Cj[count]);	
	   free(Cj);/* free mem. alloc. for Cj*/
        }
   else	/* measure option == SET REWARD	*/
	free(Cj_set);/* free memory alloc for Cj_set*/
	
  free(vreward);
  free(dvec);
  free(initial_pi);
 
  if (measure_option == SET_REWARD)
     free(rvec);

  if (stop_condition == MAX_ITERATIONS)
     return (89);
  else
     return (0);

 } 


/******************************************************/
/* Interface to transient probabilities               */
/* Uses the Efficient transient State Probability     */
/* Approximation method - Direct Techinique           */
/******************************************************/
int interface_espa_direct(basename, R, measure_option,state_var)
char *basename;
int R;                /*total stages in approximation method                      */
int measure_option;   /*1-state probability; 2-set probability; 3 - expected value*/
char *state_var;      /*state var - expected value                                */

{

   char gen_matrix_name[1024];   /* name of file with gen. mtx.              */
   char init_prob_name[1024];    /* name of file with init_prob              */
   char dest_states_name[1024];  /* name of file with destination_states     */
   char intervals_name[1024];    /* name of file with observation intervals  */
   char partition_name[1024];    /* name of file with partitions             */
   char out_name[1024];          /* name of file with results                */
   char expected_name[1024];     /* name of file with state variable values  */

   FILE *f_gen_matrix;           /*Generator Matrix file descriptor          */
   FILE *f_out;                  /*Result file descriptor                    */
   FILE *f_dest;                 /*Destination states file descriptor        */
   FILE *f_init_prob;            /*Initial probability vector file descriptor*/
   FILE *f_intervals;            /*Interval vector file descriptor           */
   FILE *f_partition;            /*Partition file descriptor                 */
   FILE *f_expected;             /*Values of state variable file descriptor  */

   Matrix *Q;                    /*Generator Matrix                          */
   int num_t_points;             /* num of intervals to evaluate             */
   double final_time_observation;/*Last time of interval                     */
   int *dvec;                    /*vector with destination states            */
   int *rvec;                    /*vector 1j, with 1's or 0's                */
   int *pvec;                    /*partit vec: no. of partition,size of each */
   double *initial_pi;           /*Initial probability vector                */
   int number_of_states;         /*Total number of states in the model       */
   int status;                   /*Auxiliary variable                        */
   int count;                  /*Auxiliary variables                       */
   char line[200];               /*Auxiliary variable                        */
   double **pi;                  /*vec w/n_t ptrs to vectors pi(t)           */
   double *pi_set;               /*vec w/n_t probabilities pi(t)             */
   double time;
   double uniform_rate;         /*uniformization rate of the model           */
   int i;                       /*auxiliary variable                         */
   int number_of_states_true;
   int temp;
   double value;              

   f_dest     = NULL;
   f_expected = NULL;
   dvec       = NULL;
   rvec       = NULL;
   pi         = NULL;
   pi_set     = NULL;


   sprintf(gen_matrix_name,"%s.generator_mtx",basename);
   sprintf(intervals_name, "%s.int_direct", basename);
   sprintf(partition_name,"%s.partition",basename);

     /*Open files*/


   if (!(f_gen_matrix = fopen(gen_matrix_name,"r")))
      {
         fprintf(stderr,"Efficient transient Probability Approximation method (Direct): Error while opening file%s.\n",gen_matrix_name); 	
         return (30);
      }

   if(!(f_intervals = fopen(intervals_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",intervals_name);
	 return(30);
      }	 

   if(!(f_partition = fopen(partition_name,"r")))
      {
         fprintf(stderr," Efficient transient Probability Approximation method (Direct):Error while opening file %s.\n",partition_name);
	 return (30);
      }	

   if (measure_option == SET_PROB)
      {
         sprintf(dest_states_name,"%s.rate_reward.GlobalReward.states",basename);
	 if(!(f_dest = fopen(dest_states_name,"r")))
            {
               fprintf(stderr,"Efficient transient Probability Approximation method (Direct) :Error while opening file %s.\n",intervals_name);
	       return(30);
            }
      }
      
   if(measure_option == EXPECTED_VAL)
      {
         sprintf(expected_name,"%s.rate_reward.%s",basename,state_var);
	 if(!(f_expected = fopen(expected_name,"r")))
            {
               fprintf(stderr,"Efficient transient Probability Approximation method (Direct) :Error while opening file %s.\n",expected_name);
	       return(30);
            }
      }
            	    	
    sprintf(init_prob_name, "%s.init_prob", basename);
    if(!(f_init_prob = fopen(init_prob_name,"r")))
      {
         fprintf(stderr,"Efficient transient Probability Approximation method (Direct) :Error while opening file %s.\n",init_prob_name);
	 return (30);
      }
	    	

   /*get generator matrix from file*/
   status = fscanf(f_gen_matrix,"%d",&number_of_states);
   if (status != 1)
      {
         fprintf(stderr,".generator_mtx : Invalide file format.\n");
	 return (32);
      }	
   if ((Q = get_trans_matrix2(f_gen_matrix,number_of_states)) == NULL)
      {
	 fprintf(stderr,"Error in Generator Matrix\n");
	 return (32);
      }

      fprintf(stderr,"\n Efficient transient Probability Approximation method (Direct): Using %s file. Assuming it is a rate matrix.\n",gen_matrix_name);

  /*get total number of subintervals and total time of observation*/
   status = fscanf(f_intervals,"%d",&num_t_points);
   if (status != 1)
      {
        /* perror("fscanf"); */
         fprintf(stderr,"Intervals Description : Invalide file format.\n");
         return (32);
      }
 
   status = fscanf(f_intervals,"%lf",&final_time_observation);
   if (status != 1)
      {
        /* perror("fscanf"); */
         fprintf(stderr,"Intervals Description : Invalide file format.\n");
         return (32);
      }        

   /*get all partitions*/
   pvec = get_partitions(f_partition,number_of_states); 

   
   /*get destination states vector*/
   switch (measure_option)
     {
        case(SET_PROB):
                       dvec = get_destination_states(f_dest);
                       fclose(f_dest);
        break;	       
       
        case(STATE_PROB):
                      dvec = (int *)my_calloc((number_of_states + 1), sizeof(int)); 
                      dvec[0]= number_of_states;
                      for (count = 1; count <= number_of_states; count ++)
                        dvec[count] = count;
	break;
	
	case(EXPECTED_VAL):
	break;
			
     }	   
  
        
   /*get initial probability vector*/
   initial_pi = get_initial_pi_2(f_init_prob,number_of_states);    
   fclose(f_init_prob);

   if (measure_option == SET_PROB)
     {
        /*allocate vector 1j*/	
        rvec = (int *)my_calloc((number_of_states + 1),sizeof(int));
        rvec[0] = number_of_states;     
        /* fill "value-by-state vector" with value of 1 in states
        belonging to the set and 0 to the states outside
        the set	*/
        for (count = dvec[1], i = 1; i <= dvec[0]; count = dvec[++i])
	    rvec[count] = 1;
     }

  if (measure_option == EXPECTED_VAL)
     {
       /* measure option == EXPECTED VALue	*/
	/* get value-by-state vector from input file*/
         rvec = (int *)my_calloc((number_of_states + 1),sizeof(int));
         rvec[0] = number_of_states;
	 fgets (line, 200, f_expected);
	 number_of_states_true = 0;
	 while (!feof(f_expected))
        {	        
            number_of_states_true += 1;
            fgets(line, 200,f_expected);
        }    
     rewind(f_expected);  
     fgets (line, 200, f_expected);  
     number_of_states_true -= 1; 
     for (count = 1; count <= number_of_states_true; count++)
	   if ((status = fscanf(f_expected, "%d  %lf", &temp, &value)) != 2)
	      {
		   fprintf(stderr, "Efficient transient Probability Approximation method (Iterative): Error in %s\n", expected_name);
	           return(32);
       	      }
	   else
	       rvec[temp] = value;   
         fclose(f_expected);     
     }



 /*calculating the measure*/
 
  if(measure_option == STATE_PROB)
        pi = ESPA_dir(Q,R,final_time_observation,num_t_points,initial_pi,pvec,dvec,&uniform_rate);
  else /*measure option == SET_PROBability or EXPECTED_VALue*/
        pi_set = ESPA_dir_set(Q,R,final_time_observation,num_t_points,initial_pi,pvec,rvec,&uniform_rate);
 	
 /*printting the result */
 switch (measure_option)
   {
      case (SET_REWARD):
			    sprintf(out_name,"%s.TS.espa_dir.SET_PROB",basename);
		            if ((f_out = fopen(out_name,"w+")) == NULL)
		               {
			           fprintf(stderr,"%s","Error in output file creation\n");
			           return(31);
			        }	
			   fprintf(f_out,"#Solution Method:Efficient transient State Probability(Approximation Method).\n");
			   fprintf(f_out,"#Uniformization Rate:%.6e\n",uniform_rate);
               fprintf(f_out,"#Total No of Erlang Stages in each interval:%d \n",R);
			   fprintf(f_out,"#Set Probability given the initial probability\n");
			   fprintf(f_out,"#Time                       Probability\n"); 	  
			   fprintf(f_out,"#--------------------------------------------\n");
			   /*loop through all instants*/
			   for (count = 1;count<=num_t_points;count++)
		             fprintf(f_out,"%.10g                        %10e\n",count*final_time_observation/num_t_points,pi_set[count]);
	                   fclose(f_out);
		           break;
	case (STATE_REWARD):	      		     
		          for(count = 1;count<=num_t_points;count++)
			     {
				  time = count*(final_time_observation/num_t_points);
				  sprintf(out_name,"%s.TS.espa_dir.STATE_PROB.%10e",basename,count*final_time_observation/num_t_points);
				    if ((f_out = fopen(out_name,"w+")) == NULL)
				       {
					   fprintf(stderr,"%s","Error in output file creation\n");
					   return(31);
					}

			       fprintf(f_out,"#Solution Method:Efficient transient State Probability (Approximation Method).\n");
				   fprintf(f_out,"#Uniformization Rate:%.6e\n",uniform_rate);
                   fprintf(f_out,"#Total No of Erlang Stages in each interval: %d \n",R);
				   fprintf(f_out,"#State Probability given the initial probability \n");
				   fprintf(f_out,"#Time:%f\n",time);
				   fprintf(f_out,"# Destination State                  Probability \n");
				   fprintf(f_out,"#--------------------------------------------------------- \n");
				   for (i=1; i <= dvec[0] ; i++)
			              fprintf(f_out,"  %d                                   %.10e\n",dvec[i], pi[count][i]);
			           fclose(f_out);
			     }
		         break;
        case (EXPECTED_VAL):
			    
			    sprintf(out_name,"%s.TS.espa_dir.EXPECTED_VAL",basename);
		            if ((f_out = fopen(out_name,"w+")) == NULL)
		               {
			           fprintf(stderr,"%s","Error in output file creation\n");
			           return(31);
			        }	
			   fprintf(f_out,"#Solution Method:Efficient transient State Probability(Approximation Method).\n");
			   fprintf(f_out,"#Uniformization Rate:%.6e\n",uniform_rate);
               fprintf(f_out,"#Total Number of Erlang Stages in each interval:%d \n",R);
			   fprintf(f_out,"#Expected Value given the initial probability\n");
			   fprintf(f_out,"#Time                       Expected\n"); 	  
			   fprintf(f_out,"#--------------------------------------------\n");
			   /*loop through all instants*/
			   for (count = 1;count<=num_t_points;count++)
		             fprintf(f_out,"%.10g                        %10e\n",count*final_time_observation/num_t_points,pi_set[count]);
	                   fclose(f_out);
		    break;
    }
      
 /* Closing all files */ 
  fclose(f_gen_matrix);
  fclose(f_intervals);
  
/* rel. mem. alloc.*/
  if (measure_option == STATE_PROB)
	{
	   for (count = 0; count <=  num_t_points ; count++)
		/* release memory allocated for pi[count]*/
		free(pi[count]);	
	   free(pi);	/* free mem. alloc. for pi*/
	}
  else	/* measure option == SET_PROB or EXPECTED_VALue*/
	free(pi_set);	/* free memory alloc for pi_set	*/


 if ((measure_option == STATE_PROB)||(measure_option == SET_PROB))
     free(dvec);

 if (measure_option != STATE_PROB)
   free(rvec);


 free(initial_pi);     

 return(0);
 
 } 
	
/******************************************************/
/* Interface to transient probabilities               */
/* Uses the Efficient transient State Probability     */
/* Approximation method - Iterative Technique         */
/******************************************************/
int interface_espa_iterative (basename,measure_option,state_var,
precision,solution_option,no_max_iterations)

char *basename;
int measure_option;  /*1-state reward; 2-set reward     */
char *state_var;     /*state var - expected value       */
double precision;    /*precision of solution            */
int solution_option; /*1- SOR_METHOD; 2 - GAUSS_METHOD; */
                     /* 3 - JACOBI_METHOD; 4 POWER_METHOD*/
double no_max_iterations; /*stop condition              */

{  
   char gen_matrix_name[1024];   /* name of file with gen. mtx.              */
   char init_prob_name[1024];    /* name of file with init_prob              */
   char dest_states_name[1024];  /* name of file with destination_states     */
   char intervals_name[1024];    /* name of file with observation intervals  */
   char out_name[1024];          /* name of file with results                */
   char expected_name[1024];     /* name of file with state variable values  */


   FILE *f_gen_matrix;           /*Generator Matrix file descriptor          */   
   FILE *f_out;                  /*Result file descriptor                    */
   FILE *f_dest;                 /*Destination states file descriptor        */
   FILE *f_init_prob;            /*Initial probability vector file descriptor*/
   FILE *f_intervals;            /*Interval vector file descriptor           */
   FILE *f_expected;             /*Values of state variable file descriptor  */

   Matrix *Q;                    /*Generator Matrix                          */
   int num_t_points;             /* num of intervals to evaluate             */
   int *dvec;                    /*vector with destination states            */
   int *rvec;                    /*vector 1j, with 1's or 0's                */
   int set_of_intervals;
   double *intervals;            /*vector with intervals                     */
   double *initial_pi;           /*Initial probability vector                */
   int number_of_states;         /*Total number of states in the model       */
   int status;                   /*Auxiliary variable                        */
   int count;                    /*Auxiliary variables                       */
   char line[200];              /*Auxiliary variable                         */
   double **pi;                 /*vec w/n_t ptrs to vectors pi(t)            */
   double *pi_set;              /*vec w/n_t probabilities pi(t)              */
   double uniformization_rate;  /*uniformization rate of the model           */
   int i;                       /*auxiliary variable                         */
   int stop_condition;          /*stop_condition 0 - Max_Iterations 1- Conv  */
   info_intervals *set_of_R;    /*intervals and number of Erlang stages      */
   int temp,number_of_states_true;
   double value;

   f_dest     = NULL;
   f_expected = NULL;
   dvec       = NULL;
   rvec       = NULL;
   pi         = NULL;
   pi_set     = NULL;
   


   sprintf(gen_matrix_name,"%s.generator_mtx",basename);
   sprintf(intervals_name, "%s.int_iterative", basename);


   /*Open files*/
   
   if (!(f_gen_matrix = fopen(gen_matrix_name,"r")))
      {
         fprintf(stderr,"Efficient transient Probability Approximation method (Iterative): Error while opening file%s.\n",gen_matrix_name); 	
         return (30);
      }

   if(!(f_intervals = fopen(intervals_name,"r")))
      {
         fprintf(stderr,"Rate Cumulative Reward :Error while opening file %s.\n",intervals_name);
	 return(30);
      }	 


   if (measure_option == SET_PROB)
      {
         sprintf(dest_states_name,"%s.rate_reward.GlobalReward.states",basename);
	 if(!(f_dest = fopen(dest_states_name,"r")))
            {
               fprintf(stderr,"Efficient transient Probability Approximation method (Iterative) :Error while opening file %s.\n",intervals_name);
	       return(30);
            }
      }


   if(measure_option == EXPECTED_VAL)
      {
         sprintf(expected_name,"%s.rate_reward.%s",basename,state_var);
	 if(!(f_expected = fopen(expected_name,"r")))
            {
               fprintf(stderr,"Efficient transient Probability Approximation method (Iterative) :Error while opening file %s.\n",expected_name);
	       return(30);
            }
      }
            	    	

   sprintf(init_prob_name, "%s.init_prob", basename);
   if(!(f_init_prob = fopen(init_prob_name,"r")))
       {
           fprintf(stderr,"Efficient transient Probability Approximation method (Iterative) :Error while opening file %s.\n",init_prob_name);
	   return (30);
       }

   /*get generator matrix from file*/
   status = fscanf(f_gen_matrix,"%d",&number_of_states);
   if (status != 1)
      {
         fprintf(stderr,".generator_mtx : Invalide file format.\n");
	 return (32);
      }	
   if ((Q = get_trans_matrix2(f_gen_matrix,number_of_states)) == NULL)
      {
	 fprintf(stderr,"Error in Generator Matrix\n");
	 return (32);
      }

   fprintf(stderr,"\n Efficient transient Probability Approximation method (Iterative): Using %s file. Assuming it is a rate matrix.\n",gen_matrix_name);


 /*get all subintervals*/
 status = fscanf (f_intervals, "%d", &num_t_points);
  if (status != 1)
   {
      fprintf(stderr,"int_rew : Invalide file format.\n");
      return (32);
   }	
 
 status = fscanf (f_intervals, "%d", &set_of_intervals);
 if (status != 1)
   {
      fprintf(stderr,"int_rew : Invalide file format.\n");
      return (32);
   } 


 /* fill struct with information about subset*/


 set_of_R  = (info_intervals*)my_calloc((set_of_intervals),sizeof(info_intervals));
 intervals = get_intervals_R(f_intervals,num_t_points,set_of_intervals,set_of_R);


   switch (measure_option)
     {
        case(SET_PROB):
                       dvec = get_destination_states(f_dest);
                       fclose(f_dest);
        break;	       
       
        case(STATE_PROB):
                      dvec = (int *)my_calloc((number_of_states + 1), sizeof(int)); 
                      dvec[0]= number_of_states;
                      for (count = 1; count <= number_of_states; count ++)
                        dvec[count] = count;
	break;
	
	case(EXPECTED_VAL):
	break;
			
     }	   


   /*get initial probability vector*/
   initial_pi = get_initial_pi_2(f_init_prob,number_of_states);    
   fclose(f_init_prob);

   if (measure_option == SET_PROB)
     {
        /*allocate vector 1j*/	
        rvec = (int *)my_calloc((number_of_states + 1),sizeof(int));
        rvec[0] = number_of_states;     
        /* fill "value-by-state vector" with value of 1 in states
        belonging to the set and 0 to the states outside
        the set	*/
        for (count = dvec[1], i = 1; i <= dvec[0]; count = dvec[++i])
	    rvec[count] = 1;
     }

  if (measure_option == EXPECTED_VAL)
     {
       /* measure option == EXPECTED VALue	*/
	/* get value-by-state vector from input file*/
         rvec = (int *)my_calloc((number_of_states + 1),sizeof(int));
         rvec[0] = number_of_states;
	 fgets (line, 200, f_expected);
	 number_of_states_true = 0;
	 while (!feof(f_expected))
        {
	        
            number_of_states_true += 1;
            fgets(line, 200,f_expected);
        }    
     rewind(f_expected); 
     fgets (line, 200, f_expected);  
     number_of_states_true -= 1; 
     for (count = 1; count <= number_of_states_true; count++)
	   if( (status = fscanf(f_expected, "%d  %lf", &temp, &value)) != 2)
	      {
		   fprintf(stderr, "Efficient transient Probability Approximation method (Iterative): Error in %s\n", expected_name);
	           return(32);
       	  }
	   else
	       rvec[temp] = value;   
         fclose(f_expected);     
     }


 if (measure_option == SET_PROB || measure_option == EXPECTED_VAL)
	              pi_set = ESPA_itr_set(Q,set_of_R,
		               num_t_points,set_of_intervals,initial_pi,
		               rvec,&uniformization_rate,solution_option,precision,no_max_iterations,&stop_condition);
 
 if (measure_option == STATE_PROB)
		      pi = ESPA_itr(Q,set_of_R,
		           num_t_points,set_of_intervals,initial_pi,
		           dvec,&uniformization_rate,solution_option,precision,no_max_iterations,&stop_condition);

 /*printting the result */
 switch (measure_option)
   {
      case (SET_PROB):
			   sprintf(out_name,"%s.TS.espa_iterative.SET_PROB",basename);
		           if ((f_out = fopen(out_name,"w+")) == NULL)
		               {
			           fprintf(stderr,"%s","Error in output file creation\n");
			           return(31);
			        }	
			   fprintf(f_out,"#Solution Method:    Efficient transient Set Probability (Approximation Method).\n");
			   fprintf(f_out,"#Uniformization Rate:%.6e  \n",uniformization_rate);
			   fprintf(f_out,"#Set Probability given the initial probability distribution\n");
			   fprintf(f_out,"#Time                       Set Probability\n"); 	  
			   fprintf(f_out,"#--------------------------------------------\n");
			   /*loop through all instants*/
			   for (count = 1;count<=num_t_points;count++)
		             fprintf(f_out,"%.8f\t\t\t%10e\n",intervals[count],pi_set[count]);
	                   fclose(f_out);

		           break;
	case (STATE_PROB):
		      		     
		          for(count = 1;count<=num_t_points;count++)
			     {
				  sprintf(out_name,"%s.TS.espa_iterative.STATE_PROB.%10e",basename,intervals[count]);
				    if ((f_out = fopen(out_name,"w+")) == NULL)
				       {
					   fprintf(stderr,"%s","Error in output file creation\n");
					   return(31);
					}
				
			           fprintf(f_out,"#Solution Method:    Efficient transient State Probability (Approximation Method).\n");
				   fprintf(f_out,"#Uniformization Rate:%.6e  \n",uniformization_rate);
				   fprintf(f_out,"#State Probability given the initial probability \n");
				   fprintf(f_out,"# Destination State                  State Probability \n"); 	  
				   fprintf(f_out,"#--------------------------------------------------------- \n");
				   for (i=1; i <= dvec[0] ; i++)
			              fprintf(f_out,"  %d                                   %.10e\n",dvec[i], pi[count][i]);		
			           fclose(f_out);
			     }

         		 break;
        case (EXPECTED_VAL):

			    sprintf(out_name,"%s.TS.espa_iterative.EXPECTED_VAL",basename);
		            if ((f_out = fopen(out_name,"w+")) == NULL)
		               {
			           fprintf(stderr,"%s","Error in output file creation\n");
			           return(31);
			        }	
			   fprintf(f_out,"#Solution Method:    Efficient transient State Probability(Approximation Method).\n");
			   fprintf(f_out,"#Uniformization Rate:%.6e  \n",uniformization_rate);
			   fprintf(f_out,"#Expected Value given the initial probability\n");
			   fprintf(f_out,"#Time                       Expected Value\n"); 	  
			   fprintf(f_out,"#--------------------------------------------\n");
			   /*loop through all instants*/
			   for (count = 1;count<=num_t_points;count++)
		             fprintf(f_out,"%.10g                        %10e\n",intervals[count],pi_set[count]);
	                   fclose(f_out);
		           break;

   }

/*closing all files*/
  fclose (f_gen_matrix);
  fclose (f_intervals);

/* rel. mem. alloc.*/
  if (measure_option == STATE_PROB)
	{
	    for (count = 0; count <=  num_t_points ; count++)
		/* release memory allocated for pi[count]*/
		free(pi[count]);	
	    free(pi);/* free mem. alloc. for pi	*/
	}
  else	/* measure option == SET_PROB or EXPECTED_VALue*/
	free(pi_set);	/* free memory alloc for pi_set	*/
	

  if ((measure_option == STATE_PROB)||(measure_option == SET_PROB))
     free(dvec);

  if (measure_option != STATE_PROB)
    free(rvec);

  free(initial_pi);     


  if (stop_condition == MAX_ITERATIONS)
     return (89);
  else
     return (0);

}

/******************************************************/
/* interface expected transient impulse reward        */
/******************************************************/


int interface_transient_impulse_reward( basename, rewardname, precision )
char *basename;
char *rewardname;
double precision;
{
    char filename[ 1024 ], filename_reward[ 1024];
    FILE *f_out;                 /* ptr to output file                       */
    FILE *f_Q;
    FILE *f_R;
    FILE *f_self;                /* transitions from same src and dst        */
    FILE *f_initProb;            /* ptr to initial prob. desc. file          */
    FILE *f_intervals;           /* ptr to interval description file         */
    Matrix *Q;
    Matrix *R;                   /* Matrix with reward associated with trans */
    Matrix *P;
    double epsilon;              /* error upperbound in summation            */
    double LAMBDA;               /* uniformization const                     */
    double lamb_t;               /* product LAMBDA * t                       */
    double *time_points;         /* ptr to t values                          */
    int num_t_points;            /* num of intervals to evaluate             */
    double *pi0;                 /* ptr to initial probabilities             */
    double *exp_imp_rew;         /* ptr to expected impulse reward           */
    int    number_of_states;     /* number of states                         */
    double *Min_poisson_val;     /* Poisson evaluated at Nmin for a give time*/
    int    *N_min;               /* ptr to low bound (poisson) for each time */
    int    *N_max;               /* ptr to up bound (truncation point for    */
                                 /* posiion) for each time.                  */
    int    i, j;
    double rate;
    char   line[ 256 ];

    epsilon = precision;
    sprintf( filename, "%s.generator_mtx", basename );
    if( (f_Q = fopen( filename, "r" )) == NULL )
    {
        fprintf( stderr, "Expected transient impulse reward method: \
                 Error while opening file %s\n", filename);
        return(30);
    }

    sprintf( filename_reward, "%s.impulse_reward.%s", basename, rewardname );
    if( (f_R = fopen( filename_reward, "r" )) == NULL )
    {
        fprintf( stderr, "Expected transient impulse reward method: \
                 Error while opening file %s\n", filename);
        fclose( f_Q );
        return(30);
    }

    sprintf( filename, "%s.intervals", basename );
    if( (f_intervals = fopen( filename, "r" )) == NULL )
    {
        fprintf( stderr, "Expected transient impulse reward method: \
                 Error while opening file %s\n", filename);
        fclose( f_Q );
        fclose( f_R );
        return(30);
    }

    sprintf( filename, "%s.init_prob", basename );
    if( (f_initProb = fopen( filename, "r" )) == NULL )
    {
        fprintf( stderr, "Expected transient impulse reward method: \
                 Error while opening file %s\n", filename);
        fclose( f_Q );
        fclose( f_R );
        fclose( f_intervals );
        return(30);
    }

    sprintf( filename, "%s.TS.expected_imp_reward.%s", basename, rewardname );
    if( (f_out = fopen( filename, "w" )) == NULL )
    {
        fprintf( stderr, "Expected transient impulse reward method: \
                 Error while opening file %s\n", filename);
        fclose( f_Q );
        fclose( f_R );
        fclose( f_intervals );
        fclose( f_initProb );
        return(30);
    }

    /* Read matrix */
    Q = get_trans_matrix(f_Q);    
    number_of_states = Q->num_col;

    fprintf( stderr, "\nExpected transient impulse reward method:\
 Using %s.generator_mtx file. Assuming it is a rate matrix.\n", basename);

    /* Read intervals */
    time_points = get_intervals(f_intervals, &num_t_points);

    if( time_points == NULL )
    {
       fprintf(stderr, "Expected transient impulse reward method: Invalid \
                       intervals description\n");
       return(59);
    }

    /* Read initial probabilities file desc */
    pi0 = get_initial_pi(f_initProb, number_of_states);

    if( pi0 == NULL )
    {
       fprintf(stderr, "Expected transient reward method: Invalid initial probability description\n");
       return(55);
    }

    /* Read the impulse reward matrix */
    R = get_impulse_matrix(f_R, number_of_states);

    generate_diagonal('Q', Q);

    LAMBDA = get_uniformization_factor( Q );

    sprintf( filename, "%s.trans_same_st", basename );
    if( (f_self = fopen( filename, "r" )) != NULL )
    {
        while( !feof( f_self ) )
        {
            fgets( line, 256, f_self );
            if( sscanf( line, "%d %d %lf", &i, &j, &rate ) == 3 )
                if( i == j && rate - get_matrix_position( i, j, Q ) > LAMBDA
                    && get_matrix_position( i, j, R ) != 0.0 )
                    LAMBDA = rate - get_matrix_position( i, j, Q );
        }
        fclose( f_self );
    }

    P = create_prob_matrix( Q, LAMBDA );

    /* allocates vectors */
    N_min  = (int    *) my_calloc(num_t_points * sizeof(int), sizeof(int));
    N_max  = (int    *) my_calloc(num_t_points * sizeof(int), sizeof(int));
    Min_poisson_val = (double *) my_calloc(num_t_points, sizeof(double));

    for( i = 0; i < num_t_points; i++ )
    {
       lamb_t = LAMBDA * time_points[i];
       lo_bound( &(N_min[i]), &(Min_poisson_val[i]), lamb_t );
       N_max[i] = partial_poisson_sum( lamb_t, epsilon );
    }
#ifdef DEBUG
    for( i = 0; i < num_t_points; i++ )
    {
       printf( "Expected transient reward method: N_min[%d] = %d, \
               N_max[%d] = %d, Min_poisson_val[%d] = %f \n",
          i, N_min[i], i, N_max[i], i, Min_poisson_val[i] );
    }
#endif

    exp_imp_rew = expected_trans_imp_reward( pi0, P, R, LAMBDA, num_t_points,
                                   time_points, N_min, N_max, Min_poisson_val );

    /* print input values */
    fprintf( f_out, "#Solution method: Expected Transient Impulse Reward \
                    (Uniformization)\n" );
    fprintf( f_out,"#Lambda = %.6e, Precision = %.6e \n\n", LAMBDA, precision );

    fprintf( f_out, "#Time (t)             E[NR(t)] \n" );
    for( i = 0; i < num_t_points; i++ )
    {
        fprintf( f_out,"#Time = %.5e, N_min = %d, N_max = %d\n", time_points[i],
                 N_min[i], N_max[i] );
        fprintf( f_out, "%.5e   %.10e\n", time_points[i], exp_imp_rew[i] );
    }

    fprintf(f_out,"#**************************\n");

    free( (Matrix *)Q );
    free( (Matrix *)P );
    free( (double *)pi0 );

    free( (int *) N_min );
    free( (int *) N_max );
    free( (double *)Min_poisson_val );

    free( (double *)time_points );
    free( (double *)exp_imp_rew );

    fclose( f_out );
    fclose( f_initProb );
    fclose( f_intervals );
    fclose( f_R );
    fclose( f_Q );

    return(0);
}

