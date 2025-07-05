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
*	@file general.cpp
*   @brief <b> General methods (e.g. Debug, Alarm Handler, SIGSEGV handler, etc ). </b>
*   @remarks Lot of.
*   @author LAND/UFRJ
*   @date 1999-2009
*   @warning Do not modify this class before knowing the whole Tangram-II project
*   @since version 1.0
*
*   The detailed description is unavailable. 
*/

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "stdarg.h"

extern int       Debug_Level;
extern t_context context;

#ifdef _MEM_DEBUG

extern int DBG_System_State_counter;
extern int DBG_System_State_memory;
extern int DBG_System_State_List_counter;
extern int DBG_System_State_List_memory;

extern int DBG_Object_State_counter;
extern int DBG_Object_State_memory;
extern int DBG_Object_State_List_counter;
extern int DBG_Object_State_List_memory;

extern int DBG_State_Variable_counter;
extern int DBG_State_Variable_memory;
extern int DBG_State_Variable_List_counter;
extern int DBG_State_Variable_List_memory;

extern int DBG_Transition_counter;
extern int DBG_Transition_memory;
extern int DBG_Transition_List_counter;
extern int DBG_Transition_List_memory;

extern int DBG_Pending_Message_counter;
extern int DBG_Pending_Message_memory;
extern int DBG_Pending_Message_List_counter;
extern int DBG_Pending_Message_List_memory;

extern int DBG_Known_State_counter;
extern int DBG_Known_State_memory;
extern int DBG_Known_State_List_counter;
extern int DBG_Known_State_List_memory;

extern int DBG_Rate_Reward_counter;
extern int DBG_Rate_Reward_memory;
extern int DBG_Rate_Reward_List_counter;
extern int DBG_Rate_Reward_List_memory;

extern int DBG_Imp_Reward_counter;
extern int DBG_Imp_Reward_memory;
extern int DBG_Imp_Reward_List_counter;
extern int DBG_Imp_Reward_List_memory;

extern int DBG_Imp_Rew_Val_counter;
extern int DBG_Imp_Rew_Val_memory;
extern int DBG_Imp_Rew_Val_List_counter;
extern int DBG_Imp_Rew_Val_List_memory;

extern int DBG_Expr_Val_counter;
extern int DBG_Expr_Val_memory;

void debug_mem(int DBG_id, const char *DBG_msg)
{
  //     printf ("OBJ: %d --> %s\n",DBG_id, DBG_msg);
  // fflush(stdout);
}

void debug_mem_print()
{
     int total = 0;
     int aux;

     fprintf ( stdout, "Class: System_State\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_System_State_counter);
     aux = DBG_System_State_memory * sizeof(System_State);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_System_State_memory, aux);
     fprintf ( stdout, "Class: System_State_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_System_State_List_counter);
     aux = DBG_System_State_List_memory * sizeof(System_State_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_System_State_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Object_State\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Object_State_counter);
     aux = DBG_Object_State_memory * sizeof(Object_State);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Object_State_memory, aux);
     fprintf ( stdout, "Class: Object_State_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Object_State_List_counter);
     aux = DBG_Object_State_List_memory * sizeof(Object_State_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Object_State_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: State_Variable\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_State_Variable_counter);
     aux = DBG_State_Variable_memory * sizeof(State_Variable);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_State_Variable_memory, aux);
     fprintf ( stdout, "Class: State_Variable_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_State_Variable_List_counter);
     aux = DBG_State_Variable_List_memory * sizeof(State_Variable_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_State_Variable_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Transition\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Transition_counter);
     aux = DBG_Transition_memory * sizeof(Transition);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Transition_memory, aux);
     fprintf ( stdout, "Class: Transition_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Transition_List_counter);
     aux = DBG_Transition_List_memory * sizeof (Transition_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Transition_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Pending_Message\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Pending_Message_counter);
     aux = DBG_Pending_Message_memory * sizeof (Pending_Message);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Pending_Message_memory, aux);
     fprintf ( stdout, "Class: Pending_Message_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Pending_Message_List_counter);
     aux = DBG_Pending_Message_List_memory * sizeof(Pending_Message_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Pending_Message_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Known_State\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Known_State_counter);
     aux = DBG_Known_State_memory * sizeof (Known_State);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Known_State_memory, aux);
     fprintf ( stdout, "Class: Known_State_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Known_State_List_counter);
     aux = DBG_Known_State_List_memory * sizeof(Known_State_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d\n", DBG_Known_State_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Rate_Reward\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Rate_Reward_counter);
     aux = DBG_Rate_Reward_memory * sizeof (Rate_Reward);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Rate_Reward_memory, aux);
     fprintf ( stdout, "Class: Rate_Reward_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Rate_Reward_List_counter);
     aux = DBG_Rate_Reward_List_memory * sizeof (Rate_Reward_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Rate_Reward_List_memory,aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Imp_Reward\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Imp_Reward_counter);
     aux = DBG_Imp_Reward_memory * sizeof (Imp_Reward);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Imp_Reward_memory, aux);
     fprintf ( stdout, "Class: Imp_Reward_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Imp_Reward_List_counter);
     aux = DBG_Imp_Reward_List_memory * sizeof (Imp_Reward_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Imp_Reward_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Imp_Rew_Val\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Imp_Rew_Val_counter);
     aux = DBG_Imp_Rew_Val_memory * sizeof (Imp_Rew_Val);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Imp_Rew_Val_memory, aux);
     fprintf ( stdout, "Class: Imp_Rew_Val_List\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Imp_Rew_Val_List_counter);
     aux = DBG_Imp_Rew_Val_List_memory * sizeof (Imp_Rew_Val_List);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Imp_Rew_Val_List_memory, aux);
     fprintf ( stdout, "\n");

     fprintf ( stdout, "Class: Expr_Val\n");
     fprintf ( stdout, "\tobjects created  : %d\n", DBG_Expr_Val_counter);
     aux = DBG_Expr_Val_memory * sizeof (Expr_Val);
     total += aux;
     fprintf ( stdout, "\tobjects in memory: %d -- %d bytes\n", DBG_Expr_Val_memory, aux);

     fprintf ( stdout, "\nTotal Allocated memory: %d\n", total);

}

#endif /* _MEM_DEBUG */

void debug(int db_level, const char *db_msg, ...)
{
    /* this function was modified to allow variable number of parameters like printf */
    va_list params;
    char msg[ MAXSTRING ];

   /* the message is printed according with the debuglevel*/
   if (Debug_Level >= db_level) {
      va_start( params, db_msg );
      vsprintf( msg, db_msg, params );
      va_end( params );
      fprintf( stderr, msg );
      fprintf( stderr, "\n" );
      fflush(stderr);
    }
}

void segv_handler(int a)
{
  /* to disable the warning when compiling */
  a = 0;

  if (context.user_code) {
    fprintf ( stderr, "ERROR: Segmentation violation (SIGSEGV or SIGBUS) in user code execution.\n");
    fprintf ( stderr, "       Possibly vector out of range access.\n");
    fprintf ( stderr, "       Please, check your code.\n\n");
    fprintf ( stderr, "Aborting...\n");
  } else {
    fprintf ( stderr, "ERROR: Severe error in chain generator (SIGSEGV or SIGBUS)\n");
  }
  print_context();

  exit (-1);
}


void alarm_handler(int a)
{
  /* to disable the warning when compiling */
  a = 0;

  fprintf ( stderr, "ERROR: Too much time to execute user code.\n");
  fprintf ( stderr, "       Possibly inside an infinite loop.\n");
  fprintf ( stderr, "       Please, check your code.\n\n");
  print_context();
  fprintf ( stderr, "Aborting...\n");
  exit (-1);
}


int install_handlers()
{
  struct sigaction act;

  act.sa_handler = alarm_handler;
  if ( sigaction (SIGALRM, &act, NULL) < 0) {
    perror ("sigaction");
    return (-1);
  }

  act.sa_handler = segv_handler;
  if (sigaction (SIGSEGV, &act, NULL) < 0) {
    perror ("sigaction");
    return (-1);
  }

  if (sigaction (SIGBUS, &act, NULL) < 0) {
    perror ("sigaction");
    return (-1);
  }

  return (1);
}

void reset_context()
{
  /* reset the value of the context data structure */
  strcpy(context.obj_name, "");
  strcpy(context.ev_msg_name, "");
  context.ev_msg_type = 0;
  context.act_num = 0;
  context.user_code = 0;
}

void print_context()
{
  fprintf ( stdout, "        Object: %s\n", context.obj_name);
  if (context.ev_msg_type == CONTEXT_EV_TYPE)
    fprintf ( stdout, "        Event: %s\n", context.ev_msg_name);
  else
    fprintf ( stdout, "        Message: Delivered on port %s\n", context.ev_msg_name);
  fprintf ( stdout, "        Action number: %d\n", context.act_num);
}
