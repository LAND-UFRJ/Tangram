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


#ifndef _DET_EVENT_H_
#define _DET_EVENT_H_

int *read_global_states(char*, int, int, int);
Timeout_state *timeout_transitions(char*, int, int*);
void read_entry_chain(int, int, Event*, FILE*);
void read_independent_chains(int, int, Event*, FILE*);
int compare_chain_states(int, int, int, int, int*, Event*);
int compare_absorbing_states(int, int, int, int, int*, Event*);
void mapping_global_states(int, int, int, int*, Event*);
void read_event_input(int, int, int, int*, Event*, FILE*);
double *multiply_vector(int, int, Event*, double*) ;
void omega_c0(int, int, int, int*, double*, double*, Event*);
int compare_abs_states(int, int, int, int, Event*);
void calculate_omega_g1(int,int,int,int,int,int*,double*,double*,Event*);
void calculate_omega_gn(int,int,int,int,int,int,int*,double*,double*,Event*);
double *calculate_gamma(int, int, int, int*, Event*) ;
void prob_absorbing_states(int, int, double, Event*, Matrix*);
void event_prob(int, int, int, double, Timeout_state*, Matrix*, Matrix*);
void prob_timeout(int,int,int,int,double,Timeout_state*,Event*,Matrix*,Matrix*);
Matrix *new_matrix_H(char*, Matrix*, int*);
void write_prob_file(char*, double, Matrix*);
void write_steady_states_file(char*, int, double*);
double *calculate_steady_states_vector(char*, int, int, int*, int*, Matrix*, double, char, double, int * );
double *marginal_prob_preemp_chain(int, int*, int*, Event*, double*);
double expected_cycle_length(int,int,double,double,Event*,double*,Matrix*,FILE*);
void prob_chain_0(int, int, int, int, Event*, double*, double*, double*);
void prob_chain_c(int, int, int, int, Event*, double*, double*);
void when_abs_occurs(int, int, int, Event*, double*, double*, double*);
void when_timeout_occurs(int, int, int, int, Event*, double*, double*, double*);
void preemptive_chain_prob(int, int, int, Event*, double*, double*);
void constant_interval(int, int, int, Event*, double*, double*);
void expected_time(int, int, double, double*, double*, Event*);
void calculate_measures_interest(char*, int, int, int, double, double, int*, double*, Matrix*, Event*);
void embedded_markov_chain( char *, double, char, double, int * );

#endif
