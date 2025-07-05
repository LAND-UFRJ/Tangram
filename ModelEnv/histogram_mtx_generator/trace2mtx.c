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

/* Este programa constroi um modelo para uma sequencia de                   */
/* de video qualquer. E' necessario um arquivo que contenha o               */
/* tamanho, ou taxa, (celulas, bits ou bytes) de cada unidade de informacao */
/* (frames, GOBS, stripes, etc.) separados por "\n". O programa fornece:    */
/* - vetor de recompensas (taxas),                                          */
/* - matriz de probabilidades de transicao,                                 */
/* - media e variancia (taxas).                                             */
/* O programa tambem calcula o histograma baseado nas recompensas.          */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

                                
/* A ser usado pela funcao do Edmundo.       */
/* Tem que multiplicar o resultado retornado */
/* por simulrandom por fator_normal.         */

#define fator_normal (4.656612875e-10)

/* Fornece uma variavel de distribuicao exponencial */
double poisson_time();

/* Fornece uma variavel de distribuicao uniforme */
double simulrandom();

/* Explica a linha de comando do programa */
void usage();


int main(argc,argv)
int  argc;
char **argv;
{
    FILE   *movie, *prb, *mc, *mc_2;
    double **P;
    double *reward;
    int    *rwd_count;
    double *rwd_prb;
    int    ind_pre, ind_pos, ind;
    double step;
    double *frames;
    int    count;
    double min_rate, min;
    double max_rate, max;
    double rate, aux_rate;
    char   rate_str[20], out[36];
    double acc_X = 0.0, var_X = 0.0;
    int    i, j;
    double shot1 = 0.0, shot2 = 0.0;
    int    count_poisson, total;
    char   name[100];

    /*** Parametros: ***/
    int    LEVELS;      /* Numero de estados da cadeia do modelo. */
    double LAMBDA;      /* Taxa de uniformizacao. */ 
    char   MOVIE[100];  /* Nome do arquivo com a sequencia. (ASCII) */
    int    FRAMES;      /* Numero de amostras da sequencia. */ 
    double INTERVAL;    /* Intervalo entre as amostras (seg.). */
    double MODE;        /* Assume INTERVAL ou 1.0, dependendo de TYPE */
    char   TYPE;        /* Tipo de entrada: r = rate, s = size */
    double DIVISOR;     /* Para se evitar overflow. */
    double DATA_UNIT;    /* Unidade de dados expressa em bytes */


    /* Defaults: */
    LEVELS    = 8;
    LAMBDA    = 1.0;
    strcpy(MOVIE,"");
    FRAMES    = 100;
    INTERVAL  = 1.0;
    TYPE      = 's';
    DIVISOR   = 1.0;
    DATA_UNIT = 1.0;
    min_rate  = max_rate =0;

    /* Aquisitando parametros: */
    for (i = 1, j = 0; i < argc; i++, j = 0)
    {
        if (argv[i][0] == '-')
            j++;

        /* printf("%s %c %s\n",argv[i],argv[i][j],&(argv[i][j+2])); */

        switch(argv[i][j])
        {
          case 't' :
          case 'T' : TYPE = argv[i][j + 1];
                     break;
          case 'a' :
          case 'A' : strcpy(MOVIE,(char *)&(argv[i][j + 1]));
                     break;
          case 'n' :
          case 'N' : FRAMES = atoi((char *)&(argv[i][j + 1]));
                     break;
          case 'i' :
          case 'I' : INTERVAL = atof((char *)&(argv[i][j + 1]));
                     break;
          case 'u' :
          case 'U' : LAMBDA = atof((char *)&(argv[i][j + 1]));
                     break;
          case 'd' :
          case 'D' : DIVISOR = atof((char *)&(argv[i][j + 1]));
                     break;
          case 'm' :
          case 'M' : LEVELS = atoi((char *)&(argv[i][j + 1]));
                     break;
          case 'b' :
          case 'B' : DATA_UNIT = atof((char *)&(argv[i][j + 1]));
                     break;
          case 'h' : 
          case 'H' : usage();
                     exit(0);
        }
    }

    printf("\nt= %c\na= %s\nn= %d\ni= %.10f\nu= %.10f\nd= %.10f\nm= %d\n",
                     TYPE, MOVIE, FRAMES, INTERVAL, LAMBDA, DIVISOR, LEVELS);
    printf("*****\n");


    if ((TYPE == 's') || (TYPE == 'S'))     /* Arq. contem tamanho dos frames */
        MODE = INTERVAL;
    else    
        if ((TYPE == 'r') || (TYPE == 'R')) /* Arq. contem taxa inst. dos frames */
            MODE = 1.0;
        else
        {
            printf("Erro na especificacao do modo de entrada. (t = %c)\n", TYPE);
            exit(1);
        }
    

    /* Alocando memoria: */
    P = (double **) malloc(LEVELS * sizeof(double));
    for (i = 0; i < LEVELS; i++)
         P[i] = (double *) malloc(LEVELS * sizeof(double));
    reward = (double *) malloc(LEVELS * sizeof(double));
    rwd_count = (int *) malloc(LEVELS * sizeof(double));
    rwd_prb = (double *) malloc(LEVELS * sizeof(double));
    frames = (double *) malloc(FRAMES * sizeof(double));

    if (!strcmp(MOVIE,""))
    {
        printf("\nInput file name: ");
        scanf("%s",MOVIE);
    }

    /* Leio o arquivo de entrada. Apesar de chamar "rate_str", esta variavel */
    /* contera' o tamanho da amostra.                                        */
    if ((movie = fopen(MOVIE,"r")) == NULL)
    {   
        printf("Erro ao abrir o arquivo!\n");
        exit(1);
    }

    for (count = 0; count < FRAMES; count++)
    {
        fscanf(movie,"%s", rate_str);

        frames[count] = (atof(rate_str))/DIVISOR;
    }

    fclose(movie);
    /***/


    /* Calcula a taxa maxima, minima e media. Calcula a variancia. */
    acc_X = 0.0;
    for (count = 0; count < FRAMES; count++)
    {
        rate = frames[count]/MODE;

        if (count == 0)
            min_rate = max_rate = rate;
        else
        {
            if (rate < min_rate)
                min_rate = rate;

            if (rate > max_rate)
                max_rate = rate;
        }

        acc_X = acc_X + (rate)/(FRAMES);
    }

    var_X = 0.0;
    for (count = 0; count < FRAMES; count++)
        var_X = var_X + 
                        (((frames[count]/MODE) - acc_X) * 
                         ((frames[count]/MODE) - acc_X))/(FRAMES - 1);

    printf ("Min: %.10f  Max: %.10f\n", min_rate, max_rate);
    printf ("E[rate]= %.6e\n", acc_X);
    printf ("Var[rate]= %.6e\n", var_X);
    /***/
    

    /*** Calculando parametros da cadeia ***/

   
    /* Vetor de recompensas */
    step = (max_rate - min_rate)/LEVELS;
    aux_rate = min_rate + (step/2.0);
    for (count = 0; count < LEVELS; count++)
    {
        reward[count] = aux_rate;
        aux_rate += step;
    }


    /*** Matriz de probabilidades de transicao ***/
    
    /* Inicilalizando */
    for (ind_pre = 0; ind_pre < LEVELS; ind_pre++)
    {
        for (ind_pos = 0; ind_pos < LEVELS; ind_pos++)
            P[ind_pre][ind_pos] = 0.0;

        rwd_count[ind_pre] = 0;
    }


    /* Deve estar de acordo com frames[i] */
    max = max_rate * MODE;
    min = min_rate * MODE;
    step = (max - min)/LEVELS;


    /* Calculando as frequencias de transicao entre estados, */
    /* segundo dist. exponencial com par. LAMBDA.            */
    shot1 = shot2 = 0.0;
    do
    {
        shot2 += poisson_time(LAMBDA);

        count_poisson = shot1/INTERVAL;

        ind_pre = (frames[count_poisson] - min)/step;
        if (ind_pre == LEVELS)
            ind_pre = LEVELS - 1;

        count_poisson = shot2/INTERVAL;

        /* Estourou a sequencia. */
        if (count_poisson > FRAMES - 1)
            break;

        ind_pos = (frames[count_poisson] - min)/step;
        if (ind_pos == LEVELS)
            ind_pos = LEVELS - 1;

        P[ind_pre][ind_pos] += 1.0;
        rwd_count[ind_pre]++;

        shot1 = shot2;
    }
    while (shot2 < (FRAMES * INTERVAL));


    /* Normalizando. */
    for (ind_pre = 0; ind_pre < LEVELS; ind_pre++)
        for (ind_pos = 0; ind_pos < LEVELS; ind_pos++)
        {
            if (rwd_count[ind_pre])
                P[ind_pre][ind_pos] /= rwd_count[ind_pre];
        }



    /* Abrindo arquivo de saida para gerador de trafego. */
    strcpy(name,MOVIE);
    strcat(name,".markov_mtx");
    if ((mc = fopen(name,"w")) == NULL)
    {
        printf("Erro ao abrir o arquivo de saida da cadeia p/ gerador de trafego!\n");
        exit(1);
    }

    /* Abrindo arquivo de saida para conversor p/ Tangram. */
    strcpy(name,MOVIE);
    strcat(name,".conv");    
    if ((mc_2 = fopen(name,"w")) == NULL)
    {
        printf("Erro ao abrir o arquivo de saida da cadeia p/ conversor!\n");
        exit(1);
    }


    /* Arquivos sao escritos de forma que as taxas mais altas */
    /* possuam menores indices.                               */


    /* Escrevendo tipo de arquivo */
    fprintf(mc, "MARKOV\n");
    
    
    /* Escrevendo unidade em bytes */
    fprintf(mc, "%.6f\n", DATA_UNIT);
    

    /* Escrevendo vetor de recompensas. */
    fprintf(mc, "%d \n", LEVELS);
    for (count = 0; count < LEVELS; count++)
        fprintf(mc, "%.10f \n", reward[(LEVELS - 1) - count]);


    /* Escrevendo cadeia no arquivo de saida. */
    for (ind_pre = 0; ind_pre < LEVELS; ind_pre++)
        for (ind_pos = 0; ind_pos < LEVELS; ind_pos++)
            fprintf(mc, "%d %d %.10f\n", (LEVELS - ind_pre), (LEVELS - ind_pos), 
                                                 (P[ind_pre][ind_pos]));
                                          
                                          
    
    /* Escrevendo vetor de recompensas. */
    fprintf(mc_2, "%d \n", LEVELS);
    for (count = 0; count < LEVELS; count++)
        fprintf(mc_2, "%d %.10f \n", (LEVELS - count), 
                              reward[count]);


    /* Escrevendo cadeia no arquivo de saida. */
    fprintf(mc_2, "%f \n", LAMBDA);
    for (ind_pre = 0; ind_pre < LEVELS; ind_pre++)
        for (ind_pos = 0; ind_pos < LEVELS; ind_pos++)
            fprintf(mc_2, "%d %d %.10f\n", (LEVELS - ind_pre), (LEVELS - ind_pos), 
                                                              P[ind_pre][ind_pos]);
                                          


    fclose(mc);
    fclose(mc_2);
    /***/


    /* Computando ultimo frame p/ calculo do histograma */
    ind_pre = (frames[FRAMES] - min_rate)/step;
    if (ind_pre == LEVELS)
        ind_pre = LEVELS - 1;
    rwd_count[ind_pre]++;
    /***/


    /* Calculando histograma da sequencia segundo os estados atribuidos. */
    strcpy(out, MOVIE);
    strcat(out,".prb");

    if ((prb = fopen(out,"w")) == NULL)
    {   
        printf("Erro ao abrir o arquivo .cov!\n");
        exit(1);
    }

    for (ind = 0, total = 0; ind < LEVELS; ind++)
        total += rwd_count[ind];

    for (ind_pre = 0; ind_pre < LEVELS; ind_pre++)
    {
        rwd_prb[ind_pre] = rwd_count[ind_pre];
        rwd_prb[ind_pre] /= total;

        fprintf(prb,"%f %f\n", reward[ind_pre], rwd_prb[ind_pre]);
    }

    fclose(prb);
    /***/
    return( 0 );
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
  printf("Usage: geramodelo [-a=filename] [-n=samples_number] "); 
  printf("[-i=interval_time] [-u=uniformization_rate] [-d=divisor] ");
  printf("[-m=number_of_states] [-t=sample_type] [-b=data_unit_in_bytes]\n\n");
  printf("Upper case is allowed and '-' can be omitted.\n");
}
