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

/*------------------------------------------------------------------------------
//        T A N G R A M - I I      << . >>      L A N D - U F R J - B R A Z I L
//----------------------------------------------------------------------------*/

/* Note: ev_name_param AND obj_st can't be used as a user identifier. */

/* the "_include" state is used for picking up
   the name of an include file */
%x _include

/* '_ifdef' state: code has entered an #ifdef section
   '_ignore_tokens': will ignore all token, until #endif is found */
%x _ifdef _ignore_tokens

/* '_page_index' state: begin object name parsing in a TGIF multipage model */
%x _page_index

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gramatica.bison.h"

#include "gramatica.h"
#include "code_generator.h"

int       mode;
t_status  current_status;
int       Can_Generate_Chain;
int       Can_Simulate;
int       has_mtk_object;   /* set true when a mtk object is found
                               used to instruct code_generator to include the
                               needed headers */
int       proc_token( int );
void      append_code( char *new_code );
void      ignore_comments( int );
void      skip_string( void );
int       check_for_st_var();
int       yyparse( void );

#define MAX_INCLUDE_DEPTH 100 /* TODO: implement a dynamic stack */
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
t_include_file  include_file[MAX_INCLUDE_DEPTH];
int             include_stack_ptr = 0;

//{COMMENT}          { proc_token(SPACE_TAB); } /* do nothing */

%}
/*------------------------------------------------------------------------------
//     D E F I N I T I O N S
//----------------------------------------------------------------------------*/
DIGIT    [0-9]
ID       [a-zA-Z]([_a-zA-Z0-9])*
IDIGIT   ([_a-zA-Z0-9])+
EDIGIT   [Ee][+-]?{DIGIT}+
COMMENT  ("//".*\n)|("/*"(.|\n)*"*/")

/*------------------------------------------------------------------------------
//     R U L E S
//----------------------------------------------------------------------------*/
%%
#include           { BEGIN(_include); }
<_include>[ \t]*       /* eat the whitespace */
<_include>\"[a-zA-Z0-9\/.-_]+\" { 
                        /* got the include file name */
                        if ( include_stack_ptr >= MAX_INCLUDE_DEPTH )
                        {
                            fprintf( stderr, "\n ==== Error while parsing the model ====\n");
                            fprintf( stderr, "                                          \n"
                                             "   Includes nested too deeply, aborting.  \n"
                                             "   Warning: it might be the case of       \n"
                                             "            self-recurrent including.     \n"
                                             "                                          \n");
                            exit( 1 );
                        }                        
                        
                        include_file[include_stack_ptr].linenumber--; /* workaround */
                        
                        include_stack[include_stack_ptr++] =
                        YY_CURRENT_BUFFER;

                        /* yytext[1] to yytext[yyleng-1]: trimming quotes */
                        yytext[yyleng-1] = '\0';

                        strcpy( include_file[include_stack_ptr].filename, yytext + 1 ); 
                        include_file[include_stack_ptr].linenumber = 0;

                        yyin = fopen( yytext + 1, "r" );

                        if ( ! yyin )
                        {
                            fprintf( stderr, "\nError while opening \'%s\', aborting!\n\n" , yytext + 1 );
                            exit( 1 );
                        }

                        yy_switch_to_buffer(
                        yy_create_buffer( yyin, YY_BUF_SIZE ) );

                        BEGIN(INITIAL);
                   }

#ifdef             { BEGIN( _ifdef ); }
<_ifdef>[ \t\f]+"SIMULATION" {
                                  if( mode == SIMULATION )
                                      BEGIN( INITIAL );
                                  else
                                      BEGIN( _ignore_tokens );
                             }
<_ifdef>[ \t\f]+"CHAIN_GENERATION" {
                                        if( mode == CHAIN_GENERATION )
                                            BEGIN( INITIAL );
                                        else
                                            BEGIN( _ignore_tokens );
                                   }
<_ignore_tokens,INITIAL>"#endif" { BEGIN( INITIAL ); }
<_ignore_tokens>.  {}

##                    { BEGIN( _page_index );                      }
<_page_index>{DIGIT}+ { current_status.tgif_page = atoi( yytext ); }
<_page_index>\!       { BEGIN( INITIAL );                          }

[ \t\f]+           { proc_token(SPACE_TAB); }       /* do nothing */
[\n]               { proc_token(CR_LF);     }
"/*"               { ignore_comments( 1 ); }    /* type 1 comment */
"//"               { ignore_comments( 2 ); }    /* type 2 comment */
Object_Desc        { return(proc_token(OBJECT_DESC)); }
Declaration        { return(proc_token(DECL));  }
Var                { return(proc_token(VAR));   }
Const              { return(proc_token(CONST)); }
Param              { return(proc_token(PARAM)); }
State              { return(proc_token(STATE));   }
StateFloat         { return(proc_token(FLOATSTATE));   }
IntegerQueue       { return(proc_token(INTQUEUE));   }
FloatQueue         { return(proc_token(FLOATQUEUE));   }
Float              { return(proc_token(FLOAT));   }
Integer            { return(proc_token(INTEGER)); }
MTKObject          { return(proc_token(MTK_OBJECT)); }
Object             { return(proc_token(OBJECT)); }
Port               { return(proc_token(PORT)); }
port               { return(proc_token(PORT_)); }
Reward             { return(proc_token(REWARD)); }
Initialization     { return(proc_token(INITIALIZATION)); }
Events             { return(proc_token(EVENTS)); }
event              { return(proc_token(EVENT)); }
clone_ev           { return(proc_token(CLONE_EV)); }
condition          { return(proc_token(CONDITION)); }
action             { return(proc_token(ACTION)); }
prob               { return(proc_token(PROB)); }
set_st             { return(proc_token(SET_ST)); }
get_st             { return(proc_token(GET_ST)); }
save_at_head       { return(proc_token(SAVE_AT_HEAD)); }
save_at_tail       { return(proc_token(SAVE_AT_TAIL)); }
restore_from_head      { return(proc_token(RESTORE_FROM_HEAD)); }
restore_from_tail      { return(proc_token(RESTORE_FROM_TAIL)); }
Messages               { return(proc_token(MESSAGES)); }
msg_rec                { return(proc_token(MSG_REC)); }
msg                    { return(proc_token(MSG)); }
get_msg_data           { return( proc_token( GET_MSG_DATA ) ); }
msg_source             { return(proc_token(MSG_SOURCE)); }
msg_data               { return(proc_token(MSG_DATA)); }
msg_type               { return(proc_token(MSG_TYPE)); }
msg_size               { return(proc_token(MSG_SIZE)); }

MSG_TYPE_SCALAR        { return( proc_token( MSG_TYPE_DEFINE ) ); }
MSG_TYPE_INT_VECTOR    { return( proc_token( MSG_TYPE_DEFINE ) ); }
MSG_TYPE_FLOAT_VECTOR  { return( proc_token( MSG_TYPE_DEFINE ) ); }

objcmp             { return(proc_token(OBJ_CMP)); }
all                { return(proc_token(ALL_OBJS)); }

copy               { return( proc_token( COPY ) ); }

 /* MTK functions */
mtk_create  { return( proc_token( MTK_CREATE ) ); }
mtk_delete  { return( proc_token( MTK_DELETE ) ); }
mtk_run     { return( proc_token( MTK_RUN    ) ); }
mtk_copy    { return( proc_token( MTK_COPY   ) ); }
mtk_get     { return( proc_token( MTK_GET    ) ); }
mtk_set     { return( proc_token( MTK_SET    ) ); }

EXP                { return(proc_token(EXP)); }
DET                { return(proc_token(DET)); }
UNI                { return(proc_token(UNI)); }
GAUSS              { return(proc_token(GAUSS)); }
ERLANG             { return(proc_token(ERLANG)); }
LOGNORM            { return(proc_token(LOGNORM)); }
WEIB               { return(proc_token(WEIB)); }
PAR                { return(proc_token(PAR)); }
TRUNCPAR           { return(proc_token(TRUNCPAR)); }
FILE               { return(proc_token(FILE_)); }
FBM                { return(proc_token(FBM)); }
FARIMA             { return(proc_token(FARIMA)); }

REWARD_REACHED     { return(proc_token(REWARD_REACHED)); }
"/\\"              { return(proc_token(UPP_REACHED)); }
"\\/"              { return(proc_token(LOW_REACHED)); }

INIT               { return(proc_token(INIT)); }

Global_Rewards     { return(proc_token(GLOBAL_REWARDS)); }
Rewards            { return(proc_token(REWARDS)); }
rate_reward        { return(proc_token(RATE_REWARD)); }
impulse_reward     { return(proc_token(IMPULSE_REWARD)); }
rate_reward_sum    { return(proc_token(RATE_REWARD_SUM)); }
rewards            { return(proc_token(REWARDS4SUM)); }
bounds             { return(proc_token(BOUNDS)); }
cr_bounds          { return(proc_token(BOUNDS)); }
value              { return(proc_token(VALUE)); }
cr_init_val        { return(proc_token(CR_INIT_VAL)); }
get_cr             { return(proc_token(GET_CR)); }
set_cr             { return(proc_token(SET_CR)); }
get_ir             { return(proc_token(GET_IR)); }
set_ir             { return(proc_token(SET_IR)); }
unset_ir           { return(proc_token(UNSET_IR)); }
get_cr_sum         { return(proc_token(GET_CR_SUM)); }
get_simul_time     { return(proc_token(GET_SIMUL_TIME)); }
get_random         { return(proc_token(GET_RANDOM)); }

Independent_Chains { return(proc_token(INDEP_CHAINS)); }
chain              { return(proc_token(CHAIN)); }

TRUE               { return(proc_token(TRUE_)); }
FALSE              { return(proc_token(FALSE_)); }

int                { return( proc_token(INT) );        }
long               { return( proc_token(LONG) );       }
float              { return( proc_token(C_FLOAT) );    }
double             { return( proc_token(C_FLOAT) );    }
INF                { return( proc_token(INF) );        }

"="                { return(proc_token(ASSIGNMENT)); }
if                 { return(proc_token(IF)); }
else               { return(proc_token(ELSE)); }
while              { return(proc_token(WHILE)); }
do                 { return(proc_token(DO)); }
switch	           { return(proc_token(SWITCH));}
case	           { return(proc_token(CASE));}
break              { return(proc_token(BREAK));}
default            { return(proc_token(DEFAULT));}
for                { return(proc_token(FOR));}

printf             { return(proc_token(PRINTF));}
fprintf            { return(proc_token(FPRINTF));}
stdout             { return(proc_token(STDOUT));}
stderr             { return(proc_token(STDERR));}

exp                { return(proc_token(EXP_FUNC)); }
frexp              { return(proc_token(FREXP_FUNC)); }
ldexp              { return(proc_token(FREXP_FUNC)); }
log                { return(proc_token(LOG_FUNC)); }
log10              { return(proc_token(LOG10_FUNC)); }

pow                { return(proc_token(POW_FUNC)); }
sqrt               { return(proc_token(SQRT_FUNC)); }

cos	               { return(proc_token(COS_FUNC)); }
sin                { return(proc_token(SIN_FUNC)); }
tan	               { return(proc_token(TAN_FUNC)); }
acos               { return(proc_token(ACOS_FUNC)); }
asin               { return(proc_token(ASIN_FUNC)); }
atan               { return(proc_token(ATAN_FUNC));}
atan2              { return(proc_token(ATAN2_FUNC));}

cosh	           { return(proc_token(COSH_FUNC));}
sinh	           { return(proc_token(SINH_FUNC));}
tanh	           { return(proc_token(TANH_FUNC)); }
 
ceil	           { return(proc_token(CEIL_FUNC)); }
fabs	           { return(proc_token(FABS_FUNC)); }
floor	           { return(proc_token(FLOOR_FUNC)); }
fmod               { return(proc_token(FMOD_FUNC));}


"?"                { return(proc_token(INTERROG)); }
":"                { return(proc_token(COLON)); }
";"                { return(proc_token(SEMICOLON)); }
","                { return(proc_token(COMMA)); }
"\""               { return(proc_token(QUOTE)); }
"."                { return(proc_token(DOT)); }

"&&"               { return(proc_token(AND)); }
"||"               { return(proc_token(OR)); }
"&"                { return(proc_token(BAND)); }
"|"                { return(proc_token(BOR)); }
"!"                { return(proc_token(NOT)); }

"=="               { return(proc_token(EQUAL)); }
"!="               { return(proc_token(NOTEQUAL)); }
"<="               { return(proc_token(LE)); }
"<"                { return(proc_token(LT)); }
">="               { return(proc_token(GE)); }
">"                { return(proc_token(GT)); }
"+"                { return(proc_token(PLUS)); }
"-"                { return(proc_token(MINUS)); }
"/"                { return(proc_token(SLASH)); }
"\\"               { return(proc_token(BACKSLASH)); }
"*"                { return(proc_token(STAR)); }
"%"                { return(proc_token(MOD)); }
">>"               { return(proc_token(RSHIFT)); }
"<<"               { return(proc_token(LSHIFT)); }
"("                { return(proc_token(LPAREN)); }
")"                { return(proc_token(RPAREN)); }
"{"                { return(proc_token(LBRACES)); }
"}"                { return(proc_token(RBRACES)); }
"["                { return(proc_token(LBRACKET)); }
"]"                { return(proc_token(RBRACKET)); }

"++"               { return(proc_token(INC_OP)); }
"--"               { return(proc_token(DEC_OP)); }

"+="               { return( proc_token(C_ASSIGNMENT) ); }
"-="               { return( proc_token(C_ASSIGNMENT) ); }
"*="               { return( proc_token(C_ASSIGNMENT) ); }
"/="               { return( proc_token(C_ASSIGNMENT) ); }

{ID}                            { return(proc_token(IDENTIFIER)); }
{ID}"."{IDIGIT}("."{IDIGIT})+   { return(proc_token(MULT_COMPOUND_IDENTIFIER)); }
{ID}"."{IDIGIT}                 { return(proc_token(COMPOUND_IDENTIFIER)); }
{DIGIT}+                        { return(proc_token(DIGSEQ)); }
{DIGIT}*"."{DIGIT}+             { return(proc_token(REALNUMBER)); }
{DIGIT}+{EDIGIT}                { return(proc_token(DIGSEQEXP)); }
{DIGIT}*"."{DIGIT}+{EDIGIT}     { return(proc_token(REALNUMBEREXP)); }

.                  { return ILLEGAL ;}

<<EOF>>            {
                       if ( --include_stack_ptr < 0 )
                       {
                           yyterminate();
                       }
                       else
                       {
                           yy_delete_buffer( YY_CURRENT_BUFFER );
                           yy_switch_to_buffer(
                                include_stack[include_stack_ptr] );
                       }
                   }

%%
//------------------------------------------------------------------------------
//     U S E R   C O D E
//------------------------------------------------------------------------------

int yywrap()
{
    return 1;
}

/******************************************************************************/
/* Analyse the current token contained in the variable token and take the     */
/* apropriated actions according to it. The user code beeing analysed can be  */
/* forwarded in some cases by this routine.                                   */
/******************************************************************************/
int proc_token( int token )
{
    static int prev_token = 0;
    static int state      = 0;

    /* increment the current line number */
    if( prev_token == CR_LF )
    {
        current_status.line_number++;
        include_file[include_stack_ptr].linenumber++;
    }

    /* set the object name */
    if( prev_token == OBJECT_DESC )
        strcpy( current_status.obj_name, yytext );

    /* set the event name */
    if( prev_token == EVENT )
        strcpy( current_status.ev_msg_name, yytext );

    /* set the message name (identifier) */
    if( prev_token == MSG_REC )
        sprintf( current_status.ev_msg_name,
                 "msg_rec number %d of message attribute",
                 current_status.msg_number );

    /* MTK commands will produce their own translation */
    if( token == MTK_CREATE ||
        token == MTK_DELETE ||
        token == MTK_RUN    ||
        token == MTK_COPY   ||
        token == MTK_GET    ||
        token == MTK_SET     )
        current_status.app_code = 0;

    /* get the code for the action or for the condition */
    if( current_status.app_code == 1 )
    {
        if( token == C_FLOAT )
        {
            /* substitutes C_FLOAT type for the TGFLOAT definition */
            append_code( "TGFLOAT" );
        }
        else
        {
            /* Just copies the string in its place */
            append_code( yytext );
        }
    }
    /* set the code capture flag on */
    if( ( prev_token == ACTION ) && ( token == ASSIGNMENT ) ) 
    {
	    strcpy( current_status.code, "" );
	    current_status.app_code = 1;
    }
    /* update last line read and current line */
    if( token == CR_LF )
    {
        strcpy( current_status.last_line, current_status.current_line );
        strcpy( current_status.current_line, "" );
    }
    strcat( current_status.current_line, yytext );
    /* ev_msg_number and action_number are set on reduction rules in .y */

    /* to skip spaces and tabs and assignment tokens to get the name */
    if( ( prev_token == OBJECT_DESC ) ||
        ( prev_token == EVENT       ) ||
        ( prev_token == MSG_REC     ) )
    {
	    if( token == SPACE_TAB || token == ASSIGNMENT )
        {
	        return token;
        }
    }

    /* to skip spaces and tabs to get the code */
    if( ( prev_token == ACTION ) && ( token == SPACE_TAB ) )
        return token;

    if( ( state == PRINTF ) && ( token == QUOTE ) )
    {
        skip_string();
        state = 0;
        return STRING;
    }

    if( token == PRINTF || token == FPRINTF )
        state = PRINTF;

    /* get previous token */
    prev_token = token;

    return token;
}
/******************************************************************************/
/* This routine adds the code pointed by new_code in the internal structures, */
/* meaning that this code will be present in the <model_name>.user_code.c     */
/******************************************************************************/
void append_code( char *new_code )
{
    char *temp_code;
    /* If we have an overflow in the code buffer, increase it */
    if( strlen( current_status.code )+strlen( new_code ) >= 
                                   current_status.code_size )
    {

        current_status.code_size += ( (int)((1.0 * strlen(new_code) / 
                                       CODE_SIZE_INCREASE) + 1.0)
                                                     * CODE_SIZE_INCREASE );
        temp_code = (char *)malloc( current_status.code_size  );
        strcpy( temp_code, current_status.code );

        if( current_status.code )
            free( current_status.code );

        current_status.code = temp_code;
    }
    strcat( current_status.code, new_code );
}
/******************************************************************************/
/* skip all comments in the user code. This routine is called whenever the    */
/* parser finds a comment marker. The comments are NOT copied to the internal */
/* structures meaning that it will not be present in <model_name>.user_code.c */
/******************************************************************************/
void ignore_comments( int type )
{
    int ant, act;

    /* type 1 comment (this one) */
    if( type == 1 )
    {
        ant = act = (int) ' ';
        while ( !( (ant == '*') && (act == '/') ) && (act != EOF) )
        {
            if( act == '\n' )
                current_status.line_number++;

            ant = act;
            act = input();
        }
    }
    else /* type 2 comment (//) */
    {
        ant = act = (int) ' ';
        while( !(act == '\n') && (act != EOF) )
        {
            ant = act;
            act = input();
        }
        if( act == '\n' )
            current_status.line_number++;
    }
}
/******************************************************************************/
/* This function is used to avoid parser inside quotes. The code enclosed by  */
/* quotes is copied to the internal structures as it is. This is beeing used  */
/* to parser the strings in the printf, fprintf and mtk_* commands.           */
/******************************************************************************/
void skip_string( void )
{
    int ant, act, size;
    char line[ 255 ];

    ant = act = (int) ' ';
    size = 0;
    while( !( (ant != '\\') && (act == '\"') ) && (act != EOF) )
    {
        ant = act;
        act = input();
        line[ size ] = act;
        size++;
        line[ size ] = 0;
        if( size == 255 )
        {
            append_code( line );
            size = 0;
        }
    }

    if( size > 0 )
        append_code( line );
}
//------------------------------------------------------------------------------
void usage( char *name )
{
    printf( "Usage: %s <-G|-S> <input filename> <base filename> <debug level>\n", name );
    printf( "\t<-G|-S>          - -G: Mathematical Model Solution; -S Simulation Model Solution\n" );
    printf( "\t<input filename> - file to be parsed\n" );
    printf( "\t<base filename>  - base filename of generated files\n" );
    printf( "\t<debug level>    - debug level of output (0 = no debug)\n" );
}
//------------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    char input_filename[ MAX_STRING ];
    char base_filename[ MAX_STRING ];
    int  Debug_Level = 0;

    mode = -1;

    if( argc != 5 )
    {
        usage( argv[ 0 ] );
        return( -1 );
    }

    if( strcasecmp( argv[1], "-S" ) == 0 )
        mode = SIMULATION;
    else if( strcasecmp( argv[1], "-G" ) == 0 )
        mode = CHAIN_GENERATION;

    if( mode == -1 )
    {
        usage( argv[ 0 ] );
        return( -1 );
    }

    strcpy( input_filename, argv[ 2 ] );
    strcpy( base_filename,  argv[ 3 ] );
    Debug_Level = atoi( argv[ 4 ] );

    strcpy( include_file[include_stack_ptr].filename, argv[ 2 ] ); 
    include_file[include_stack_ptr].linenumber = 0;
    
    initialize_current_status();

    /* initially, any model can be used for simulation or chain generation */
    Can_Generate_Chain = 1;
    Can_Simulate       = 1;
    
    /* at first, the model does not have any mtk object */
    has_mtk_object = 0;

    if( ( yyin = fopen( input_filename, "r" ) ) == NULL )
    {
        printf( "Error while opening the file: %s\n", input_filename );
        perror( "fopen" );
        exit( 1 );
    }

    if( yyparse() == 0 )
    {
        /* check for at least one state variable in the model */
        if( !check_for_st_var() )
        {
            printf( "Error: No state variable defined in the model." );
            printf( "The model should have at least one state variable.\n" );
            return( -1 );
        }
        /* initialize the maximum value of each state variable */
        if( mode )
        {
            if( initialize_max_values( base_filename ) < 0 )
                return( -1 );
            if( reorder_objects_and_variables( base_filename ) < 0 )
                return( -1 );
        }
        /* initialize the maximum value of each state variable */
        /* check to see if the initialization of object parameters exists */
        if( check_object_initialization() < 0 )
            return( -1 );
        /* print all the tables and code parsed */
        if( Debug_Level > 0 )
        {
            print_tables();
            print_global_rew_tb();
            print_indep_chain_tb();
        }
        /* Model has been parsed correctly */
        printf( "Model parsed correctly.\n" );
        fflush( stdout );

        /* generate the user_code.c file */
        code_generator( base_filename );

        /* generate the modelname.vstat file  */
        generate_vstat( base_filename );

        /* generate the modelname.events file */
        generate_events( base_filename );

        /* dump the parse tables to a file */
        dump_tables( base_filename );

        return( 0 );
    }
    return( -1 );
}
//------------------------------------------------------------------------------
