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

/* Este programa extrai medidas de uma sequencia de video.                */
/* E' necessario um arquivo que contenha o                                */
/* tamanho ou taxa (celulas, bits ou bytes) de cada unidade de informacao */
/* (frames, GOBS, stripes, etc.) separados por "\n". O programa fornece:  */
/* - autocovariancia (Cov(t)),                                            */
/* - indice de dispersao (IDC(t)),                                        */
/* - distribuicao do tempo total de rajada em T (P[TR(T) < t]),           */
/* - distribuicao da duracao da rajada (DR(t)),                           */
/* - vetor de probabilidades iniciais para P[TR(T) < t] e IDC(t),         */
/* - vetor de probabilidades iniciais de rajada para DR(t).               */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRUE     1
#define FALSE    0

#define fator_normal (4.656612875e-10)  /* Usado pela funcao do Edmundo.     */
                                        /* Tem que multiplicar o resultado   */
                                        /* retornado por simulrandom por     */
                                        /* fator_normal.                     */


/* Fornece uma variavel de distribuicao exponencial */
double poisson_time();

/* Fornece uma variavel de distribuicao uniforme */
double simulrandom();

/* Explica a linha de comando do programa */
void usage();

/* Detecao de erro preliminar */
void pre_err_detection_treat();

/* Seleciona os sample-paths baseado no tempo limite e tamanho do trace. */
int  *select_sample_paths();
    
/* Calcula descritores estacionarios */
void steady_descriptors();

/* Calcula vetor de probabilidades iniciais associado */
void init_prob();

/* Calcula autocovariancia */
void med_cov();

/* Calcula indice de dispersao */
void med_idc();

/* Calcula distribuicao do tempo total de rajada */
void med_tot_burst_dist();

/* Calcula distribuicao do tempo total de rajada */
void med_tot_burst_expect();

/* Calcula distribuicao complementar da duracao da rajada */
void med_burst_dist();

/* Calcula a duracao media da rajada */
void med_burst_expect();


int main(argc, argv)
int  argc;
char *argv[];
{
    FILE   *movie;
    double *frames;
    double min, max;
    double step;
    char   rate_str[500];
    double acc_X, var_X;
    int    SAMPLES;
    int    *amostras;
    int    num_amostras;
    int    i, j, count;
    int    counts_btw_points;
    char   descriptor_name[50];

    /*** Parametros: ***/
    int    LEVELS;      /* Numero de estados da cadeia do modelo. */
    double TRIGGER;     /* Nivel minimo para rajada. */ 
    char   MOVIE[1000];  /* Nome do arquivo com a sequencia. (ASCII) */
    int    FRAMES;      /* Numero de amostras da sequencia. */ 
    double INTERVAL;    /* Intervalo entre as amostras (seg.). */
    double MODE;        /* Assume INTERVAL ou 1.0, dependendo de TYPE. */
    char   TYPE;        /* Tipo de entrada: r = rate, s = size. */
    char   DIST;        /* Distancia entre sample-paths: e = exponencial
			                                 f = fixo        */
    char   OPTION;      /* Opcao de medida: 0 - Autocorrelacao;
                                            1 - Autocovariancia;
                                            2 - IDC;
                                            3 - Descritores estacionarios. */
    double LIMITE;      /* Tempo maximo para as medidas. */
    double DIVISOR;     /* Para se evitar overflow. */
    int    POINTS;      /* Number of points. */
    int    NUM_TIME;    /* Numero de amostras em LIMITE (LIMITE/INTERVAL). */
    double EMP_FREQ;    /* Freq. empirica p/distribuicao de sample-paths.  */



    /* Defaults: */
    /* (Never changes until upgrade) */
    TRIGGER = 0.0;
    DIVISOR = 1.0;
    EMP_FREQ = 0.0;
    LEVELS = 1;
    /* (Maybe changed) */
    INTERVAL = 1.0;
    TYPE = 's';
    DIST = 'f';
    /* (Must be changed for transient measures) */
    LIMITE = 0.0;
    POINTS = 0;
    /* (Must be changed) */
    strcpy(MOVIE,"");
    FRAMES = 0;
    OPTION = 'N';
    amostras = NULL;


    /* Aquisitando parametros: */
    if (argc == 1)
    {
        usage();
        exit(20);
    }

    for (i = 1, j = 0; i < argc; i++, j = 0)
    {
        if (argv[i][0] == '-')
            j++;

#ifdef DEBUG
        printf("%s %c %s\n",argv[i],argv[i][j],&(argv[i][j+2])); 
#endif

        switch(argv[i][j])
        {
          case 'o' :
          case 'O' : OPTION = argv[i][j + 2];
	             break;
          case 't' :
          case 'T' : TYPE = argv[i][j + 2];
                     break;
          case 'a' :
          case 'A' : strcpy(MOVIE,(char *)&(argv[i][j + 2]));
                     break;
          case 'n' :
          case 'N' : FRAMES = atoi((char *)&(argv[i][j + 2]));
                     break;
          case 'i' :
          case 'I' : INTERVAL = atof((char *)&(argv[i][j + 2]));
                     break;
          case 'r' :
          case 'R' : TRIGGER = atof((char *)&(argv[i][j + 2]));
                     break;
          case 'd' :
          case 'D' : DIVISOR = atof((char *)&(argv[i][j + 2]));
                     break;
          case 'l' :
          case 'L' : LIMITE = atof((char *)&(argv[i][j + 2]));
                     break;
          case 'm' :
          case 'M' : LEVELS = atoi((char *)&(argv[i][j + 2]));
                     break;
          case 'p' :
          case 'P' : POINTS = atoi((char *)&(argv[i][j + 2]));
                     break;
       	  case 's' :
          case 'S' : DIST = argv[i][j + 2];
	             break; 
          case 'e' :
          case 'E' : EMP_FREQ = atof((char *)&(argv[i][j + 2]));
                     break;
          default  : usage();
                     exit(21);
        }
    }


    switch(OPTION)
    {
      case '0' : strcpy(descriptor_name,"Trace Autocorrelation");
                 break;
      case '1' : strcpy(descriptor_name,"Trace Autocovariance");
                 break;
      case '2' : strcpy(descriptor_name,"Trace IDC");
                 break;
      case '3' : strcpy(descriptor_name,"Trace Stationary descriptors");
                 break;
      case '4' : strcpy(descriptor_name,"Trace Burst Total Time Distribution");
                 break;
      case '5' : strcpy(descriptor_name,"Trace Burst Duration Distribution");
                 break;
      case '6' : strcpy(descriptor_name,"Trace Burst Duration Autocorrelation");
                 break;
      case '7' : strcpy(descriptor_name,"Trace Burst Duration Autocovariance");
                 break;
      case '8' : strcpy(descriptor_name,"Trace Burst Total Time Expectation");
                 break;
      case '9' : strcpy(descriptor_name,"Trace Burst Duration Expectation");
                 break;
      default  : usage();
                 exit(21);
    }


    /* Detecao preliminar de erro e tratamento de parametros. */
    /* Sai do programa se houver parametro inconsistente.     */
    pre_err_detection_treat(DIST, TYPE, &MODE, MOVIE, FRAMES, OPTION, LIMITE, INTERVAL,
                    &NUM_TIME, &EMP_FREQ, POINTS, &counts_btw_points, descriptor_name);


#ifdef DEBUG
    printf("\n");
    printf("t= %c\na= %s\nn= %d\ni= %.10f\nr= %.10f\nd= %.10f\nl= %.10f\nm= %d\np= %d\nd= %c\n",
                 TYPE, MOVIE, FRAMES, INTERVAL, TRIGGER, DIVISOR, LIMITE, LEVELS, POINTS, DIST);
    printf("*****\n");
    printf("Num_time: %d, Emp_time: %.6e\n", NUM_TIME, EMP_FREQ); 
    printf("*****\n");
    printf("Counts between points = %d\n", counts_btw_points);
    printf("*****\n");
#endif


    /* Alocando memoria: */
    if ((frames = (double *) malloc(FRAMES * sizeof(double))) == NULL)
    {
       fprintf(stderr,"%s: Cannot allocate memory space with %d bytes.\n", descriptor_name,
                                                                (FRAMES * sizeof(double)));
       exit(40);
    }


    /* Leio o arquivo de entrada. Apesar de chamar "rate_str", esta variavel */
    /* contera' o tamanho da amostra.                                        */
    if ((movie = fopen(MOVIE,"r")) == NULL)
    {   
        fprintf(stderr,"%s: Error while opening file %s\n", descriptor_name, MOVIE);
        exit(30);
    }
    
    for (count = 0; count < FRAMES; count++)
    {
        fscanf(movie,"%s", rate_str);
        frames[count] = (atof(rate_str))/DIVISOR;
    }
    
    fclose(movie);


    if ((OPTION == '2') || (OPTION == '4') || (OPTION == '5') || (OPTION == '8') || (OPTION == '9'))
        /* Seleciona os sample-paths baseado no tempo limite e tamanho do trace. */
        amostras = select_sample_paths(DIST, FRAMES, NUM_TIME, EMP_FREQ, INTERVAL,
                                &num_amostras, OPTION, frames, TRIGGER, &SAMPLES);


    /* Calcula descritores estacionarios e joga pra arquivo se OPCAO = 3 */
    steady_descriptors(MOVIE, FRAMES, frames, &acc_X, &var_X, MODE, OPTION, 
                                                      &min, &max, TRIGGER);
    

    /* Calcula intervalo entre as recompensas */
    step = (max - min)/LEVELS;

    /* Calculo vetor de probabilidades iniciais dos sample-paths.          */
    /* init_prob(num_amostras, amostras, frames, min, step, LEVELS, MODE); */


    if ((OPTION == '0') || (OPTION == '1'))
        /* Calcula a autocorrelacao ou a autocovariancia da sequencia. */
        med_cov(MOVIE, NUM_TIME, FRAMES, frames, INTERVAL, acc_X, MODE,
                                            counts_btw_points, OPTION);

    if (OPTION == '2')
        /* Efetua o calculo do IDC. */
        med_idc(MOVIE, NUM_TIME, num_amostras, amostras, frames, INTERVAL, TYPE,
                                                             counts_btw_points);

    if (OPTION == '4')
        /* Efetua o calculo de P[TR(t) < s]. */
        med_tot_burst_dist(MOVIE, NUM_TIME, num_amostras, amostras, frames, INTERVAL,
                                                 TRIGGER, counts_btw_points, FRAMES);

    if (OPTION == '8')
        /* Efetua o calculo de E[TR(t)]. */
        med_tot_burst_expect(MOVIE, NUM_TIME, num_amostras, amostras, frames, INTERVAL,
                                                 TRIGGER, counts_btw_points, FRAMES);

    if (OPTION == '5')
        /* Efetua o calculo de P[DR(t) > s]. */
        med_burst_dist(MOVIE, NUM_TIME, frames, INTERVAL, TRIGGER, LEVELS, FRAMES, min,
                        step, MODE, counts_btw_points, POINTS, amostras, num_amostras);

    if (OPTION == '9')
        /* Efetua o calculo de E[DR(t)]. */
        med_burst_expect(MOVIE, NUM_TIME, frames, INTERVAL, TRIGGER, LEVELS, FRAMES, min,
                        step, MODE, counts_btw_points, POINTS, amostras, num_amostras);

    /*** Fim ***/

    return 0;
}


/*****************************************************************************/
/* Gera uma variavel aleatoria exponencial com parametro L, fornecido para a */
/* funcao.                                                                   */
/*****************************************************************************/
double poisson_time(L)
double L;
{
    double U, X, time;
    static long seed = 377003613; /* Edmundo possui uma lista com 300  */
                                  /* sementes, mas so' preciso de uma. */

    U = simulrandom(seed);
    seed = (long) U;
    U *= fator_normal;
    X = log(U);

    time = -(1.0/L) * X;

    return (time);
}


/******************************************************************************/
/* Gera valor aleatorio. Esta func. e do Edmundo, melhor que a random() do C. */
/* O retorno desta func. deve ser utilizado na prox. chamada dela. Ou seja,   */
/* gera um valor aleatorio a partir do parametro seed e retorna este valor    */
/* aleatorio. Este valor retornado deve ser utilizado na proxima instancia.   */
/******************************************************************************/
double simulrandom (seed)
long seed;
{
  int ix = seed;
  int k = ix/127773;
  ix = 16807*(ix - k*127773) - k*2836;
  if (ix < 0) ix += 2147483647;
  seed = ix;
  return((double) ix );
}


/******************************************************************************/
/* Mostra uma ajuda para a utilizacao do programa.                            */
/******************************************************************************/
void usage()
{
  fprintf(stdout, "\nUsage: measure_descs [-o=descriptor_selection] [-a=tracename] ");
  fprintf(stdout, "[-n=number_of_samples] [-i=interval_of_a_sample] ");
  fprintf(stdout, "[-l=limit_time_for_transient_measures] [-t=sample_type] ");
  fprintf(stdout, "[-p=number_of_points] [-r=maximum_value_below_burst]\n\n");
  fprintf(stdout, "Selection options:\n");
  fprintf(stdout, "       0 - Autocorrelation\n");
  fprintf(stdout, "       1 - Autocovariance\n");
  fprintf(stdout, "       2 - IDC\n");
  fprintf(stdout, "       3 - Stationary descriptors (mean, variance, etc)\n");
  fprintf(stdout, "       4 - Burst Total Time Distribution\n");
  fprintf(stdout, "       5 - Burst Duration Distribution;\n");
  fprintf(stdout, "       8 - Burst Total Time Expectation and Variance\n");
  fprintf(stdout, "       9 - Burst Duration Expectation and Variance;\n\n");
  fprintf(stdout, "Upper case is allowed and '-' can be omitted.\n");
  /* printf("[-d=divisor] [-m=number_of_states] "); */
}


/******************************************************************************/
/* Detecao preliminar de erro na entrada e tratamento dos parametros.         */
/******************************************************************************/
void pre_err_detection_treat(DIST, TYPE, MODE, MOVIE, FRAMES, OPTION, LIMITE, 
    INTERVAL, NUM_TIME, EMP_FREQ, POINTS, counts_btw_points, descriptor_name)
char   DIST;               /* Distribuicao da distancia entre sample-paths.   */
char   TYPE;               /* Tipo de entrada.     */
double *MODE;              /* Assume INTERVAL ou 1.0, dependendo de TYPE.     */
char   *MOVIE;             /* Nome do arquivo com a sequencia. (ASCII)        */
int    FRAMES;             /* Numero de amostras da sequencia.                */
char   OPTION;             /* Selecao de descritor.               */
double LIMITE;             /* Tempo maximo para as medidas.       */
double INTERVAL;           /* Intervalo entre as amostras (seg.). */
int    *NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL). */
double *EMP_FREQ;          /* Freq. empirica p/distribuicao de sample-paths.  */
int    POINTS;             /* Number of points.    */
int    *counts_btw_points; /* Numero de frames entre os pontos de medida.     */
char   *descriptor_name;   /* descriptor name */
{
    int end;


    /* Inicializacao, end = TRUE indica saida por erro na entrada de parametros */
    end = FALSE;
    
    if ((DIST != 'f') && (DIST != 'F') && (DIST != 'e') && (DIST != 'E'))
    {
        fprintf(stderr,"%s: Wrong sample-path distribution specification. (s = %c)\n",
                                                               descriptor_name, DIST);
        end = TRUE;
    }

    if ((TYPE == 's') || (TYPE == 'S'))      /* Arquivo contem tamanho dos frames    */
        *MODE = INTERVAL;
    else    
        if ((TYPE == 'r') || (TYPE == 'R'))  /* Arq. contem taxa instant. dos frames */
            *MODE = 1.0;
        else
        {
            fprintf(stderr,"%s: Wrong entry mode specification. (t = %c)\n", descriptor_name, TYPE);
            end = TRUE;
        }
    
    if (!strcmp(MOVIE,""))
    {
        fprintf(stderr,"%s: Need trace file name specification.\n", descriptor_name);
        end = TRUE;
    }
    
    if (FRAMES == 0)
    {
        fprintf(stderr,"%s: Need number of frames specification.\n", descriptor_name);
        end = TRUE;
    }
    
    if (OPTION == 'N')
    {
        fprintf(stderr,"%s: Need descriptor selection.\n", descriptor_name);
        end = TRUE;
    }
    
    if ((OPTION != '0') && (OPTION != '1') && (OPTION != '2') && (OPTION != '3') &&
        (OPTION != '4') && (OPTION != '5') && (OPTION != '8') && (OPTION != '9'))
    {
        fprintf(stderr,"%s: Invalid descriptor selection.\n", descriptor_name);
        end = TRUE;
    }
    
    if ((LIMITE == 0.0) && (OPTION != '3'))
    {
        fprintf(stderr,"%s: Need limit time for transient measures specification.\n", descriptor_name);
        end = TRUE;
    }
    
    
    /* Sai se houve erro */
    if (end == TRUE)
        exit(57);
    

    /* Numero de frames ate' o tempo limite */
    *NUM_TIME = LIMITE/INTERVAL;
    
    /* Se nao foi definido */
    if (*EMP_FREQ == 0.0)
        *EMP_FREQ = 10.0/LIMITE;      /* Empirical */
    
    if (POINTS)
    {
       if (((*counts_btw_points = (*NUM_TIME)/POINTS) < 1) && (OPTION != '3'))
       {
           *counts_btw_points = 1;
  	       fprintf(stderr,"%s: Number of points shrinked to %d\n", descriptor_name, *NUM_TIME);
       }
    }
    else
       *counts_btw_points = 1;
}


/********************************************************************************/
/* Seleciona os sample-paths baseado no tempo limite e tamanho do trace.        */
/********************************************************************************/
int *select_sample_paths(DIST, FRAMES, NUM_TIME, EMP_FREQ, INTERVAL, num_amostras,
                                                 OPTION, frames, TRIGGER, SAMPLES)
char   DIST;               /* Distribuicao da distancia entre sample-paths.   */
int    FRAMES;             /* Numero de amostras da sequencia.                */
int    NUM_TIME;           /* Numero de amostras em LIMITE (LIMITE/INTERVAL). */
double EMP_FREQ;           /* Freq. empirica p/distribuicao de sample-paths.  */
double INTERVAL;           /* Intervalo entre as amostras (seg.).             */
int    *num_amostras;      /* Numero de sample-paths.            */
char   OPTION;             /* Descritor selecionado.             */
double *frames;            /* Vetor com valores dos frames.      */
double TRIGGER;            /* Valor maximo de taxa para nao ser rajada.       */
int    *SAMPLES;           /* Numero maximo estimado de amostras              */
{
    int    cont, i;
    int    acum;
    double time, quant;
    int    min_amostras;
    int    *amostras;      /* Vetor com o indice de cada sample-path.         */


    i = 0;

    /* Estimando numero maximo de amostras */
    *SAMPLES = (FRAMES/NUM_TIME);


    /* Alocando espaco para vetor contendo o inicio das amostras */
    amostras = (int *) malloc((*SAMPLES) * sizeof(double));


    /* Armazena em "amostras[i]" o offset do i-esimo sample-path. */
    /* Os intervalos entre os sample-paths podem ser distribuidos */
    /* exponencialmente ou igualmente espacados.                  */
    cont = 0;
    min_amostras = 1000000; /* Apenas um valor alto. */


    if ((DIST == 'e') || (DIST == 'E'))
    {
        if (OPTION == '2')
        {
            amostras[0] = 0;
            acum = 0;
        }
        else
        {
            acum = 0;
            while (frames[acum] <= TRIGGER)
                acum++;
            amostras[0] = acum;
        }

        for (i = 1; ((FRAMES - acum) > NUM_TIME) && (i < *SAMPLES); i++)
        {
            /* Pega intervalo com distribuicao exponencial. */
            time = poisson_time(EMP_FREQ);
            quant = (floor(time/INTERVAL));
        
            /* Para o caso de dois sample-paths comecando no mesmo ponto */
            /* if (quant < 1) , nao uso mais pois permite overlap        */ 
        
            if (quant < NUM_TIME) /* para nao ter overlap */
            {
                i--;
                cont++;
                continue;
            }
        
            acum += (int) quant;

            if (OPTION == '2')
                amostras[i] = acum;
            else
            {
                while (((frames[acum] <= TRIGGER) || (frames[acum - 1] > TRIGGER)) && (acum < FRAMES))
                    acum++;

                if ((FRAMES - acum) >= NUM_TIME)
                    amostras[i] = acum;
                else
                    i--;
            }
        
            if (quant < min_amostras)
                min_amostras = quant;
        }
    }
    
    if ((DIST == 'f') || (DIST == 'F'))
    {
        if (OPTION == '2')
        {
            amostras[0] = 0;
            acum = 0;
        }
        else
        {
            acum = 0;
            while (frames[acum] <= TRIGGER)
                acum++;
            amostras[0] = acum;
        }

        for (i = 1; ((FRAMES - acum) > NUM_TIME) && (i < *SAMPLES); i++)
        {
            acum += NUM_TIME;

            if (OPTION == '2')
                amostras[i] = acum;
            else
            {
                while (((frames[acum] <= TRIGGER) || (frames[acum - 1] > TRIGGER)) && (acum < FRAMES))
                    acum++;

                if ((FRAMES - acum) >= NUM_TIME)
                    amostras[i] = acum;
                else
                    i--;
            }
        }
    }

    *num_amostras = i;

#ifdef DEBUG
    /********/
    for (i = 0; i < *num_amostras; i++)
        printf("amostras[%d] = %d\n", i, amostras[i]);
    /********/
    printf("Number of sample-paths: %d\n", *num_amostras);
    printf("*****\n");

    if ((DIST == 'e') || (DIST == 'E'))
    {
        /* So' para estatistica. */
        printf("Min. amostras: %d\n", min_amostras);
        printf("Amostras perdidas: %d\n", cont);
        printf("*****\n");
    }
#endif

    return amostras;
}


/*****************************************************************************/
/* Calcula a taxa maxima, minima, media e variancia.                         */
/*****************************************************************************/
void steady_descriptors(MOVIE, FRAMES, frames, acc_X, var_X, MODE, OPTION,
                                                        min, max, TRIGGER)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII) */
int    FRAMES;            /* Numero de amostras da sequencia.         */
double *frames;           /* Ponteiro para a sequencia de amostras.   */
double *acc_X;            /* Taxa media.           */
double *var_X;            /* Taxa media.           */
double MODE;              /* Modo de entrada.      */
char   OPTION;            /* Selecao de descritor. */
double *min;              /* Minimum frame size.   */
double *max;              /* Maximum frame size.   */
double TRIGGER;           /* Nivel minimo para rajada. */
{
    int    count;
    double rate, min_rate, max_rate;
    double acc_burst, count_burst;
    char   out[1000];
    FILE   *f_steady;


    *acc_X      = 0.0;
    count_burst = 0.0;
    acc_burst   = 0.0;
    min_rate    = max_rate = 0.0;
    for (count = 0; count < FRAMES; count++)
    {
        rate = frames[count]/MODE;
	
#ifdef DEBUG
    /* printf("Index: %d  Rate: %f\n", count, rate); */
#endif
        
        if (count == 0)
            min_rate = max_rate = rate;
        else
        {
            if (rate < min_rate)
                min_rate = rate;
            
            if (rate > max_rate)
                max_rate = rate;
        }
        
        *acc_X = *acc_X + (rate/FRAMES);
        
        if (rate > (TRIGGER/MODE))
        {
            acc_burst += rate;
            count_burst += 1.0;
        }
    }
    
    *var_X = 0.0;
    for (count = 0; count < FRAMES; count++)
        *var_X = *var_X + 
                        (((frames[count]/MODE) - *acc_X) * 
                         ((frames[count]/MODE) - *acc_X))/(FRAMES - 1);


    if (OPTION == '3')
    {
        strcpy(out,MOVIE);
        strcat(out,".seq_stationary_Descriptors");
        if ((f_steady = fopen(out,"w")) == NULL)
        {   
            fprintf(stderr,"Trace Stationary descriptors: Error while opening file %s\n", out);
            exit(31);
        }
        
        fprintf(f_steady,"# Stationary Descriptors:\n");
        fprintf(f_steady,"# Min rate  = %.6e\n", min_rate);
        fprintf(f_steady,"# Max rate  = %.6e\n", max_rate);
        fprintf(f_steady,"# E[rate]   = %.6e\n", *acc_X);
        fprintf(f_steady,"# Var[rate] = %.6e\n", *var_X);

        /* fprintf("f_steady,E[burst]  = %.6e\n", (acc_burst/count_burst)); */
        /* fprintf("f_steady,*****\n"); */
        
        fclose(f_steady);
        
        exit(0);
    }

    *min = min_rate * MODE;
    *max = max_rate * MODE;

#ifdef DEBUG
    if ((OPTION == '4') || (OPTION == '5') || (OPTION == '6') || (OPTION == '7'))
        printf("Max. burst length: %.6e   Min. burst length: %.6e\n*****\n", *max, *min);
    else
        printf("Max size: %.6e   Min size: %.6e\n*****\n", *max, *min);
#endif
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida a autocovariancia para cada        */
/* intervalo de tempo.                                                        */
/******************************************************************************/
void med_cov(MOVIE, NUM_TIME, FRAMES, frames, INTERVAL, acc_X, MODE,
                                 counts_btw_points, OPTION, TRIGGER)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII)    */
int    NUM_TIME;          /* Numero de total de pontos para se calcular. */
int    FRAMES;            /* Numero de amostras da sequencia.       */
double *frames;           /* Ponteiro para a sequencia de amostras. */
double INTERVAL;          /* Intervalo entre as amostras (seg.).    */
double acc_X;             /* Taxa media.     */
double MODE;              /* Modo de entrada */
int    counts_btw_points; /* Amostras entre os pontos consecutivos de observacao */
char   OPTION;            /* Selecao de descritor */
double TRIGGER;           /* Nivel maximo abaixo de uma rajada. */
{
    FILE   *cov, *corr;
    char   out[1000], trigger_str[300];
    int    count, ind1, ind2;
    double Xo_Xn, acc_Xo_Xn;
    double autocov, time;
    double var;


    cov = corr = NULL;
    strcpy(out,MOVIE);
    if (OPTION == '0')
    {
        strcat(out,".seq_autocorrelation");
        if ((corr = fopen(out,"w")) == NULL)
        {
            fprintf(stderr,"Trace Autocorrelation: Error while opening file: %s\n", out);
            exit(31);
        }
    }

    if (OPTION == '1')
    {
        strcat(out,".seq_autocovariance");
        if ((cov = fopen(out,"w")) == NULL)
        {
            fprintf(stderr,"Trace Autocovariance: Error while opening file: %s\n", out);
            exit(31);
        }
    }

    if (OPTION == '6')
    {
        strcat(out,"_dur_autocorrelation");
        sprintf(trigger_str,"_%.6e",TRIGGER);
        strcat(out,trigger_str);
        if ((corr = fopen(out,"w")) == NULL)
        {
            fprintf(stderr,"Trace Burst Duration Autocorrelation: Error while opening file: %s\n", out);
            exit(31);
        }
        fprintf(corr, "# Maximum value below burst: %f\n", TRIGGER);
        fprintf(corr, "# Number of bursts: %d\n", FRAMES);
    }

    if (OPTION == '7')
    {
        strcat(out,"_dur_autocovariance");
        sprintf(trigger_str,"_%.6e",TRIGGER);
        strcat(out,trigger_str);
        if ((cov = fopen(out,"w")) == NULL)
        {
            fprintf(stderr,"Trace Burst Duration Autocovariance: Error while opening file: %s\n", out);
            exit(31);
        }
        fprintf(cov, "# Maximum value below burst: %.6e\n", TRIGGER);
        fprintf(cov, "# Number of bursts: %d\n", FRAMES);
    }


    /* Checking a commom problem when observing autocovariance of burst durations. */
    if (NUM_TIME >= FRAMES)
    {
        fprintf(stderr, "\nNumber of observations higher than number of samples.\n");
        fprintf(stderr, "Truncating to number of samples.\n\n");
        NUM_TIME = FRAMES - 1;
    }

    /* Para cada intervalo em frames. */
    for (count = 0; count <= NUM_TIME; count += counts_btw_points)
    {
        var       = 0.0;
        Xo_Xn     = 0.0;
        acc_Xo_Xn = 0.0;

        /* Para cada diferenca de count frames. */
        for (ind1 = 0, ind2 = count; ind2 < FRAMES; ind1++, ind2++)
        {
             Xo_Xn += (frames[ind1]/MODE - acc_X) * (frames[ind2]/MODE
- acc_X);
             var += (frames[ind1]/MODE - acc_X) * (frames[ind1]/MODE -
acc_X);
             acc_Xo_Xn += 1.0;
        }

        autocov = (Xo_Xn/acc_Xo_Xn);
        var     = (var/acc_Xo_Xn);

        /* Using autocov[0] as variance value. Seems to be better. */
        if (count == 0)
            var = autocov;

        time = count * (INTERVAL);
	    if (OPTION == '0')
            fprintf(corr,"%.6e %.10e\n", time, (autocov/var));
	    if (OPTION == '6')
            fprintf(corr,"%d %.10e %.6e\n", (int)time, (autocov/var), TRIGGER);
	    if (OPTION == '1')
            fprintf(cov,"%.6e %.10e\n", time, autocov);
	    if (OPTION == '7')
	        fprintf(cov,"%d %.10e %.6e\n", (int)time, autocov, TRIGGER);
    }

    if ((OPTION == '0') || (OPTION == '6'))
    {
        fprintf(corr, "\n");
        fclose(corr);
    }
    else
    {
        fprintf(cov, "\n");
        fclose(cov);
    }
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida o indice de dispersao para cada    */
/* intervalo de tempo.                                                        */
/******************************************************************************/
void med_idc(MOVIE, NUM_TIME, num_amostras, amostras, frames, INTERVAL, TYPE,
                                                           counts_btw_points)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII) */
int    NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL). */
int    num_amostras;      /* Numero de sample-paths. */
int    *amostras;         /* Vetor de indices dos inicios dos sample-paths. */
double *frames;           /* Ponteiro para a sequencia de amostras. */
double INTERVAL;          /* Intervalo entre as amostras (seg.). */
char   TYPE;              /* Tipo de entrada */
int    counts_btw_points; /* Amostras entre pontos consecutivos de observacao */
{
    FILE   *idc;
    char   out[1000];
    double acc1, acc2, acc;
    int    count, ind, j;
    double time, var, indfc;
    int    cont_amostras;



    /* Arquivo de saida. */
    strcpy(out, MOVIE);
    strcat(out,".seq_idc");
    if ((idc = fopen(out,"w")) == NULL)
    {   
        fprintf(stderr,"Trace IDC: Error while opening file %s\n", out);
        exit(31);
    }

    /* acc1 = Media do numero de bits para i frames. */
    /* acc2 = Seg. momento do numero de bits para i frames. */

    /* Primeira linha do arquivo de saida. */
    fprintf(idc, "0.0 0.0 0.0 0.0 0.0\n");

    /* Para cada tempo (estipulado em numero de frames). */
    for (count = counts_btw_points; count < NUM_TIME + 1; count += counts_btw_points)
    {
        acc1 = 0.0;
        acc2 = 0.0;

        /* Para cada sample-path estipulado em "amostras". */
        for (ind = 0; ind < num_amostras; ind++)
        {
            cont_amostras = (int)amostras[ind];
            acc = 0.0;

            /* Acumula as unidades dos "count" frames. */
            for (j = 0; j < count; j++)
            {
                /* Numero de unidades que chegaram. */
                if ((TYPE == 'r') || (TYPE == 'R'))
                    acc += (frames[cont_amostras + j] * INTERVAL);
                else
                    acc += frames[cont_amostras + j];
            }

            /* Acumula p/ calculo da media. */
            acc1 += (acc/num_amostras);

            /* Acumula p/ calculo do segundo momento. */
            acc2 += ((acc * acc)/num_amostras);         
        }
 
        /* Respectivo intervalo de tempo. */
        time = count * (INTERVAL);
        
        /* Variancia. */
        var = acc2 - (acc1 * acc1);
        
        /* IDC. */
        indfc = var/acc1;

        fprintf(idc, "%f %.10e %.10e %.10e %.10e\n",
                       time, indfc, var, acc1, acc2); 
    }

    fclose(idc);
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida a distribuicao do tempo total de   */
/* rajada.                                                                    */
/******************************************************************************/
void med_tot_burst(MOVIE, NUM_TIME, num_amostras, amostras, frames, INTERVAL, 
                                  TRIGGER, counts_btw_points, FRAMES, OPTION)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII)            */
int    NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL).     */
int    num_amostras;      /* Numero de sample-paths.   */
int    *amostras;         /* Vetor contendo o indice do inicio dos sample-paths. */
double *frames;           /* Ponteiro para a sequencia de amostras. */
double INTERVAL;          /* Intervalo entre as amostras (seg.).    */
double TRIGGER;           /* Nivel minimo para rajada. */
int    counts_btw_points; /* Amostras entre os pontos consecutivos de observacao */
int    FRAMES;            /* Numero de amostras da sequencia.                    */
char   OPTION;            /* Selecao de descritor      */
{
    FILE   *raj, *raj_2;
    char   out[1000], trigger_str[300];
    double acc_burst;
    int    accraj;
    int    count, ind, j;
    double time;
    double *cumt;
    int    cont_amostras;
    double acc_tot_burst, var_tot_burst;
    double min_burst, max_burst;
    int    ind_burst_time;
    double *burst_time_vec;
    int    Num_Itens;

    burst_time_vec = NULL;
    raj = raj_2    = NULL;
    cumt           = NULL;
    /* Arquivo de saida da distribuicao. */
    if (OPTION == '4')
    {
        strcpy(out, MOVIE);
        strcat(out, ".seq_total_burst");
        sprintf(trigger_str, "_%.6e", TRIGGER);
        strcat(out, trigger_str);
        if ((raj = fopen(out,"w")) == NULL)
        {
            fprintf(stderr,"Trace Total Time Dist: Error while opening file %s\n", out);
            exit(31);
        }
    }


    /* Arquivo de saida da esperanca. */
    if (OPTION == '8')
    {
        strcpy(out, MOVIE);
        strcat(out, ".seq_expected_total_burst");
        sprintf(trigger_str, "_%.6e", TRIGGER);
        strcat(out, trigger_str);
        if ((raj_2 = fopen(out,"w")) == NULL)
        {
            fprintf(stderr,"Trace Total Time Expectation: Error while opening file %s\n", out);
            exit(31);
        }
    }


    if (OPTION == '4')
    {
        /* Alocando vetor */
        cumt = (double *) malloc((NUM_TIME + 1) * sizeof(double));

        /* Inicializacao */
        for (count = 0; count < (NUM_TIME + 1); count++)
            cumt[count] = 0.0;
    }

    if (OPTION == '8')
    {
        /* Frames/2 is the maximum number of possible bursts in a sequence */
        Num_Itens = FRAMES/2;
        burst_time_vec = (double *) calloc(Num_Itens, sizeof(double));
    }


    ind_burst_time = 0;
    for (ind = 0; ind < num_amostras; ind++)
    {
        cont_amostras = amostras[ind];
        accraj = 0;

        for (j = 0; j < NUM_TIME; j++)
        {
            /* Acumula se for rajada. */
            if (frames[cont_amostras + j] > TRIGGER)
                accraj++;
        }


        if (OPTION == '4')
            /* Computa a frequencia das rajadas. */
            cumt[accraj] += 1.0;

        if (OPTION == '8')
            /* Monta trace com os tempos de rajadas */
            burst_time_vec[ind_burst_time++] = (accraj * INTERVAL);
    }


    if (OPTION == '4')
    {
        acc_burst = 0.0;
        fprintf(raj, "# Maximum value below burst: %.6e\n", TRIGGER);
        fprintf(raj, "# Observed time: %.6e\n", (double)((double)NUM_TIME * INTERVAL));
        fprintf(raj, "# Number of sample-paths: %d\n", num_amostras);
        for (count = 0; count < (NUM_TIME + 1); count++)
        {
            time = count * INTERVAL;
            acc_burst += (cumt[count]/num_amostras);

            if ((count % counts_btw_points) == 0)
                fprintf(raj, "%.6e %.10e %.6e\n", time, acc_burst, TRIGGER);
        }
        fprintf(raj, "\n");

        free(cumt);

        fclose(raj);
    }


    if (OPTION == '8')
    {
        steady_descriptors(out, num_amostras, burst_time_vec, &acc_tot_burst, &var_tot_burst,
                                          (double)1.0, '8', &min_burst, &max_burst, TRIGGER);

        fprintf(raj_2, "# Observed time: %.6e\n", (double)((double)NUM_TIME * INTERVAL));
        fprintf(raj_2, "# Number of sample-paths: %d\n", num_amostras);
        fprintf(raj_2, "# Maximum value below burst x Expected Total Burst x Var. of Total Burst:\n");
        fprintf(raj_2, "%.6e %.10e %.10e\n", TRIGGER, acc_tot_burst, var_tot_burst);

        fclose(raj_2);
    }
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida a distribuicao do tempo total de   */
/* rajada.                                                                    */
/******************************************************************************/
void med_tot_burst_dist(MOVIE, NUM_TIME, num_amostras, amostras, frames,
                           INTERVAL, TRIGGER, counts_btw_points, FRAMES)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII)            */
int    NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL).     */
int    num_amostras;      /* Numero de sample-paths.   */
int    *amostras;         /* Vetor contendo o indice do inicio dos sample-paths. */
double *frames;           /* Ponteiro para a sequencia de amostras. */
double INTERVAL;          /* Intervalo entre as amostras (seg.).    */
double TRIGGER;           /* Nivel minimo para rajada. */
int    counts_btw_points; /* Amostras entre os pontos consecutivos de observacao */
int    FRAMES;            /* Numero de amostras da sequencia.                    */
{
    FILE   *raj;
    char   out[1000], trigger_str[300];
    double acc_burst;
    int    accraj;
    int    count, ind, j;
    double time;
    double *cumt;
    int    cont_amostras;
    int    ind_burst_time;


    /* Initializations */
    raj            = NULL;
    cumt           = NULL;


    /* Opening output file. */
    strcpy(out, MOVIE);
    strcat(out, ".seq_total_burst");
    sprintf(trigger_str, "_%.6e", TRIGGER);
    strcat(out, trigger_str);

    if ((raj = fopen(out,"w")) == NULL)
    {
        fprintf(stderr,"Trace Total Time Dist: Error while opening file %s\n", out);
        exit(31);
    }


    /* Allocating memory */
    cumt = (double *) malloc((NUM_TIME + 1) * sizeof(double));

    /* Initializing */
    for (count = 0; count < (NUM_TIME + 1); count++)
         cumt[count] = 0.0;


    /* Looping through the sample-paths */
    ind_burst_time = 0;
    for (ind = 0; ind < num_amostras; ind++)
    {
        cont_amostras = amostras[ind];
        accraj = 0;

        for (j = 0; j < NUM_TIME; j++)
        {
            /* If it's a burst, accumulate. */
            if (frames[cont_amostras + j] > TRIGGER)
                accraj++;
        }


        /* Computes frequency of bursts. */
        cumt[accraj] += 1.0;
    }


    acc_burst = 0.0;
    fprintf(raj, "# Maximum value below burst: %.6e\n", TRIGGER);
    fprintf(raj, "# Observed time: %.6e\n", (double)((double)NUM_TIME * INTERVAL));
    fprintf(raj, "# Number of sample-paths: %d\n", num_amostras);
    for (count = 0; count < (NUM_TIME + 1); count++)
    {
        time = count * INTERVAL;
        acc_burst += (cumt[count]/num_amostras);

        if ((count % counts_btw_points) == 0)
            fprintf(raj, "%.6e %.10e %.6e\n", time, acc_burst, TRIGGER);
    }
    fprintf(raj, "\n");


    /* Releasing vector */
    free(cumt);

    /* Closing output file */
    fclose(raj);
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida a distribuicao do tempo total de   */
/* rajada.                                                                    */
/******************************************************************************/
void med_tot_burst_expect(MOVIE, NUM_TIME, num_amostras, amostras, frames,
                             INTERVAL, TRIGGER, counts_btw_points, FRAMES)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII)            */
int    NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL).     */
int    num_amostras;      /* Numero de sample-paths.   */
int    *amostras;         /* Vetor contendo o indice do inicio dos sample-paths. */
double *frames;           /* Ponteiro para a sequencia de amostras. */
double INTERVAL;          /* Intervalo entre as amostras (seg.).    */
double TRIGGER;           /* Nivel minimo para rajada. */
int    counts_btw_points; /* Amostras entre os pontos consecutivos de observacao */
int    FRAMES;            /* Numero de amostras da sequencia.                    */
{
    FILE   *raj_2;
    char   out[1000], trigger_str[300];
    int    accraj;
    int    ind, j;
    int    cont_amostras;
    double acc_tot_burst, var_tot_burst;
    double min_burst, max_burst;
    int    ind_burst_time;
    double *burst_time_vec;
    int    Num_Itens;


    /* Initializations */
    burst_time_vec = NULL;
    raj_2          = NULL;


    /* Opening output file. */
    strcpy(out, MOVIE);
    strcat(out, ".seq_expected_total_burst");
    sprintf(trigger_str, "_%.6e", TRIGGER);
    strcat(out, trigger_str);
    if ((raj_2 = fopen(out,"w")) == NULL)
    {
        fprintf(stderr,"Trace Total Time Expectation: Error while opening file %s\n", out);
        exit(31);
    }


    /* Allocating vectors */
    /* Frames/2 is the maximum number of possible bursts in a sequence */
    Num_Itens = FRAMES/2;
    burst_time_vec = (double *) calloc(Num_Itens, sizeof(double));


    /* Looping through the sample paths */
    ind_burst_time = 0;
    for (ind = 0; ind < num_amostras; ind++)
    {
        cont_amostras = amostras[ind];
        accraj = 0;

        for (j = 0; j < NUM_TIME; j++)
        {
            /* Acumulates if it's a burst. */
            if (frames[cont_amostras + j] > TRIGGER)
                accraj++;
        }

        /* Fill a vector with burst's duration */
        burst_time_vec[ind_burst_time++] = (accraj * INTERVAL);
    }


    steady_descriptors(out, num_amostras, burst_time_vec, &acc_tot_burst, &var_tot_burst,
                                      (double)1.0, '8', &min_burst, &max_burst, TRIGGER);

    /* Printing output file */
    fprintf(raj_2, "# Observed time: %.6e\n", (double)((double)NUM_TIME * INTERVAL));
    fprintf(raj_2, "# Number of sample-paths: %d\n", num_amostras);
    fprintf(raj_2, "# Maximum value below burst x Expected Total Burst x Var. of Total Burst:\n");
    fprintf(raj_2, "%.6e %.10e %.10e\n", TRIGGER, acc_tot_burst, var_tot_burst);

    /* Closing output file */
    fclose(raj_2);

    /* Releasing memory */
    free(burst_time_vec);
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida a distribuicao complementar da     */
/* duracao da rajada. Calcula tambem a autocovariancia/autocorrelacao das     */
/* duracoes de rajadas e joga a saida para um arquivo.                        */
/******************************************************************************/
void med_burst_dist(MOVIE, NUM_TIME, frames, INTERVAL, TRIGGER, LEVELS, FRAMES, min,
                      step, MODE, counts_btw_points, POINTS, amostras, num_amostras)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII)         */
int    NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL).  */
double *frames;           /* Ponteiro para a sequencia de amostras.   */
double INTERVAL;          /* Intervalo entre as amostras (seg.).      */
double TRIGGER;           /* Nivel minimo para rajada.                */
int    LEVELS;            /* Numero de estados da cadeia do modelo.   */
int    FRAMES;            /* Numero de amostras da sequencia.         */
double min;               /* Amostra de menor valor. */
double step;              /* Diferenca entre os niveis de recompensa. */
double MODE;              /* Modo de entrada         */
int    counts_btw_points; /* Amostras entre pontos consecutivos de observacao. */
int    POINTS;            /* Numero de pontos para autocorrelacao/covariancia. */
int    *amostras;         /* Vetor contendo o indice do inicio dos sample-paths. */
int    num_amostras;      /* Numero de sample-paths.   */
{
    FILE   *bur;
    char   out[1000], trigger_str[300];
    double acc_burst, var_burst, min_burst, max_burst;
    int    accraj;
    int    count, ind, i, j, ind_burst_time;
    double time;
    double **cumt;
    int    cont_amostras, base_amostras;
    double *aux;


    /* Initializations */
    bur            = NULL;
    cumt           = NULL;
    aux            = NULL;


    /* Name for output file. If OPTION = 6 or 7, name prefix. */
    strcpy(out, MOVIE);
    strcat(out,".seq_burst");


    /* Opening file for Burst duration complementary distribution */
    sprintf(trigger_str,"_%.6e",TRIGGER);
    strcat(out,trigger_str);
    if ((bur = fopen(out,"w")) == NULL)
    {
       fprintf(stderr,"Trace Burst compl. Dist.: Error while opening file: %s\n", out);
       exit(31);
    }


    /* Allocating memory */
    if ((cumt = (double **) malloc(num_amostras * sizeof(double *))) == NULL)
    {
        fprintf(stderr,"Trace Burst compl. Dist.: Cannot allocate memory.\n");
        exit(40);
    }

    for (i = 0; i < num_amostras; i++)
        if ((cumt[i] = (double *) malloc((NUM_TIME + 1) * sizeof(double))) == NULL)
        {
           fprintf(stderr,"Trace Burst compl. Dist.: Cannot allocate memory.\n");
           exit(40);
        }

    if  ((aux = (double *) malloc(num_amostras * sizeof(double))) == NULL)
    {
        fprintf(stderr,"Trace Burst compl. Dist.: Cannot allocate memory.\n");
        exit(40);
    }


    /* Looping through the sample-paths */
    for (i = 0; i < num_amostras; i++)
    {
      base_amostras = amostras[i];

      /* Initializing variables */
      cont_amostras = 0;
      ind_burst_time = 0;

      for (j = 0; j < NUM_TIME; )
      {
        accraj = 0;

        do
        {
            if (frames[base_amostras + j] > TRIGGER)
                accraj++;

            j++;   /* Conta os frames */
        }
        while ((frames[base_amostras + j] > TRIGGER) && ((base_amostras + j) < (base_amostras + NUM_TIME)));

        if (accraj)
        {
            /* Computa distribuicao de probabilidade. */
            /* Nao conta quando nao ha burst e quando se passou de Tmax. */
            /* Trunca em Tmax. */
            if (accraj > NUM_TIME)
                accraj = NUM_TIME + 1;

            for (ind = 1; ind < accraj; ind++)
                cumt[i][ind] += 1.0;

            cont_amostras++;
        }
      } /* j, through ONE sample-path */

      #ifdef DEBUG
      printf("Numero de rajadas: %d\n", cont_amostras);
      #endif

      cumt[i][0] = cont_amostras;

    } /* i, through ALL sample-paths */


    for (i = 0; i < num_amostras; i++)
        for (count = 1; count <= NUM_TIME; count++)
        {
            cumt[i][count] /= cumt[i][0];

            #ifdef DEBUG
            fprintf(stderr,"Cumt[%d,%d] = %.6e\n", i, count, cumt[i][count]);
            #endif
        }


    /* Printing output file */
    fprintf(bur, "# Maximum value below burst: %.6e\n", TRIGGER);
    fprintf(bur, "# Number of sample-paths: %d\n", num_amostras);
    fprintf(bur, "0.000000 1.000000000 0.0000000000 %.6e\n", TRIGGER);
    for (count = 1; count <= NUM_TIME; count++)
    {
        /* Fills a vector with the probabilities computed for each sample-path */
        if ((count % counts_btw_points) == 0)
            for (i = 0; i < num_amostras; i++)
            {
                aux[i] = cumt[i][count];

                #ifdef DEBUG
                fprintf(stderr,"Time: %d  aux[%d] = %.6e\n", count, i, aux[i]);
                #endif
            }

        steady_descriptors(out, num_amostras, aux, &acc_burst, &var_burst, (double)1.0,
                                                 '5', &min_burst, &max_burst, TRIGGER);

        time = count * INTERVAL;

        /* Only prints desired points */
        if ((count % counts_btw_points) == 0)
           fprintf(bur, "%.6e %.10e %.10e %.6e\n", time, acc_burst, var_burst, TRIGGER);
    }
    fprintf(bur, "\n");


    /* Releasing memory */
    for (i = 0; i < num_amostras; i++)
        free((double *)(cumt[i]));

    free((double **)cumt);
    free(aux);

    /* Closing output file */
    fclose(bur);
}


/******************************************************************************/
/* Calcula e joga para um arquivo de saida a distribuicao complementar da     */
/* duracao da rajada. Calcula tambem a autocovariancia/autocorrelacao das     */
/* duracoes de rajadas e joga a saida para um arquivo.                        */
/******************************************************************************/
void med_burst_expect(MOVIE, NUM_TIME, frames, INTERVAL, TRIGGER, LEVELS, FRAMES, min,
                        step, MODE, counts_btw_points, POINTS, amostras, num_amostras)
char   *MOVIE;            /* Nome do arquivo com a sequencia. (ASCII)         */
int    NUM_TIME;          /* Numero de amostras em LIMITE (LIMITE/INTERVAL).  */
double *frames;           /* Ponteiro para a sequencia de amostras.   */
double INTERVAL;          /* Intervalo entre as amostras (seg.).      */
double TRIGGER;           /* Nivel minimo para rajada.                */
int    LEVELS;            /* Numero de estados da cadeia do modelo.   */
int    FRAMES;            /* Numero de amostras da sequencia.         */
double min;               /* Amostra de menor valor. */
double step;              /* Diferenca entre os niveis de recompensa. */
double MODE;              /* Modo de entrada         */
int    counts_btw_points; /* Amostras entre pontos consecutivos de observacao. */
int    POINTS;            /* Numero de pontos para autocorrelacao/covariancia. */
int    *amostras;         /* Vetor contendo o indice do inicio dos sample-paths. */
int    num_amostras;      /* Numero de sample-paths.   */
{
    FILE   *bur_2;
    char   out[1000], out_2[1000], trigger_str[300];
    double *burst_time_vec;
    double acc_burst, var_burst, min_burst, max_burst;
    int    accraj;
    int    i, j, ind_burst_time;
    int    cont_amostras, base_amostras;
    int    Num_Itens;
    double *expectations;


    /* Initializations */
    bur_2          = NULL;
    burst_time_vec = NULL;
    expectations   = NULL;


    /* Name for output file of expectation. */
    strcpy(out_2, MOVIE);
    strcat(out_2,".seq_expected_burst");
    sprintf(trigger_str,"_%.6e",TRIGGER);
    strcat(out_2,trigger_str);

    /* Openning output file */
    if ((bur_2 = fopen(out_2,"w")) == NULL)
    {
        fprintf(stderr,"Trace Expected Burst Duration: Error while opening file: %s\n", out_2);
        exit(31);
    }


    /* Frames/2 is the maximum number of possible bursts in a sequence */
    Num_Itens = FRAMES/2;
    burst_time_vec = (double *) malloc(Num_Itens * sizeof(double));
    expectations = (double *) malloc(num_amostras * sizeof(double));


    /* Looping through the sample-paths */
    for (i = 0; i < num_amostras; i++)
    {
      base_amostras = amostras[i];

      /* Initializing variables */
      cont_amostras = 0;
      ind_burst_time = 0;

      for (j = 0; j < NUM_TIME; )
      {
        accraj = 0;

        do
        {
            if (frames[base_amostras + j] > TRIGGER)
                accraj++;

            j++;   /* Conta os frames */
        }
        while ((frames[base_amostras + j] > TRIGGER) && ((base_amostras + j) < (base_amostras + NUM_TIME)));

        if (accraj)
        {
            /* Monta trace com os tempos de rajadas */
            burst_time_vec[ind_burst_time++] = (accraj * INTERVAL);

            cont_amostras++;
        }
      } /* j, through ONE sample-path */


      #ifdef DEBUG
      printf("Numero de rajadas: %d\n", cont_amostras);
      /*** Teste ***/
      for (ind_burst_time = 0; ind_burst_time < cont_amostras; ind_burst_time++)
          printf("burst_time_vec[%d] = %f\n", ind_burst_time, burst_time_vec[ind_burst_time]);
      printf("*****\n");
      /*************/
      #endif


      steady_descriptors(out, cont_amostras, burst_time_vec, &acc_burst, &var_burst,
                              (double)1.0, '9', &min_burst, &max_burst, TRIGGER);

      expectations[i] = acc_burst;

    } /* i, through ALL sample-paths */


    steady_descriptors(out, num_amostras, expectations, &acc_burst, &var_burst,
                            (double)1.0, '9', &min_burst, &max_burst, TRIGGER);

    fprintf(bur_2, "# Number of sample-paths: %d\n", num_amostras);
    fprintf(bur_2, "# Maximum non burst level x Expected Burst Duration x Var. of Burst Duration:\n");
    fprintf(bur_2, "%.6e %.10e %.10e\n", TRIGGER, acc_burst, var_burst);


    #ifdef DEBUG
    /*** Teste ***/
    printf("E[burst_duration] = %f  Var[burst_duration] = %f\n*****\n", acc_burst, var_burst);
    #endif


    fclose(bur_2);

    free(burst_time_vec);
    free(expectations);
}


/******************************************************************************/
/* Calcula vetor de probabilidades iniciais                                   */
/******************************************************************************/
void init_prob(num_amostras, amostras, frames, min, step, LEVELS, MODE)
int    num_amostras; /* Numero de sample-paths. */
int    *amostras;    /* Vetor contendo o indice do inicio dos sample-paths. */
double *frames;      /* Ponteiro para a sequencia de amostras. */
double min;          /* Amostra de menor valor. */
double step;         /* Diferenca entre os niveis de recompensa. */
int    LEVELS;       /* Numero de estados da cadeia do modelo. */
double MODE;         /* Modo de entrada */
{
    int    count, ind_pi1, num;
    double soma;
    double *pi1;
    
    
    pi1 = (double *) malloc(LEVELS * sizeof(double));
    
    /* Vetor de probabilidades iniciais. */
    for (count = 0; count < LEVELS; count++)
        pi1[count] = 0.0;
    
    /* Calcula frequencia das primeiras amostras de cada sample-path. */
    num = 0;
    for (count = 0; count < num_amostras; count++)
    {
        /* Dependendo do valor, calcula o indice do vetor. */
        ind_pi1 = ((frames[amostras[count]]) - min)/step;
        
        if (ind_pi1 == LEVELS)
            ind_pi1 = LEVELS - 1;
        
        pi1[ind_pi1] += 1.0;
        num++;
    }
    
    /* Normaliza e averigua se a soma das prob. da' 1. */
    soma = 0;
    for (count = 0; count < LEVELS; count++)
    {
        pi1[count] /= num;         
        soma += pi1[count];
    }
    /** printf("soma= %lf\n",soma); */
    
    /* Escrevendo vetor de probabilidades iniciais. */
    printf("Associated initial probability vector (IDC and P[TR(T) < t]):\n");
    printf("%d ", LEVELS);
    for (count = 0; count < LEVELS; count++)
        printf("%.10f ", pi1[(LEVELS - 1) - count]);
    printf("\n*****\n");
    
    free(pi1);
}
