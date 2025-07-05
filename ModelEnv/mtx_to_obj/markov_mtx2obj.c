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

/***************************************************************************
								mmfpgenerator.c  -  description
                             -------------------
    begin                : Qui Jan 31 12:00:07 BRST 2002
    copyright            : (C) 2002 by Bruno Felisberto
    email                : bruno@land.ufrj.br
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program generates a MMFP (Markov Modulated Fluid Process) model. *
 *   Its output is compatible with Tangram II's model objects              *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define FLOAT double
#define _READe_ "E"
#define _READf_ "lf"
#define FLOOR(a) floor(a)

#define MAX(a,b) ((a>b)?a:b)

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE 1

#define FLUID 1
#define PACKET 2

const char *tgif_line = "mini_line(14,11,3,0,0,0,[\n"
									"str_block(0,14,11,3,0,-1,0,0,0,[\n"
									"str_seg(\'%s\',\'Courier\',0,69120,14,11,3,0,-1,0,0,0,0,0,\n"
									"	\"%s\")])\n"
									"]),\n";
const char *tgif_event_begin = "mini_line(351,13,3,0,0,0,[\n"
								"str_block(0,351,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,351,13,3,0,-4,0,0,0,0,0,\n"
								"        \" event= Transition_%d(EXP, %"_READf_")\")])\n"
								"]),\n"
								"mini_line(225,13,3,0,0,0,[\n"
								"str_block(0,225,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,225,13,3,0,-4,0,0,0,0,0,\n"
								"        \" condition= (status==%d)\")])\n"
								"]),\n"
								"mini_line(225,13,3,0,0,0,[\n"
								"str_block(0,225,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,225,13,3,0,-4,0,0,0,0,0,\n"
								"        \" action=\")])\n"
								"]),\n";

const char *tgif_poisson_event = "mini_line(351,13,3,0,0,0,[\n"
								"str_block(0,351,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,351,13,3,0,-4,0,0,0,0,0,\n"
								"        \" event= Packet_Generation_%d(EXP, %.4"_READf_")\")])\n"
								"]),\n"
								"mini_line(225,13,3,0,0,0,[\n"
								"str_block(0,225,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,225,13,3,0,-4,0,0,0,0,0,\n"
								"        \" condition= (status==%d)\")])\n"
								"]),\n"
								"mini_line(225,13,3,0,0,0,[\n"
								"str_block(0,225,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,225,13,3,0,-4,0,0,0,0,0,\n"
								"        \" action=\")])\n"
								"]),\n"
								"mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \" {  float f; f = 0; };\")])\n"
								"]),\n"
								"mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \"\")])\n"
								"]),\n";
/*
const char *tgif_event_if_begin ="mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \"    if (status == %d)\")])\n"
								"]),\n";
*/
const char *tgif_event ="mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \" {  ; set_st(\\\"status\\\",%d); }: prob=%.8"_READf_";\")])\n"
								"]),\n";

/*
const char *tgif_event_if_end ="mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \"    }\")])\n"
								"]),\n";
*/
const char *tgif_event_end = "mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \"\")])\n"
								"]),\n";

const char *tgif_reward = "mini_line(261,13,3,0,0,0,[\n"
								"str_block(0,261,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'#0000ff\','Courier',0,69120,261,13,3,0,-4,0,0,0,0,0,\n"
								"        \"    condition = (status == %d)\")])\n"
								"]),\n"
								"mini_line(162,13,3,0,0,0,[\n"
								"str_block(0,162,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'#0000ff\','Courier',0,69120,162,13,3,0,-4,0,0,0,0,0,\n"
								"        \"    value= %"_READf_"-C;\")])\n"
								"]),\n";


const char *tgif_declaration = "";

const char *tgif_event_message_out = "mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \" if (status == %d) {  \")])\n"
								"]),\n"
								"mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \"    st=%d;  \")])\n"
								"]),\n"
								"mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \"    msg(port_out,all,%"_READf_");  \")])\n"
								"]),\n"
								"mini_line(324,13,3,0,0,0,[\n"
								"str_block(0,324,13,3,0,-4,0,0,0,[\n"
								"str_seg(\'black\',\'Courier\',0,69120,324,13,3,0,-4,0,0,0,0,0,\n"
								"        \" } \")])\n"
								"]),\n";

#include "tgif_template.h"

typedef int* intp;
typedef FLOAT* floatp;

static int		 firstsamplelevel;
/// Number of moments to calculate...  for now we just want standard deviation
static int		 moments = 2;

static int 		 hasbounds=FALSE;
/// moments
static FLOAT*  sample_moments;
/// programming stuff... to get some mesures after calculating, just fot the user.
static FLOAT*  saved_sample_moments;

static FLOAT   std_dev_percentil;

/// Tells if is just to read a given prob matrix
static int hasprobmatrix = FALSE;
/// Tells if the tgif file will have messages on each state
static int messageout = FALSE;
/// Tells qsort how to compare two integers (from samples)
int compar(const void *a, const void *b)
{
	return ((*(int*)a) - (*(int*)b));
}

/***
*  stddev - Calculates On-the-Fly the mean and standard of the samples
*  ATENTION: totalsamples is not the total amount of samples... it's the
*            amount of samples until now... it's a calculate-on-the-fly routine
**/
FLOAT stddev(int sample, int totalsamples)
{
	FLOAT  norm_sample;
	int i;

	norm_sample = ((FLOAT)sample)/((FLOAT)totalsamples);
	for (i=0; i < moments; i++)
	{
 		sample_moments[i] = sample_moments[i]*((FLOAT)totalsamples-1)/((FLOAT)totalsamples) + norm_sample;
		norm_sample = norm_sample*((FLOAT)sample);
	}
	return (sqrt(sample_moments[1]-sample_moments[0]*sample_moments[0]));
}

/// Prints calculated statistics
void print_statistics()
{
	int k;
	printf("%.8"_READe_" - Samples mean\n",sample_moments[0]);
	for (k=1; k < moments; k++)
	{
		if (k == 1)
		{
			printf("%.8E - Samples standard deviation\n",sqrt(sample_moments[1]-sample_moments[0]*sample_moments[0]));
		}
		else
		{
			if (k==2) printf("%.8"_READe_" - Samples %dtd moment\n",sample_moments[k],k+2);
			else printf("%.8"_READe_" - Samples %dth moment\n",sample_moments[k],k+2);
		}
  }
}

/// Prints calculated probabilities
void print_probs(intp probmatrix, int states)
{
	int k,j;
	long long unsigned int total_state,no_round_error,error;
	FLOAT result;

	printf("\n *** Probability Matrix.\n%d - States\n\n",states);
	for (k=0; k < states; k++)
	{
		error = no_round_error = total_state = 0;
		for (j=0; j < states; j++)
		{
			total_state += probmatrix[k*states+j];
		}
		if (probmatrix[k*states+k] == total_state) // absorving state
		{
			int ii;
			if  (k == 0) ii = 1;
			else ii=k-1;
			probmatrix[k*states+ii]++;
			total_state++;
		}
		for (j=0; j < states; j++)
		{
			no_round_error += probmatrix[k*states+j];
			if (no_round_error == total_state)
				result = (FLOAT)1.0-((FLOAT)error)/(FLOAT)1e6;
			else
			{
				result = (FLOAT)((1000000*(long long unsigned int)(probmatrix[k*states+j]))/(long long unsigned int)total_state)/(FLOAT)1e6;
				error += (1000000*(long long unsigned int)(probmatrix[k*states+j]))/(long long unsigned int)total_state;
			}
			if (probmatrix[k*states+j] > 0) printf("%d %d %.6"_READf_"\n",k,j,result);
  		}
  }

}

/**
* calculate_hist_levels - Calculate how many levels are necessary to
*    create a MMFP that the samples corresponding to every state,
*    has in the maximun a standard deviation that is 2*srqt(mean).
*		It also returns the samples-to-state classification.
* Input: An ordered samples' vector (ascending).
**/
void calculate_hist_levels(int *samples, int totalsamples, int *levels, intp *levelmax, floatp staterates)
{
	int i,k,lastleveli;
	int currsample = 1;
	FLOAT save_moments[moments];

	// Initialize the sample moments
	for (i=0; i < moments; i++) sample_moments[i] = 0.0;
	// ..and the levels
	*levels = 0;
	(*levelmax)[*levels] = samples[0];
	(*levels)++;
	lastleveli=0;
	for (i=0; i < totalsamples; i++)
	{
	   memcpy(save_moments,sample_moments,sizeof(FLOAT)*moments);
	  // Calculates if this new sample is not in the same level as the others.
		if (stddev(samples[i],currsample++) > sample_moments[0]*std_dev_percentil)
		{ // has reached a new level
	   	memcpy(sample_moments,save_moments,sizeof(FLOAT)*moments);
			printf("\n --- Has changed from level %d [%d sample(s)] , min sample %d ---\n",*levels,i-1-lastleveli,(*levelmax)[*levels-1]);
			lastleveli = i-1;

			print_statistics();
			staterates[*levels-1] = sample_moments[0];
			// Keep the maximun value of the sample... note: i must be >= 2,
			//                                         as to have stddev you must have at least 2 samples.
			(*levelmax)[*levels] = samples[i];
			(*levels)++;
			// Reinitialize the statistics
			for (k=0; k < moments; k++) sample_moments[k] = 0.0;
			currsample = 1;
			stddev(samples[i],currsample++);
		}
	}
	(*levelmax)[*levels] = samples[totalsamples-1]+1; // stop condition for the binary search
	staterates[*levels-1] = sample_moments[0];
	printf("\n  --- Last level (%d) [%d sample(s)], min sample %d ---\n",*levels,totalsamples-1-lastleveli,(*levelmax)[*levels-1]);
	print_statistics();
}

/// Search to find from which state the sample belongs to...
int find_level(int sample,int levels, intp levelmax)
{
	int k;
	for (k=1; k < levels; k++)
	{
		if (sample < levelmax[k])
		{
			return (k-1);
		}
	}
	return (levels-1);
}

/**
* calculate_state_trans_probability - Calculate the probability
*    of change from state i to j.
*  <b>Does not have roundoff errors</b>, as the last prob is calculated by 1-sum(the others) ;)
**/
void calculate_state_trans_probability(int *samples, int totalsamples, int levels, intp levelmax, intp *probmatrix)
{
	int i,newk,oldk,j,total;
	int states = levels;
	int statealiases[levels];
	FLOAT P;

	// alloc memory and clears it.
	*probmatrix = calloc(levels*levels*100,sizeof(int)); // CAUTION
	if (*probmatrix == NULL)
	{
		printf("\n Could not allocate %d MB of memory for %d samples and its moments. \n",
							(2*sizeof(int)*totalsamples+sizeof(int)*levels*levels*100+2*sizeof(FLOAT)*moments)/(1024*1024),totalsamples);
		exit(EXIT_FAILURE);
	}
	for (i=0; i < levels; i++) statealiases[i] = i;
	// find from which state belongs the first sample
	firstsamplelevel = oldk = find_level(samples[0],levels,levelmax);
	for (i=1; i < totalsamples; i++)
	{
		// ... get the next state
		newk = find_level(samples[i],levels,levelmax);
		// add one more transition to newk state to oldk's state
		total = 0;
		for (j=0; j < states; j++)
			total += (*probmatrix)[(statealiases[oldk]*levels)+j];
		if (total > 0)
			P = pow(1-((*probmatrix)[(statealiases[oldk]*levels)+statealiases[newk]]/(FLOAT)total),(*probmatrix)[(statealiases[oldk]*levels)+statealiases[newk]])+
						(*probmatrix)[(statealiases[oldk]*levels)+statealiases[newk]]/(FLOAT)total;
		else
			P=0.0;
		if ((P < 0.05) && ((*probmatrix)[(statealiases[oldk]*levels)+statealiases[newk]] > 4))
		{
			/*fprintf(stdout,"\n P[%d|%d] = %"_READf_,statealiases[oldk],statealiases[newk],P);
			statealiases[oldk] = states;
			aliasesstate[states] = oldk;
			states++;*/
		}
		(*probmatrix)[(statealiases[oldk]*levels)+statealiases[newk]]++;
		oldk = newk;
	}
}

void print_tgif_file(intp probmatrix, int states, FLOAT rate, floatp staterates, floatp fprobmatrix)
{
	char	*includedline;
	char	line[1024*500];
	char	color[100];
	char	*pcolor;
	char	*pincl;
	char	*tgifstring;
	char	*tplt;
	char	*str;
	int	i,j;
	FLOAT p;

	tplt = (char*)tgif_template;
	tgifstring = malloc(sizeof(char)*10*1024*1024);
	includedline = malloc(sizeof(char)*10*1024*1024);
	str = tgifstring;
	if ((tgifstring == NULL) || (includedline == NULL))
	{
		printf("\n Could not allocate %d MB of memory for tgif's output. \n",
							(sizeof(char)*20*1024*1024+sizeof(int)*states*states+2*sizeof(FLOAT)*moments)/(1024*1024));
		exit(EXIT_FAILURE);
	}
	while (*tplt)
	{
		if ((*tplt == '%') && (*(tplt+1)=='s'))
		{ // To substitute
			char *paux;
			tplt += 2;
			pincl = includedline;
			pcolor = color;
			paux = tplt+2;
			while(*paux != '\n')
			{
				*pcolor = *paux;
				pcolor++; paux++;
			}
			*pcolor = '\0';
			includedline[0] = '\0';
			switch (*tplt)
			{
			//REWARDS
				case 'r' :  if (messageout == PACKET)
								{
									sprintf(includedline+strlen(includedline),tgif_line,color,"impulse_reward=enviados");
									for(i=0; i < states; i++)
									{
										sprintf(line,"  event=Packet_Generation_%d,1",i);
										sprintf(includedline+strlen(includedline),tgif_line,color,line);
										sprintf(includedline+strlen(includedline),tgif_line,color,"  value=1;");
									}
								}
								sprintf(includedline+strlen(includedline),tgif_line,color,"rate_reward=sent");
								if (hasbounds)
									sprintf(includedline+strlen(includedline),tgif_line,color,"  bounds= Bmin,Bmax");
								for(i=0; i < states; i++)
								{
									sprintf(line,tgif_reward,i,staterates[i]);
									strcat(includedline,line);
								}
				break;
			//EVENTS
				case 'e' :	{
									long long unsigned int totalprob;
                                    totalprob = 0;
									for( i = 0; i < states; i++ )
									{
										if( fprobmatrix == NULL )
										{
											totalprob = 0;
											for(j=0; j < states; j++)
												totalprob += probmatrix[i*states+j];
										}

										sprintf(includedline+strlen(includedline),tgif_event_begin,i,rate,i);

										for(j=0; j < states; j++)
										{
											if (probmatrix != NULL) p = ((FLOAT)probmatrix[i*states+j])/((FLOAT)totalprob);
											else p = fprobmatrix[i*states+j];
											if ( p != 0.0 )
											{
												if (messageout == FLUID)
													sprintf(includedline+strlen(includedline),tgif_event_message_out,j,p,staterates[i]);
												else
													sprintf(includedline+strlen(includedline),tgif_event,j,p);
											}
										}
										sprintf(includedline+strlen(includedline),tgif_event_end);

										if (messageout == PACKET) sprintf(includedline+strlen(includedline),tgif_poisson_event,i,staterates[i],i,staterates[i]);
									}
								}
				break;
			//INITIALIZATION
				case 'i' :{

								sprintf(line,"     status=%d",firstsamplelevel);
								sprintf(includedline,tgif_line,color,line);
								if (messageout == FLUID)
									sprintf(includedline+strlen(includedline),tgif_line,color,"     port_out=");
								sprintf(includedline+strlen(includedline),tgif_line,color,"     C=0");
								if (hasbounds)
								{
									sprintf(includedline+strlen(includedline),tgif_line,color,"     Bmin=");
									sprintf(includedline+strlen(includedline),tgif_line,color,"     Bmax=");
								}
							}
				break;
			//STATE_VARS
				case 'v' :  sprintf(line,"     status=%d",firstsamplelevel);
								sprintf(includedline,tgif_line,color,line);
				break;
			//DECLARATION
				case 'd' :{
								strcpy(includedline,tgif_declaration);
								sprintf(includedline+strlen(includedline),tgif_line,color,"     Const");
								if (messageout == FLUID)
									sprintf(includedline+strlen(includedline),tgif_line,color,"       Port: port_out;");
								sprintf(includedline+strlen(includedline),tgif_line,color,"       Float: C;");
								if (hasbounds)
									sprintf(includedline+strlen(includedline),tgif_line,color,"       Float: Bmin,Bmax;");

							}
				break;
			}
			tplt=paux+1;
			while (*pincl)
			{
				*str = *pincl;
				str++; pincl++;
			}
		}
		else
		{
			*str = *tplt;
			tplt++;
			str++;
		}
	}
	*str='\0';
	fprintf(stderr,"%s",tgifstring);
	free(tgifstring);
	free(includedline);
}

void usage()
{
	printf("\n\n Usage: trace_mtx2obj <trace_file>"
				" <rate> <std_dev_percentil_from_mean> "
				" [-m] [-p]\n\n"
		/*		" -s => Maximum number of states\n" */
				" -p => Packet Mode\n"
				" -m => Create a message <msg(port_out,all,rate_x)> on each state\n\n"
                " OR:\n"
                "       trace_mtx2obj -P <rew_matrix_file> <rate> [-m] [-p]\n\n");
}

int fatoui(FILE **trace)
{
	static char s[100];
	char *p;
	int i = 0;

	fgets((p=s),100,*trace);
	while ((*p >= '0') && (*p <= '9'))
	{
		i *= 10;
		i += *p - '0';
		p++;
	}
	if ((*p != '\n') || (p == s)) return -1;
	return i;
}

void read_file(FILE **trace, int *samples)
{
	int i = 0;
	while (!feof(*trace))
	{
		samples[i++] = fatoui(trace);
		if (samples[i-1] < 0) i--;
	}
}

int read_prob_matrix(FILE **probmtxfile, floatp *probmatrix,floatp* staterates)
{
	int i,j,maxlevels;
	FLOAT p;
	fscanf(*probmtxfile,"%*s %*f %d",&maxlevels);
	*staterates = malloc(sizeof(FLOAT)*maxlevels);
	*probmatrix = calloc(maxlevels*maxlevels,sizeof(FLOAT)); // CAUTION
	if ((*staterates == NULL) || (*probmatrix==NULL))
	{
		printf("\n Could not allocate %d MB of memory for %d levels. \n",
							(maxlevels*maxlevels*sizeof(FLOAT)+sizeof(FLOAT)*maxlevels)/(1024*1024),maxlevels);
		return EXIT_FAILURE;
	}
	for(i=0; i < maxlevels; i++)
	{
		fscanf(*probmtxfile,"%"_READf_,&(*staterates)[i]);
	}
	while (!feof(*probmtxfile))
	{
		if (fscanf(*probmtxfile,"%d %d %"_READf_,&i,&j,&p) == 3)
		{
			printf("i,j %d,%d %"_READf_"\n",i,j,p);
			if ((--i < maxlevels) && (--j < maxlevels))
				(*probmatrix)[i*maxlevels+j]=p;
		}
	}
	return (maxlevels);
}

int get_file_samples(FILE **trace)
{
	int total = 0;
	while (!feof(*trace))
	{
		if (fatoui(trace) >= 0) total++;
	}
	return (total);
}

void read_parameters(int argc, char *argv[], FILE **trace, FLOAT *rate, int *max_states)
{
	int i;

	if ((argc <= 2) || (argc > 10))
	{
		usage();
		exit(EXIT_FAILURE);
	}
	if (strcmp(argv[1],"-P") == 0)
	{
		if (argc < 4)
                {
                        usage();
                        exit(EXIT_FAILURE);
                }
		hasprobmatrix = TRUE;
		(*trace) = fopen(argv[2],"rt");
		if (*trace == NULL)
		{
			printf("\n Could not open probability matrix file for reading. \n");
			exit(EXIT_FAILURE);
		}
		if ((sscanf(argv[3],"%"_READf_,rate) != 1) || (*rate <= (FLOAT)0.0))
		{
			printf("\n Invalid <rate> parameter (should be > 0).\n");
			usage();
			exit(EXIT_FAILURE);
		}
		i = 4;
	}
	else
	{
		(*trace) = fopen(argv[1],"rt");
		if (*trace == NULL)
		{
			printf("\n Could not open trace file for reading. \n");
			exit(EXIT_FAILURE);
		}
		if ((sscanf(argv[2],"%"_READf_,rate) != 1) || (*rate <= (FLOAT)0.0))
		{
			printf("\n Invalid <rate> parameter (should be > 0).\n");
			usage();
			exit(EXIT_FAILURE);
		}
		if ((sscanf(argv[3],"%"_READf_,&std_dev_percentil) != 1) || (std_dev_percentil <= (FLOAT)0.0))
		{
			printf("\n Invalid percentil parameter (should be > 0).\n");
			usage();
			exit(EXIT_FAILURE);
		}
		i = 4;
	}

	if (argc >= i+1)
	{
		while (i < argc)
		{
			if(argv[i][0] == '-')
			{
				switch (argv[i][1])
				{
					case 's':
						if ((sscanf(argv[i],"%d",max_states) != 1) || (*max_states <= 0))
						{
							printf("\n Invalid <max_states> parameter (should be > 0).\n");
							usage();
							exit(EXIT_FAILURE);
						}
						i++;
					break;
					case 'm':
						messageout = FLUID;
					break;
					case 'p':
						messageout = PACKET;
					break;
					case 'b':
						hasbounds = TRUE;
					break;
				}
			}
			else
			{
				usage();
				exit(EXIT_FAILURE);
			}
			i++;
		}
	}
}

/// Main program
int main(int argc, char *argv[])
{
	FILE*  trace;
	int    totalsamples;
	int    max_states = 1000;
	int*   samples = NULL;
	int    levels;
	intp   levelmax = NULL;
	intp 	 probmatrix;
	floatp fprobmatrix;
	/// state rates
	static floatp  state_rates;
	FLOAT rate;

	printf("\n*** Be Welcome! ***\n");
	printf("trace_mtx2obj - Written by Bruno Felisberto (LAND/UFRJ) 2002\n\n");

	read_parameters(argc,argv,&trace,&rate,&max_states);

	if (hasprobmatrix)
	{
		levels = read_prob_matrix(&trace,&fprobmatrix,&state_rates);
		print_tgif_file(NULL, levels, rate, state_rates, fprobmatrix);
		free(fprobmatrix);
		free(state_rates);
		fclose(trace);
	}
	else
	{
		printf("\nReading file to know the number of valid samples...\n");
		totalsamples = get_file_samples(&trace);
		fseek(trace,0,SEEK_SET);
		samples = malloc(sizeof(int)*totalsamples);
		state_rates = malloc(sizeof(FLOAT)*max_states);
		sample_moments = malloc(sizeof(FLOAT)*moments);
		saved_sample_moments = malloc(sizeof(FLOAT)*moments);
		levelmax = malloc(sizeof(int)*totalsamples);
		if ((samples == NULL) || (sample_moments == NULL) ||
				(saved_sample_moments == NULL) || (levelmax == NULL) || (state_rates == NULL))
		{
			printf("\n Could not allocate %d MB of memory for %d samples and its moments. \n",
								(2*sizeof(int)*totalsamples+2*sizeof(FLOAT)*moments+sizeof(FLOAT)*max_states)/(1024*1024),totalsamples);
			return EXIT_FAILURE;
		}
		printf("\nReading file...\n");
		read_file(&trace,samples);
		printf("*** Samples: %d\n",totalsamples);

		printf("\nSorting samples...\n");
	// Sort samples so we can calculate how many MMPP levels there are
		qsort((void*)samples,totalsamples,sizeof(int),compar);

		printf("\nCalculating levels...\n");
		calculate_hist_levels(samples, totalsamples, &levels, &levelmax, state_rates);

		printf("\nRereading file...\n");
	// Reread so we can calculate the levels transition probability
		fseek(trace,0,SEEK_SET);
		read_file(&trace,samples);
		fclose(trace);
		printf("\nCalculating transition probability matrix...\n");
		// Create and calculate states transition probability matrix
		calculate_state_trans_probability(samples, totalsamples, levels, levelmax, &probmatrix);

		free(samples);
		free(levelmax);

		print_probs(probmatrix,levels);

		print_tgif_file(probmatrix,levels,rate,state_rates,NULL);

		free(probmatrix);
		free(state_rates);
		free(sample_moments);
	}
	return EXIT_SUCCESS;
}
