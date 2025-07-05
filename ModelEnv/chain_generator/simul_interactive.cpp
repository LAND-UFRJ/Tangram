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
*  @file simul_interactive.cpp
*  @brief <b>Simulator_Interactive methods</b>.
*  @remarks Lot of.
*  @author LAND/UFRJ
*  @date 1999-2009
*  @warning Do not modify this class before knowing the whole Tangram-II project
*  @since version 1.0
*
*  The detailed description is unavailable. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

#include "prototypes.h"
#include "general.h"
#include "chained_list.h"
#include "system_state.h"
#include "system_desc.h"
#include "random.h"
#include "simulator.h"
#include "gramatica.h"
#include "distribution.h"

#define XOR(a,b) (((a) && !(b)) || (!(a) && (b)))

/* the global system description */
extern System_Description *the_system_desc;
/* global Debug_Level */
extern int                 Debug_Level;
/* output file */
extern FILE *output_file;

Simulator_Interactive::Simulator_Interactive(Simulator *a_simul)
{
    /* initialize the simulator */
    simulator = a_simul;

    packet_queue          = NULL;
    packet_queue_size     = 0;
    packet_queue_max_size = 0;

    received_packets           = NULL;
    number_of_received_packets = 0;

    debug(4,"Simulator_Interactive::Simulator_Interactive(): creating object");
}

Simulator_Interactive::~Simulator_Interactive()
{
    /* do nothing */
    debug(4,"Simulator_Interactive::~Simulator_Interactive(): destroying object");
}

int Simulator_Interactive::connect_to_interface( const char *service)
{
#ifndef INADDR_NONE
#define INADDR_NONE       0xffffffff
#endif
    char   host[80];
    char   protocol[] = "udp";
    struct hostent    *phe;
    struct servent    *pse;
    struct protoent   *ppe;
    struct sockaddr_in sin;
    int s;
    int type;
    char buffer; /* dummy buffer */
    fd_set soc_pool;
    int    max_desc;
    struct timeval timeout;

    /* get the name of the host */
    /* gethostname(host, 80); */
    strcpy (host, "localhost");

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
        
    if ( (pse = getservbyname(service,protocol)) != NULL)
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port = htons((u_short)atoi(service)))==0) {
        fprintf( stderr, "Can't get \"%s\" service entry\n",service);
        return(-1);
    }

    if ( (phe = gethostbyname(host)) != NULL)
        bcopy(phe->h_addr,(char *)&sin.sin_addr, phe->h_length);
    else if ((sin.sin_addr.s_addr = inet_addr(host))==INADDR_NONE) {
        fprintf( stderr, "Can't get \"%s\" host entry\n",host);
        return(-1);
    }

    if ((ppe = getprotobyname(protocol)) == NULL) {
        fprintf( stderr, "Can't get \"%s\" protocol entry\n",protocol);
        return(-1);
    }

    if (strcmp(protocol,"udp")==0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket(PF_INET, type, ppe->p_proto);
    if (s<0) {
        fprintf( stderr, "Can't create socket!\n");
        return(-1);
    }
	
    if (connect(s,(struct sockaddr *)&sin, sizeof(sin))<0) {
        fprintf( stderr, "Can't connect to %s.%s\n",host,service);
        return(-1);
    }

    /* send a fake packet to the interface */
    if (send (s, &buffer, sizeof(buffer), 0) < 0) {
        perror("send");
        return(-1);
    }

    /* zero the pool and set the bits corresponding to the socket  */
    FD_ZERO(&soc_pool);
    FD_SET(s, &soc_pool);
    max_desc = s + 1;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if ( select(max_desc, &soc_pool, NULL, NULL, &timeout) < 0) {
        perror ("select");
        return(-1);
    }

    if (FD_ISSET(s, &soc_pool)) {
        /* receive fake packet from tgif */
        if (recv (s, &buffer, sizeof(buffer), 0)  < 0 ) {
            perror ("recv");
            return(-1);
        }
    } else {
        fprintf( stderr, "Simulator Timeout expired!\n");
        return(-1);
    }

    fprintf( stdout, "Simulator connected.\n");
    return s;       
}

int Simulator_Interactive::sendSymbolPacket( int soc, T_SymbolPacket * pack_out )
{
    int  bywrite;

    if( (bywrite = write( soc, pack_out, sizeof(T_SymbolPacket) )) < 0 )
    {
        perror( "write" );
        return(-1);
    }
    return(1);
}

int Simulator_Interactive::recvSymbolPacket( int soc, T_SymbolPacket * pack_in )
{
    int  byread;

    if( (byread = read( soc, pack_in, sizeof(T_SymbolPacket)) ) < 0 )
    {
        perror( "read" );
        return(-1);
    }
    return(1);
}

int Simulator_Interactive::sendControlPacket( int soc, T_ControlPacket * pack_out )
{
    int  bywrite;

    if( (bywrite = write( soc, pack_out, sizeof(T_ControlPacket) )) < 0 )
    {
        perror( "write" );
        return(-1);
    }
    return(1);
}

int Simulator_Interactive::recvControlPacket( int soc, T_ControlPacket * pack_in )
{
    int  byread;

    if( (byread = read( soc, pack_in, sizeof(T_ControlPacket)) ) < 0 )
    {
        perror( "read" );
        return(-1);
    }
    return(1);
}

void Simulator_Interactive::addSymToBuffer( T_SymbolPacket * packet )
{
    if( !packet_queue )
    {
        packet_queue_max_size = 8;
        packet_queue = (T_SymbolPacket *)malloc( packet_queue_max_size *
                                                sizeof(T_SymbolPacket) );
    }
    else if( packet_queue_size+1 > packet_queue_max_size ) /* queue overflow */
    {
        packet_queue_max_size <<= 1;
        packet_queue = (T_SymbolPacket *)realloc( packet_queue,
                                                 packet_queue_max_size *
                                                 sizeof(T_SymbolPacket) );
    }

    memcpy( packet_queue+packet_queue_size, packet, sizeof(T_SymbolPacket) );
    packet_queue_size++;
}

void Simulator_Interactive::sendAllSymPackets( int soc )
{
    int i;

    for( i = 0; i < packet_queue_size; i++ )
        sendSymbolPacket( soc, packet_queue+i );

    if( packet_queue )
    {
        free( packet_queue );
        packet_queue = NULL;
        packet_queue_max_size = 0;
    }

    packet_queue_size = 0;
}

int Simulator_Interactive::get_symbol_info( char * object_name,
                                            char * symbol_name,
                                            SymbolInfo * symbol_info,
                                            System_State * sys_st )
{
    Object_Description * obj_desc;
    Object_State       * obj_st;
    Symbol             * symbol;
    int                  code;
    int                  length;
    Object_State_List  * obj_st_list;
    int                  obj_id;
    int                  options;

    obj_desc = NULL;
    obj_st   = NULL;
    symbol   = NULL;

    if( !object_name || !symbol_name || !symbol_info || !sys_st )
        return FALSE;

    /* get object state for the correct object */
    if( !symbol_info->obj_desc ||
        !symbol_info->obj_st ||
        strcmp( symbol_info->object_name, object_name ) != 0 )
    {
        obj_desc = the_system_desc->show_obj_desc( object_name );

        if( !obj_desc )
            return FALSE;

        obj_st_list = sys_st->show_obj_st_list( LIST_RO );
        obj_id      = obj_desc->show_id( );

        if( obj_st_list->query_obj_st( obj_id ) )
            obj_st = obj_st_list->show_curr_obj_st( );

        delete obj_st_list;

        if( !obj_st )
            return FALSE;

        symbol_info->obj_desc = obj_desc;
        symbol_info->obj_st   = obj_st;

        symbol_info->symbol = NULL;
        symbol_info->code   = -1;
        symbol_info->length = -1;
    }
    else
    {
        obj_desc = symbol_info->obj_desc;
        obj_st   = symbol_info->obj_st;
    }

    strcpy( symbol_info->object_name, object_name );

    /* get symbol information */
    if( !symbol_info->symbol ||
        symbol_info->code < 0 ||
        symbol_info->length < 0 ||
        strcmp( symbol_info->symbol_name, symbol_name ) != 0 )
    {
        symbol = obj_desc->show_symbol     ( symbol_name );
        code   = obj_desc->show_symbol_code( symbol_name );

        if( !symbol || code < 0 )
            return FALSE;

        options = 0;
        switch( symbol->show_type( ) )
        {
        case STATE_VAR_TYPE:
            options |= SI_OPT_IS_ST_VAR;
            options |= SI_OPT_IS_INTEGER;
            break;
        case STATE_VAR_FLOAT_TYPE:
            options |= SI_OPT_IS_ST_VAR;
            options |= SI_OPT_IS_FLOAT;
            break;
        case REWARD_TYPE:
        case REWARD_SUM_TYPE:
            options |= SI_OPT_IS_REWARD;
            options |= SI_OPT_IS_FLOAT;
            break;
        default:
            return FALSE;
        }

        length = obj_st->show_st_var_dimension( code );
        if( length > 1 )
            options |= SI_OPT_IS_ARRAY;

        symbol_info->symbol  = symbol;
        symbol_info->code    = code;
        symbol_info->options = options;
        symbol_info->length  = length;
    }

    strcpy( symbol_info->symbol_name, symbol_name );

    return TRUE;
}

int Simulator_Interactive::get_symbol_value( const SymbolInfo * symbol_info,
                                             T_SymbolPacket * pack_out )
{
    int index;
    int  *int_values, int_value;
    TGFLOAT *float_values, float_value;

    int_value   = 0;
    float_value = 0;

    /* Check for inconsistencies in array specification */
    if( XOR( (pack_out->options & SP_OPT_IN_ARRAY),
             (symbol_info->options & SI_OPT_IS_ARRAY) ) )
        return FALSE;

    /* Determine whether the symbol is a vector to extract the access index */
    if( pack_out->options & SP_OPT_IN_ARRAY )
    {
        /* Check whether access overflows the vector */
        if( pack_out->array_index >= symbol_info->length )
            return FALSE;

        index = pack_out->array_index;
    }
    else
        index = 0;

    /* Determine whether symbol is a state variable or a reward */
    if( symbol_info->options & SI_OPT_IS_ST_VAR )
    {
        if( symbol_info->options & SI_OPT_IS_INTEGER )
        {
            int_values = symbol_info->obj_st->show_st_var_value(
                                                           symbol_info->code );
            int_value  = int_values[index];
        }
        else if( symbol_info->options & SI_OPT_IS_FLOAT )
        {
            float_values = symbol_info->obj_st->show_st_float_var_value(
                                                           symbol_info->code );
            float_value  = float_values[index];
        }
        else /* someone might implement another kind of State Variable */
            return FALSE;
    }
    else if( symbol_info->options & SI_OPT_IS_REWARD )
    {
        if( !(symbol_info->options & SI_OPT_IS_FLOAT) )
            return FALSE;

        float_value = simulator->get_cumulative_reward( symbol_info->obj_st,
                                                        pack_out->symbol_name );
    }
    else
        return FALSE;

    if( pack_out->options & SP_OPT_HAS_FORMAT )
    {
        if( symbol_info->options & SI_OPT_IS_INTEGER )
            sprintf( pack_out->value_string, pack_out->value_format,
                     int_value );
        else if( symbol_info->options & SI_OPT_IS_FLOAT )
            sprintf( pack_out->value_string, pack_out->value_format,
                     float_value );
        else
            return FALSE;
    }
    else
    {
        if( symbol_info->options & SI_OPT_IS_INTEGER )
            sprintf( pack_out->value_string, "%d", int_value );
        else if( symbol_info->options & SI_OPT_IS_FLOAT )
            sprintf( pack_out->value_string, "%g", float_value );
        else
            return FALSE;
    }

    return TRUE;
}

int Simulator_Interactive::set_symbol_value( const SymbolInfo * symbol_info,
                                             T_SymbolPacket * pack_in )
{
    int  *int_values, int_value;
    TGFLOAT *float_values, float_value;

    /* Check for inconsistencies in array specification */
    if( XOR( (pack_in->options & SP_OPT_IN_ARRAY),
             (symbol_info->options & SI_OPT_IS_ARRAY) ) )
        return FALSE;

    /* Get symbol value from packet */
    int_value = strtol( pack_in->value_string, NULL, 0 );
    float_value = strtod( pack_in->value_string, NULL );

    /* Determine whether the symbol is a vector */
    if( pack_in->options & SP_OPT_IN_ARRAY )
    {
        /* Check whether assignment overflows the vector */
        if( pack_in->array_index >= symbol_info->length )
            return FALSE;

        /* Determine whether symbol is a state variable or a reward */
        if( symbol_info->options & SI_OPT_IS_ST_VAR )
        {
            if( symbol_info->options & SI_OPT_IS_INTEGER )
            {
                int_values = symbol_info->obj_st->show_st_var_vec_value(
                                                        pack_in->symbol_name );

                int_values[ pack_in->array_index ] = int_value;
                symbol_info->obj_st->ch_st_var_value( pack_in->symbol_name,
                                                      int_values );
            }
            else if( symbol_info->options & SI_OPT_IS_FLOAT )
            {
                float_values = symbol_info->obj_st->show_st_var_fvec_value(
                                                        pack_in->symbol_name );

                float_values[ pack_in->array_index ] = float_value;
                symbol_info->obj_st->ch_st_var_value( pack_in->symbol_name,
                                                      float_values );
            }
            else /* someone might implement another kind of State Variable */
                return FALSE;
        }
        else /* someone might implement reward vectors */
            return FALSE;
    }
    else
    {
        /* Determine whether symbol is a state variable or a reward */
        if( symbol_info->options & SI_OPT_IS_ST_VAR )
        {
            if( symbol_info->options & SI_OPT_IS_INTEGER )
                symbol_info->obj_st->ch_st_var_value( pack_in->symbol_name,
                                                      int_value );
            else if( symbol_info->options & SI_OPT_IS_FLOAT )
                symbol_info->obj_st->ch_st_var_value( pack_in->symbol_name,
                                                      float_value );
            else /* someone might implement another kind of State Variable */
                return FALSE;
        }
        else if( symbol_info->options & SI_OPT_IS_REWARD )
        {
            if( !(symbol_info->options & SI_OPT_IS_FLOAT) )
                return FALSE;

            simulator->set_cumulative_reward( symbol_info->obj_st,
                                              pack_in->symbol_name,
                                              float_value );
        }
        else
            return FALSE;
    }

    return TRUE;
}

void Simulator_Interactive::dismount_packet(int soc, T_ControlPacket * pack, System_State *sys_st)
{
    int i, j;
    T_SymbolPacket pack_in;
    SymbolInfo symbol_info;

    /* allocate maximum number of packets to be received */
    received_packets = (T_SymbolPacket *)malloc( pack->symcount *
                                                sizeof(T_SymbolPacket) );

    /* initially set the SymbolInfo to 0. subsequently the info is used
       as a cache inside get_symbol_info() */
    memset( &symbol_info, 0, sizeof(SymbolInfo) );

    /* receive and dismount each packet updating the current system state */
    for( i = 0, j = 0; i < pack->symcount; i++ )
    {
        recvSymbolPacket( soc, &pack_in );

        /* if this is not a valid symbol we won't even handle it */
        if( !get_symbol_info( pack_in.object_name, pack_in.symbol_name,
                              &symbol_info, sys_st ) )
            continue;

        /* add packet to received list */
        memcpy( received_packets+j, &pack_in, sizeof(T_SymbolPacket) );
        j++;

        /* if packet doesn't contain a value, we don't need to set anything */
        if( !(pack_in.options & SP_OPT_HAS_VALUE) )
            continue;

        /* set the value for the symbol */
        set_symbol_value( &symbol_info, &pack_in );
    }

    number_of_received_packets = j;
}

void Simulator_Interactive::mount_packet(System_State *sys_st)
{
    int i;
    T_SymbolPacket varpack;
    SymbolInfo    symbol_info;

    for( i = 0; i < number_of_received_packets; i++ )
    {
        if( !get_symbol_info( received_packets[i].object_name,
                              received_packets[i].symbol_name,
                              &symbol_info, sys_st ) )
            continue;

        /* prepare outgoing packet */
        memset( &varpack, 0, sizeof(T_SymbolPacket) );

        strcpy( varpack.object_name, received_packets[i].object_name );
        strcpy( varpack.symbol_name, received_packets[i].symbol_name );

        if( received_packets[i].options & SP_OPT_IN_ARRAY )
        {
            varpack.options |= SP_OPT_IN_ARRAY;
            varpack.array_index = received_packets[i].array_index;

            if( received_packets[i].options & SP_OPT_FIRST_IN_ARRAY )
                varpack.options |= SP_OPT_FIRST_IN_ARRAY;

            if( received_packets[i].options & SP_OPT_LAST_IN_ARRAY )
                varpack.options |= SP_OPT_LAST_IN_ARRAY;
        }

        if( received_packets[i].options & SP_OPT_HAS_FORMAT )
        {
            varpack.options |= SP_OPT_HAS_FORMAT;
            strcpy( varpack.value_format, received_packets[i].value_format );
        }

        /* put value into varpack.value_string properly formatted */
        if( get_symbol_value( &symbol_info, &varpack ) )
        {
            varpack.options |= SP_OPT_HAS_VALUE;
            addSymToBuffer( &varpack );
        }
    }

    free( received_packets );
    received_packets           = NULL;
    number_of_received_packets = 0;
}

void Simulator_Interactive::simulate_model_interactive()
{
    Transition        *trans;         /* the transition with impulse reward */
    System_State      *next_sys_st;   /* next system state after simualting */
                                      /* one transition */
    Simulator_Event   *simul_ev;
    Event_Sample      *ev_sample;

    int      soc;                      /* socket descriptor */
    T_ControlPacket pack_in;           /* packet received */
    T_ControlPacket pack_out;          /* packet sended */

    TGFLOAT curr_time;                 /* time before starting transitions */
    TGFLOAT time;                      /* elapsed time between transitions */
    int i;                             /* loop counter */
    char lastEventName[ MAXSTRING ];   /* Store the last event name  */
    char lastObjName[ MAXSTRING ];     /* Store the last object name */

/***************************************************************************/
    Distribution       *distrib;
    Rew_Reach_Distrib  *rew_reach_dist = NULL;
    Object_State       *obj_st = NULL;
    Object_State_List  *obj_st_list = NULL;
/***************************************************************************/

    /* wait for a connection from TGIF */
    if( (soc = connect_to_interface(PORT) ) < 0 )
    {
        fprintf( stderr, "simulator_model_interactive: Error in connecting to TGIF\n");
        return;
    }

    /* receive the packet from TGIF */
    if( recvControlPacket( soc, &pack_in ) < 0 )
        return;

    /* packet must be initialization */
    if( pack_in.code != CT_CODE_INIT_SIMULATION )
        return;

    /* Update initial model state */
    dismount_packet( soc, &pack_in, simulator->curr_sys_st );

    /* mount the outgoing state var packets */
    mount_packet( simulator->curr_sys_st );

    /* Send back to TGIF the initial simulation state */
    /* clear the outgoing packet */
    memset ( &pack_out, 0, sizeof(T_ControlPacket) );

    /* fill the rest of the outgoing packet */
    pack_out.code         = CT_CODE_INIT_SIMULATION;
    pack_out.symcount     = packet_queue_size;
    strcpy( pack_out.last_event, "None" );

    /* send the outgoing packet to TGIF */
    if( sendControlPacket( soc, &pack_out ) < 0 )
        return;

    sendAllSymPackets( soc );


    /* Perform simulation */
    /* while not end of simulation */
    while( 1 )
    {
        /* receive the packet from TGIF */
        if( recvControlPacket( soc, &pack_in ) < 0 )
            break;

        /* analyse the packet */
        /* end of simulation */
        if( pack_in.code == CT_CODE_END_OF_SIMULATION )
            break;

        /* step in simulation */
        next_sys_st = NULL;
        if( pack_in.code == CT_CODE_STEP_SIMULATION )
        {
            /* get the current time in the simulator */
            curr_time = simulator->simulation_time;

            dismount_packet( soc, &pack_in, simulator->curr_sys_st );

            /* Simulate pack_in.step transitions and accumulate the time */
            for( i = 0; i < pack_in.step; i++ )
            {
                /* get the event with the smallest time */
                simul_ev = simulator->event_list->show_smallest_time();
                if( simul_ev == NULL )
                {
                    fprintf( stderr, "WARNING: Current system state:\n");
                    simulator->curr_sys_st->print_sys_st();
                    fprintf( stderr, "WARNING: No more enabled events in event List.");
                    fprintf( stderr, "       Can't continue simulation\n");
                    return;
                }

/***************************************************************************/
                // If the event is REWARD_REACHED and
                // flag trigger_now is TRUE, disable flag.
                distrib = simul_ev->show_event()->show_distrib();
                if( distrib->show_type() == REW_REACH_DIST )
                {
                    rew_reach_dist = (Rew_Reach_Distrib *)distrib;
                    if( rew_reach_dist->show_trigger_now() == TRUE )
                        rew_reach_dist->ch_trigger_now( FALSE );
                }
/***************************************************************************/

                /* get the 1st (smallest) event sample */
                ev_sample = simul_ev->get_1st_ev_sample();

                /* update the simulation time */
                simulator->last_simul_time = simulator->simulation_time;
                simulator->simulation_time = ev_sample->show_time();

                /* update the number of triggers of this event */
                simul_ev->inc_no_triggers();

                /* Get the name of this event if it is the last event */
                if( i == pack_in.step - 1 )
                {
                    Event              *lastEvent;
                    Object_Description *lastObj;
                    lastEvent = simul_ev->show_event();
                    lastObj   = simul_ev->show_obj_desc();
                    lastEvent->show_name( lastEventName );
                    lastObj->show_name( lastObjName );
                }

                /* delete the sample being processed */
                delete ev_sample;

                /* update the rate reward for the current system state */
                simulator->update_rate_rewards( simul_ev );

/***************************************************************************/
                /* Verify the accuracy of this event.
                   If the event should not trigger at this time, skip the
                   other functionalities. This event is evaluated again and the
                   simulation continues.                                   */
                if( distrib->show_type() == REW_REACH_DIST )
                {
                    if( rew_reach_dist->show_skip_event() == TRUE )
                    {
                        /* Every user code will be skiped, so the obj will */
                        /* remain in the same state. Now get obj state!!   */
                        obj_st_list = simulator->curr_sys_st->show_obj_st_list(
                                                                 LIST_RO );
                        if( obj_st_list->query_obj_st(
                                       simul_ev->show_obj_desc()->show_id() ) )
                        {
                            obj_st = obj_st_list->show_curr_obj_st();
                        }
                        else
                        {
                            fprintf( stderr,
                                     "Internal ERROR: in simulate_model_interactive():"
                                     " obj_st not found!\n");
                            exit( -1 );
                        }

                        /* Evaluate the condition under this object state.
                           This is necessary only for the event that created the
                           fake trigger.
                           Note: nothing changes for the other events, so the
                           evaluation isn't necessary.
                           reset distrib before evaluate the parameter passed to
                           reset() are unused for this distrib. */
                        simul_ev->show_event()->reset_distrib( 0 );

                        if (simul_ev->eval_cond(obj_st) == TRUE)
                        {
                            // Completion of the evaluation - analysys of the
                            // every special argument.
                            simulator->eval_rr_special_args( rew_reach_dist );

                            /* NOTE: eval_cond could be false now case
                               ir_virtual = 0 */
                            /* resample the event.*/
                            simul_ev->generate_sample( obj_st,
                                                   simulator->simulation_time );
                        }

                        /* this is so we don't count this as one of the steps */
                        i--;
                        continue;
                    }
                }
/***************************************************************************/

                /* Simulate 1 step of the Model */
                next_sys_st = simulator->simulate_to_a_leaf( simul_ev );

                if( Debug_Level > 0 )
                    /* print the current system state */
                    simulator->curr_sys_st->print_sys_st();
                if( Debug_Level > 1 )
                    /* print the current event list */
                    simulator->print_ev_list();

                /* if impulse rewards were defined in this model */
                if( the_system_desc->show_impulse_reward() == TRUE )
                {
                    /* get the transition with the rewards */
                    trans = next_sys_st->get_1st_trans();
                    /* accumulate the reward in the current transition */
                    simulator->add_imp_rew_list( trans );
                    /* delete this transition */
                    delete trans;
                }

                /* delete the old state */
                delete simulator->curr_sys_st;
                /* update the current system state */
                simulator->curr_sys_st = next_sys_st;
            }

            /* Update the simulation time */
            time = simulator->simulation_time - curr_time;

            if( output_file == NULL )
            {
                fprintf( stderr,"Error: Output file is not defined!\n" );
                exit( 0 );
            }

            /* print the rate rewards values if rate rewards were defined in
              this model */
            if( the_system_desc->show_rate_reward() == TRUE && Debug_Level > 0 )
            {
                /* print the rewards for the simulation */
                fprintf( output_file, "Rate Rewards:\n" );
                simulator->rate_rew_list->print_rew( output_file );
            }

            /* print the impulse rewards if impulse rewards were defined in
               this model */
            if( the_system_desc->show_impulse_reward() == TRUE &&
                Debug_Level > 0 )
            {
                /* print the impulse rewards for the simulation */
                fprintf( output_file, "Impulse Rewards:\n" );
                simulator->imp_rew_list->print_rew( output_file );
            }

            /* mount the outgoing state var packets and add them to the queue */
            mount_packet( next_sys_st );

            /* clear the outgoing packet */
            memset ( &pack_out, 0, sizeof(T_ControlPacket) );

            /* fill the rest of the outgoing packet */
            pack_out.code         = CT_CODE_STEP_SIMULATION;
            pack_out.elapsed_time = time;
            pack_out.symcount     = packet_queue_size;

            sprintf( pack_out.last_event, "%s.%s", lastObjName, lastEventName );

            /* send the outgoing packet to TGIF */
            if( sendControlPacket( soc, &pack_out ) < 0 )
                break;

            sendAllSymPackets( soc );
        }
    }

    /* close the connection */
    close (soc);

    fprintf( stdout, "Mark_Find: Simulation finished\n");
}

Simulator * Simulator_Interactive::show_simulator()
{
    return simulator;
}
