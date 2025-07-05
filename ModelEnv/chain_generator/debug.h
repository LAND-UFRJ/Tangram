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
*	@file debug.h
*   @brief <b> Memory debug variables </b>.
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @version 1.0
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram II project
*   @since version 1.0
*
*/

#ifndef __DEGUG_H__
#define __DEGUG_H__

#ifdef D_MEM_DEBUG

int DBG_Class_counter = 0;
int DBG_Class_memory = 0;

int DBG_System_State_counter = 0;
int DBG_System_State_memory = 0;
int DBG_System_State_List_counter = 0;
int DBG_System_State_List_memory = 0;

int DBG_Object_State_counter = 0;
int DBG_Object_State_memory = 0;
int DBG_Object_State_List_counter = 0;
int DBG_Object_State_List_memory = 0;

int DBG_State_Variable_counter = 0;
int DBG_State_Variable_memory = 0;
int DBG_State_Variable_List_counter = 0;
int DBG_State_Variable_List_memory = 0;

int DBG_Transition_counter = 0;
int DBG_Transition_memory = 0;
int DBG_Transition_List_counter = 0;
int DBG_Transition_List_memory = 0;

int DBG_Pending_Message_counter = 0;
int DBG_Pending_Message_memory = 0;
int DBG_Pending_Message_List_counter = 0;
int DBG_Pending_Message_List_memory = 0;

int DBG_Known_State_counter = 0;
int DBG_Known_State_memory = 0;
int DBG_Known_State_List_counter = 0;
int DBG_Known_State_List_memory = 0;

int DBG_Rate_Reward_counter = 0;
int DBG_Rate_Reward_memory = 0;
int DBG_Rate_Reward_List_counter = 0;
int DBG_Rate_Reward_List_memory = 0;

int DBG_Imp_Reward_counter = 0;
int DBG_Imp_Reward_memory = 0;
int DBG_Imp_Reward_List_counter = 0;
int DBG_Imp_Reward_List_memory = 0;

int DBG_Imp_Rew_Val_counter = 0;
int DBG_Imp_Rew_Val_memory = 0;
int DBG_Imp_Rew_Val_List_counter = 0;
int DBG_Imp_Rew_Val_List_memory = 0;

int DBG_Expr_Val_counter = 0;
int DBG_Expr_Val_memory = 0;
int DBG_Expr_Val_List_counter = 0;
int DBG_Expr_Val_List_memory = 0;


#endif /* D_MEM_DEBUG */


#endif  /*  __DEGUG_H__ */
