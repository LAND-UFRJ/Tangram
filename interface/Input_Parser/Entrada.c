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

#ifdef _SOLARIS_
 typedef long int64_t;
 #include <sys/types.h>
#endif

#include <jni.h>
#include "Entrada.h"


// int get_precision(precision_str)
// char *precision_str;
JNIEXPORT jint JNICALL Java_Tangram2_Entrada_get_1precision(JNIEnv *env, jobject obj, jstring precision_C_str)
{
    double precision_value;
    
    const  char *precision_str;
    
    
    precision_str = (*env)->GetStringUTFChars(env, precision_C_str, 0);
            
    precision_value = atof(precision_str);
    
    if ((precision_value >= 1) || (precision_value <= 0))
    {
        (*env)->ReleaseStringUTFChars(env, precision_C_str, precision_str);
        
        return(-1);             /* Valor absurdo */
    }
    else
    {
        (*env)->ReleaseStringUTFChars(env, precision_C_str, precision_str);
        
        return(0);              /* Valor valido  */
    }
}



// int get_partition(partition_str, basename)
// char *partition_str;
// char *basename;
JNIEXPORT jint JNICALL Java_Tangram2_Entrada_get_1partition(JNIEnv *env, jobject obj, jstring partition_C_str, jstring basename_C)
{
    FILE *f_partition;
    char partition_name[100];
    char number[30];
    int  size, end_state, last_state;
    int  i, j, count;
    
    const  char *partition_str;
    const  char *basename;
    
    
    partition_str = (*env)->GetStringUTFChars(env, partition_C_str, 0);
    basename = (*env)->GetStringUTFChars(env, basename_C, 0);
    
    
    strcpy(partition_name, basename);
    strcat(partition_name, ".partition");
    
    if ((f_partition = fopen(partition_name,"w")) == NULL)
    {
        fprintf(stderr,"Can't create partitions file!\n");
        
        (*env)->ReleaseStringUTFChars(env, partition_C_str, partition_str);
        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
        return(-1);
    }
    
    
    /* Alocando espaco para numero de particoes */
    fprintf(f_partition, "00000000\n");
    
    
    count = 0;
    last_state = 0;
    for (i = 0; partition_str[i] != '\0'; )
    {
        if (isdigit((int)partition_str[i]))
        {
            memset(number, 0, 30);
            j = 0;
            while (isdigit(partition_str[i]))
            {
                number[j] = partition_str[i];
                j++;
                i++;
            }
        
            number[j] = '\0';
            end_state = atoi(number);
            if (end_state <= last_state)
            {
                fprintf(stderr, "States must be in increasing order.\n");
        
                (*env)->ReleaseStringUTFChars(env, partition_C_str, partition_str);
                (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                return(-2);
            }
            fprintf(f_partition, "%d\n", end_state);
            
            count++;
            
            last_state = end_state;
        }
        else 
            if (partition_str[i] == 's')
            {
                /* Posiciona ponteiro na definicao do tam. do bloco */
                while (partition_str[++i] == ' ');
            
                memset(number, 0, 30);
                j = 0;
                while (isdigit(partition_str[i]))
                {
                    number[j] = partition_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                size = atoi(number);
                fprintf(f_partition, "s %d ", size);


                /* Posiciona ponteiro na definicao do final do bloco */
                while (partition_str[++i] == ' ');

                memset(number, 0, 30);
                j = 0;
                while (isdigit(partition_str[i]))
                {
                    number[j] = partition_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                end_state = atoi(number); 
                if (end_state <= last_state)
                {
                    fprintf(stderr, "States must be in increasing order.\n");
        
                    (*env)->ReleaseStringUTFChars(env, partition_C_str, partition_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-2);
                }
                fprintf(f_partition, "%d\n", end_state);
                
                count += ((end_state - last_state)/size);
                
                last_state = end_state;
            }
            else
                if ((partition_str[i] == ' ') || (partition_str[i] == ';')
                                              || (partition_str[i] == ','))
                    i++;
                else
                {
                    fprintf(stderr,"Invalid character at partition description.\n");
        
                    (*env)->ReleaseStringUTFChars(env, partition_C_str, partition_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-3);
                }
           
    }
    
    /* Colocando numero de particoes */
    fseek(f_partition, 0L, SEEK_SET);
    fprintf(f_partition, "%08d\n", count);
    
    fclose(f_partition);
        
    (*env)->ReleaseStringUTFChars(env, partition_C_str, partition_str);
    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
    return(0);
}


// int get_intervals(intervals_str, basename)
// char *intervals_str;
// char *basename;
JNIEXPORT jint JNICALL Java_Tangram2_Entrada_get_1intervals(JNIEnv *env, jobject obj, jstring intervals_C_str, jstring basename_C)
{
    FILE   *f_intervals;
    char   intervals_name[100];
    char   number[30];
    double tempo, last_time;
    int    num_pontos;
    int    i, j, count;
    
    const  char *intervals_str;
    const  char *basename;
    
    
    intervals_str = (*env)->GetStringUTFChars(env, intervals_C_str, 0);
    basename = (*env)->GetStringUTFChars(env, basename_C, 0);
    
    
    strcpy(intervals_name, basename);
    strcat(intervals_name, ".intervals");
    
    if ((f_intervals = fopen(intervals_name,"w")) == NULL)
    {
        fprintf(stderr,"Can't create intervals file!\n");
        
        (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
        return(-1);
    }
    
    
    /* Alocando espaco para numero de intervalos */
    fprintf(f_intervals, "0000000\n");


    count = 0;
    last_time = -1.0;
    for (i = 0; intervals_str[i] != '\0'; )
    {
        if (isdigit((int)intervals_str[i]))
        {
            memset(number, 0, 30);
            j = 0;
            while ((isdigit(intervals_str[i])) || (intervals_str[i] == '.') || 
                     (intervals_str[i] == 'e') || (intervals_str[i] == '-') || 
                     (intervals_str[i] == '+'))
            {
                if ((!isdigit(intervals_str[i])) && (intervals_str[i] == number[j - 1]))
                {
                    fprintf(stderr,"Invalid time interval specification.\n");
        
                    (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-2);
                }
                    
                number[j] = intervals_str[i];
                j++;
                i++;
            }
        
            number[j] = '\0';
            tempo = atof(number);
            if (tempo <= last_time)
            {
                fprintf(stderr,"Time intervals must be in increasing order.\n");
        
                (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                return(-3);
            }
            fprintf(f_intervals, "%.10e\n", tempo);
            
            count++;
            
            last_time = tempo;
        }
        else 
            if (intervals_str[i] == 'n')
            {
                /* Posiciona ponteiro na definicao do tempo inicial */
                while (intervals_str[++i] == ' ');
            
                memset(number, 0, 30);
                j = 0;
                while ((isdigit(intervals_str[i])) || (intervals_str[i] == '.') || 
                         (intervals_str[i] == 'e') || (intervals_str[i] == '-') || 
                         (intervals_str[i] == '+'))
                {
                    if ((!isdigit(intervals_str[i])) && (intervals_str[i] == number[j - 1]))
                    {
                        fprintf(stderr,"Invalid time interval specification.\n");
        
                        (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                        return(-2);
                    }

                    number[j] = intervals_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                tempo = atof(number);
                if (tempo <= last_time)
                {
                    fprintf(stderr,"Time intervals must be in increasing order.\n");
        
                    (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-3);
                }
                fprintf(f_intervals, "n %10e ", tempo);
                
                last_time = tempo;


                /* Posiciona ponteiro na definicao do tempo final */
                while (intervals_str[++i] == ' ');
            
                memset(number, 0, 30);
                j = 0;
                while ((isdigit(intervals_str[i])) || (intervals_str[i] == '.') || 
                         (intervals_str[i] == 'e') || (intervals_str[i] == '-') || 
                         (intervals_str[i] == '+'))
                {
                    if ((!isdigit(intervals_str[i])) && (intervals_str[i] == number[j - 1]))
                    {
                        fprintf(stderr,"Invalid time interval specification.\n");
        
                        (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                        return(-2);
                    }

                    number[j] = intervals_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                tempo = atof(number);
                if (tempo < last_time)
                {
                    fprintf(stderr,"Time intervals must be in increasing order.\n");
        
                    (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-3);
                }
                fprintf(f_intervals, "%10e ", tempo);
                
                last_time = tempo;


                /* Posiciona ponteiro na definicao do numero de pontos */
                while (intervals_str[++i] == ' ');

                memset(number, 0, 30);
                j = 0;
                while (isdigit(intervals_str[i]))
                {
                    number[j] = intervals_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                num_pontos = atoi(number); 
                fprintf(f_intervals, "%d\n", num_pontos);
                
                count += num_pontos;
            }
            else
                if ((intervals_str[i] == ' ') || (intervals_str[i] == ';')
                                              || (intervals_str[i] == ','))
                    i++;
                else
                {
                    fprintf(stderr,"Invalid character at intervals description.\n");
        
                    (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-4);
                }
           
    }
    
    /* Colocando numero de particoes */
    fseek(f_intervals, 0L, SEEK_SET);
    fprintf(f_intervals, "%07d\n", count);

    fclose(f_intervals);
        
    (*env)->ReleaseStringUTFChars(env, intervals_C_str, intervals_str);
    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
    return(0);
}


// int get_initial_probs(initial_probs_str, basename)
// char *initial_probs_str;
// char *basename;
JNIEXPORT jint JNICALL Java_Tangram2_Entrada_get_1initial_1probs(JNIEnv *env, jobject obj, jstring initial_probs_C_str, jstring basename_C)
{
    FILE   *f_initial;
    char   initial_probs_name[100];
    char   number[30];
    double prob, sum_prob;
    int    state, first_state, end_state;
    int    i, j;
    
    const  char *initial_probs_str;
    const  char *basename;
    
        

    initial_probs_str = (*env)->GetStringUTFChars(env, initial_probs_C_str, 0);
    basename = (*env)->GetStringUTFChars(env, basename_C, 0);
    
        
    strcpy(initial_probs_name, basename);
    strcat(initial_probs_name, ".init_prob");
    
    if ((f_initial = fopen(initial_probs_name,"w")) == NULL)
    {
        fprintf(stderr,"Can't create initial probability file!\n");
        
        (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
        return(-1);
    }
    
    
    /* Caso de estados equiprovaveis */
    if (!strcmp(initial_probs_str,"e_All"))
    {
        fprintf(f_initial, "e_All\n");
	fclose(f_initial);
        
        (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
	return(0);
    }
    
    
    sum_prob = 0.0;
    for (i = 0; initial_probs_str[i] != '\0'; )
    {
        if (isdigit((int)initial_probs_str[i]))
        {
            memset(number, 0, 30);
            j = 0;
            while (isdigit(initial_probs_str[i]))
            {
                number[j] = initial_probs_str[i];
                j++;
                i++;
            }
        
            number[j] = '\0';
            state = atoi(number);
            
            
            /* Posiciona para pegar a probabilidade */
            while (initial_probs_str[++i] == ' ');
            
            memset(number, 0, 30);
            j = 0;
            while ((isdigit(initial_probs_str[i])) || (initial_probs_str[i] == '.') || 
                     (initial_probs_str[i] == 'e') || (initial_probs_str[i] == '-') || 
                     (initial_probs_str[i] == '+'))
            {
                if ((!isdigit(initial_probs_str[i])) && (initial_probs_str[i] == number[j - 1]))
                {
                    fprintf(stderr,"Invalid probability specification.\n");
        
                    (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-2);
                }
                    
                number[j] = initial_probs_str[i];
                j++;
                i++;
            }
        
            number[j] = '\0';
            prob = atof(number);
            
            if (prob > 1)
            {
                fprintf(stderr,"Probability value greater to 1 doesn't make sense.\n");
        
                (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
                (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                return(-3);
            }
            
            fprintf(f_initial, "%d %.10e\n", state, prob);
            sum_prob += prob;
        }
        else 
            if (initial_probs_str[i] == 'e')
            {
                /* Posiciona ponteiro na definicao do primeiro estado do set */
                while (initial_probs_str[++i] == ' ');

                memset(number, 0, 30);
                j = 0;
                while (isdigit(initial_probs_str[i]))
                {
                    number[j] = initial_probs_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                first_state = atoi(number); 
            

                /* Posiciona ponteiro na definicao do ultimo estado do set */
                while (initial_probs_str[++i] == ' ');

                memset(number, 0, 30);
                j = 0;
                while (isdigit(initial_probs_str[i]))
                {
                    number[j] = initial_probs_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                end_state = atoi(number); 
            

                /* Posiciona ponteiro na definicao da probabilidade dos estados do set */
                while (initial_probs_str[++i] == ' ');

                memset(number, 0, 30);
                j = 0;
                while ((isdigit(initial_probs_str[i])) || (initial_probs_str[i] == '.') || 
                         (initial_probs_str[i] == 'e') || (initial_probs_str[i] == '-') || 
                         (initial_probs_str[i] == '+'))
                {
                    if ((!isdigit(initial_probs_str[i])) && (initial_probs_str[i] == number[j - 1]))
                    {
                        fprintf(stderr,"Invalid probability specification.\n");
        
                        (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
                        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                        return(-2);
                    }
                    
                    number[j] = initial_probs_str[i];
                    j++;
                    i++;
                }
        
                number[j] = '\0';
                prob = atof(number);
            
                if (prob > 1)
                {
                    fprintf(stderr,"Probability value greater 1 doesn't make sense.\n");
        
                    (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-3);
                }
            
                fprintf(f_initial, "e %d %d %.10e\n", first_state, end_state, prob);
                sum_prob += ((end_state - first_state + 1) * prob);
            }
            else
                if ((initial_probs_str[i] == ' ') || (initial_probs_str[i] == ';')
                                                  || (initial_probs_str[i] == ','))
                    i++;
                else
                {
                    fprintf(stderr,"Invalid character at initial probability description.\n");
        
                    (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
                    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
                    return(-4);
                }
           
    }
    
    /* Teste */
    /* printf("Soma = %lf\n", sum_prob); */
    
    if (((1 - sum_prob) > 0.001) || ((1 - sum_prob) < -0.001))
    {
        fprintf(stderr,"Sum of probability values must be 1: %f.\n", sum_prob);
        
        (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
        (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
        return(-5);
    }
    
    fclose(f_initial);
        
    (*env)->ReleaseStringUTFChars(env, initial_probs_C_str, initial_probs_str);
    (*env)->ReleaseStringUTFChars(env, basename_C, basename);
        
    return(0);   
}


/* Programa de teste */
/* void main()
{
    char basename[100];
    char partition_str[100];
    char intervals_str[100];
    char init_prob_str[100];
    int  status;
    
    
    strcpy(basename,"teste");
    strcpy(partition_str, "2,s  2 8");
    strcpy(intervals_str, "  n  6.0e0 3 7.00e1   8e+1");
    strcpy(init_prob_str, "1 1e-1  ;  2   03e-1; e  3  5  2e-001");
    
    if ((status = get_partition(partition_str, basename)) == -1)
        printf("Problems with get_partitions!\n");
    else
        printf("Sucess with get_partitions!\n");
        
    if ((status = get_intervals(intervals_str, basename)) == -1)
        printf("Problems with get_intervals!\n");
    else
        printf("Sucess with get_intervals!\n");
        
    if ((status = get_initial_probs(init_prob_str, basename)) == -1)
        printf("Problems with get_initial_probs!\n");
    else
        printf("Sucess with get_initial_probs!\n");

} */
