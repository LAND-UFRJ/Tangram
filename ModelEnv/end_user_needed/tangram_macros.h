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

#ifndef __TANGRAM_MACROS_H__
#define __TANGRAM_MACROS_H__

/* macro for messages */
#define  msg(a,b,c)    obj_st->add_pend_msg(a, b, c, sizeof(c))

/* macro for coping vectors */
#define  copy(a,b)   obj_st->copy(a,sizeof(a),b,sizeof(b))

#define  get_msg_data(a)    obj_st->get_msg_data_vec(a, sizeof(a), pend_msg )

/* macro for changing state var values */
#define  set_st(a,b)   obj_st->ch_st_var_value(a, b)

/* macros for saving/restore the queue state */
#define  save_at_head(a,b)   obj_st->save__at_head(a,b)
#define  save_at_tail(a,b)   obj_st->save__at_tail(a,b)
#define  restore_from_head(a,b)   obj_st->restore__from_head(a,b)
#define  restore_from_tail(a,b)   obj_st->restore__from_tail(a,b)

/* macro for getting state var values */
#define  get_st(a,b)   obj_st->get_st_var_value(a, b)

/* macro for delivering cloned eventd */
#define  clone_ev(a)   if (simulator != NULL) simulator->add_clone(a);\
                       else fprintf ( stderr, "ERROR: 'clone' can only be used in simulation\n"); 

/* macro for setting the cumulative reward value */
#define  set_cr(a,b)   if (simulator != NULL) simulator->set_cumulative_reward(obj_st,a,b);\
                       else fprintf ( stderr, "ERROR: using set_cr with no simulation\n");

/* macro for setting the instantaneous reward value */
#define  set_ir(a,b)   if (simulator != NULL) simulator->set_instantaneous_reward(obj_st,a,b);\
                       else fprintf ( stderr, "ERROR: using set_ir with no simulation\n");

/* macro for unsetting the instantaneous reward value */
#define  unset_ir(a)   if (simulator != NULL) simulator->unset_instantaneous_reward(obj_st,a);\
                       else fprintf ( stderr, "ERROR: using unset_ir with no simulation\n");

/* macro for getting the cumulative reward value */
#define  get_cr(a)   ( (simulator != NULL) ? simulator->get_cumulative_reward(obj_st,a) : 0)

/* macro for getting the instantaneous reward value */
#define  get_ir(a)   ( (simulator != NULL) ? simulator->get_instantaneous_reward(obj_st,a) : 0)

/* macro for getting the cumulative reward sum value */
#define  get_cr_sum(a)   ( (simulator != NULL) ? simulator->get_cumulative_reward_sum(obj_st,a) : 0)

/* macro for comparing objects destinations */
#define  objcmp(a,b)   ( (strcmp(a,b) == 0) ? 1 : 0)

/* macro for special_rr_ev() boolean function */
#define  special_rr_ev(a,b,c,d)  ( (simulator != NULL) ? simulator->special_rr_event(obj_st,a,b,c,d,ev_name_param) : 0)

/* macro for get_simul_time() function */
#define  get_simul_time()  ( (simulator != NULL) ? simulator->show_simul_time() : 0)

/* macro for get_random() function */
#define  get_random()  ( (simulator != NULL) ? simulator->get_random() : 0.0)

#endif
