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
#include <string.h>

#define MAX_CAR 800
#define SIZE_OBJ 400
#define SIZE_WORD 400

#define OBJECT_DESCRIPTION    -1
#define INIT        0
#define VARIABLE    1
#define COMENT      2
#define EVENT       3
#define REWARD      4

/* The following defines are used to control the returned error */
#define RETURN_OK 0

#define RETURN_ERROR_COULD_NOT_READ_PARSER_FILE 10
#define RETURN_ERROR_COULD_NOT_CREATE_STATE_VAR_FILE 11
#define RETURN_ERROR_COULD_NOT_CREATE_REWARD_FILE 12
#define RETURN_ERROR_COULD_NOT_CREATE_EVENTS_FILE 13

#define RETURN_ERROR_WRONG_OR_MISSING_ARGUMENT 14


int stateVarName( char * );
int eventName( char * );
int rewardLevelName( char * );
int seeArg( int argc, char * argv[] );

int main( int argc, char * argv[] )
{
    int return_value, input_argument;


    input_argument = seeArg( argc, argv );

    if( input_argument == 3 ) 
        return_value = rewardLevelName( argv[2] );
    else if( input_argument == 2 )
        return_value = stateVarName( argv[2] );
    else if( input_argument == 1 )
        return_value = eventName( argv[2] );
    else
        return( RETURN_ERROR_WRONG_OR_MISSING_ARGUMENT );

    return( return_value );
}


int seeArg( int argc, char *argv[] )
{
    int flag = 0;

    if( argc != 3 )
        flag = -1;
    else if( !strcmp( argv[1], "-e" ) )
        flag = 1;
    else if( !strcmp( argv[1], "-v" ) )
        flag = 2;
	else if( !strcmp( argv[1], "-r" ) )
        flag = 3;
    else
        flag = -1;

    if( flag == -1 )
    {
        fprintf( stderr,
                 "\nCorrect usage\n"
                 " getNames <-v|-e|-r> <base name>\n"
                 " -v get state var names and make <base name>.state_variable\n"
                 " -e get events names and make <base name>.events\n"
                 " -r get reward level names and make <base name>.rewards\n" );
    }
    return( flag );
}

int stateVarName( char *name )
{
    int i, controle, lastcontrol;
    char frase[MAX_CAR], objetoAtual[SIZE_OBJ], nomeArquivo[SIZE_WORD];
    char *palavra;
    FILE *ptIN , *ptOUTsv;

    lastcontrol = controle = INIT;

    sprintf( nomeArquivo, "%s.parser", name );
    ptIN = fopen( nomeArquivo, "r" );

    sprintf( nomeArquivo, "%s.state_variable", name );
    ptOUTsv = fopen( nomeArquivo,"w" );

    if( ptIN == NULL )
    {
        printf("Could not open %s.parser file (Error while executing getNames)\n",name);
        return( RETURN_ERROR_COULD_NOT_READ_PARSER_FILE );
    }
    if( ptOUTsv == NULL )
    {
        printf("getNames error: Could not create %s.state_variable file. \n",name);
        return( RETURN_ERROR_COULD_NOT_CREATE_STATE_VAR_FILE );
    }

    while( !feof( ptIN ) )
    {
        fgets( frase, MAX_CAR, ptIN );

        i = strlen( frase );

        if( frase[ --i ] == '\n' )
            frase[ i ]   = '\0';

        palavra = strtok( frase, " ,:;()" );

        while( palavra != NULL )
        {
            if( controle != COMENT )
            {
                if( !strncmp( palavra, "/*", 2 ) )
                {
                    lastcontrol = controle;
                    controle    = COMENT;
                }
                else
                {
                    if( controle == OBJECT_DESCRIPTION )
                    {
                        strcpy( objetoAtual,palavra );
                        controle = INIT;
                    }
                    else
                    {
                        if( !strcmp( palavra, "Object_Desc" ) )
                            controle = OBJECT_DESCRIPTION;
                        else
                        {
                            if( !strcmp( palavra, "Var" ) )
                                controle = VARIABLE;
                            else
                            {
                                /* { - Dumb comment to fix
                                   syntax highlight in Nedit */
                                if( !strcmp( palavra, "Param" ) ||
                                    !strcmp( palavra, "Const" ) ||
                                    palavra[ 0 ] == '}' )
                                /* Nedit syntax highlight problem ends here */
                                {
                                    controle = INIT;
                                }
                                else if ( !strcmp( palavra, "Float" ) ||
                                          !strcmp( palavra, "StateFloat" ) )
                                {
                                    fprintf( stderr,
                                             " Warning: Float variable will "
                                             "not be set in your model\n" );
                                }
                                else if ( !strcmp( palavra, "MTKObject" ) )
                                {
                                    fprintf( stderr,
                                             " Warning: MTKObject variables "
                                             "can only be used in simulation\n" );
                                }
                                else if( strcmp( palavra, "IntegerQueue" ) == 0
                                      || strcmp( palavra, "FloatQueue" ) == 0 )
                                {
                                    do
                                    {
                                        palavra = strtok( NULL, " ,()" );
                                        if( palavra == NULL )
                                        {
                                            fgets( frase, MAX_CAR, ptIN );
                                            palavra = strtok( frase, " ,()" );
                                        }
                                    } while( palavra[0] != ';' );
                                    if( palavra[1] != '\0' )
                                    {
                                        palavra++;
                                        continue;
                                    }
                                }
                                else if( controle &&
                                         strcmp( palavra, "State" ) &&
                                         strcmp( palavra, "Integer" ) )
                                {
                                    int num, tamanho;

                                    tamanho = strlen( palavra );
                                    for( num = 0; num < tamanho; num++ )
                                    {
                                        switch( palavra[num] )
                                        {
                                        case '[':
                                            palavra[num] = 0;
                                            lastcontrol  = controle;
                                            controle     = COMENT;
                                        break;
                                        case ']':
                                            controle = lastcontrol;
                                        break;
                                        }
                                    }
                                    if( strlen( palavra ) > 0 )
                                        fprintf( ptOUTsv, "%s.%s\n",
                                                 objetoAtual, palavra);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                 if( strstr( palavra,"*/" ) != NULL ||
                     palavra[ strlen( palavra ) - 1 ] == ']' )
                     controle = lastcontrol;
            }

            palavra = strtok( NULL, " ,:;()=" );
        }
    }

    fclose( ptIN );
    fclose( ptOUTsv );
    return( RETURN_OK );
}

int rewardLevelName(char *name)   			
{
    int i, controle, lastcontrol;
    int braces;
    char frase[MAX_CAR],objetoAtual[SIZE_OBJ], nomeArquivo[SIZE_WORD];
    char *palavra;

    FILE *ptIN , *ptOUT;

    lastcontrol = controle = INIT;
    braces = 0;

    sprintf(nomeArquivo,"%s.parser",name);
    ptIN=fopen(nomeArquivo,"r");

    sprintf(nomeArquivo,"%s.rewards",name);
    ptOUT=fopen(nomeArquivo,"w");

    if( ptIN == NULL )
    {
        printf("getNames error: Could not open %s.parser file \n",name);
        return( RETURN_ERROR_COULD_NOT_READ_PARSER_FILE );
    }
    if( ptOUT == NULL )
    {
        printf("getNames error: Could not create %s.rewards file. \n",name);
        return( RETURN_ERROR_COULD_NOT_CREATE_REWARD_FILE );
    }

    while(!feof(ptIN))
    {
        fgets(frase,MAX_CAR,ptIN);

        i=strlen(frase);

        if(frase[--i]=='\n') frase[i]='\0';

        palavra=strtok(frase,"= ,:;()");


        while(palavra!=NULL)
        {
            if( controle != COMENT )
            {
               if( !strncmp(palavra,"/*", 2) )
               {
                   lastcontrol = controle;
                   controle = COMENT;
               }

               else  if(controle==OBJECT_DESCRIPTION)
               {
                   strcpy(objetoAtual,palavra);
                   controle=INIT;
               }

               else if(!strcmp(palavra,"Object_Desc"))
                   controle=OBJECT_DESCRIPTION;

               else if(!strcmp(palavra, "Rewards"))
               {
                   controle=REWARD;
                   braces = 0;
                   palavra=strtok(NULL,"= ,:;()");
                   continue;
               }

               else if(((!strcmp(palavra,"rate_reward"))||(!strcmp(palavra,"impulse_reward"))) && (controle == REWARD)) controle=VARIABLE;

               else if(!strcmp(palavra,"}") && controle == REWARD) braces--;

               else if(!strcmp(palavra,"{") && controle == REWARD) braces++;

               else if( controle == REWARD && braces == 0 ) controle = INIT;

               else if(controle==VARIABLE)
               {
                   fprintf(ptOUT,"%s.reward_levels.%s.%s\n",name,objetoAtual,palavra); 				
	               controle=REWARD;				
               }

            }
            else
                if( strstr(palavra,"*/") != NULL)
                    controle = lastcontrol;

            palavra=strtok(NULL,"= ,:;()");

        }

    }

    fclose(ptIN);
    fclose(ptOUT);
    return ( RETURN_OK );

}

int eventName (char *name)
{
    int i, controle, lastcontrol;
    int braces;
    char frase[MAX_CAR],objetoAtual[SIZE_OBJ], nomeArquivo[SIZE_WORD];
    char *palavra;

    FILE *ptIN , *ptOUT;

    lastcontrol = controle = INIT;
    braces = 0;

    sprintf(nomeArquivo,"%s.parser",name);
    ptIN=fopen(nomeArquivo,"r");

    sprintf(nomeArquivo,"%s.events",name);
    ptOUT=fopen(nomeArquivo,"w");

    if( ptIN == NULL )
    {
        printf("getNames error: Could not open %s.parser file \n",name);
        return( RETURN_ERROR_COULD_NOT_READ_PARSER_FILE );
    }
    if( ptOUT == NULL )
    {
        printf("getNames error: Could not create %s.events file. \n",name);
        return( RETURN_ERROR_COULD_NOT_CREATE_EVENTS_FILE );
    }

    while(!feof(ptIN))
    {
        fgets(frase,MAX_CAR,ptIN);

        i=strlen(frase);

        if(frase[--i]=='\n') frase[i]='\0';

        palavra=strtok(frase,"= ,:;()");


        while(palavra!=NULL)
        {
            if( controle != COMENT )
            {
               if( !strncmp(palavra,"/*", 2) )
               {
                   lastcontrol = controle;
                   controle = COMENT;
               }

               else  if(controle==OBJECT_DESCRIPTION)
               {
                   strcpy(objetoAtual,palavra);
                   controle=INIT;
               }

               else if(!strcmp(palavra,"Object_Desc"))  controle=OBJECT_DESCRIPTION;

               else if(!strcmp(palavra, "Events"))
               {
                   controle=EVENT;
                   braces = 0;
                   palavra=strtok(NULL,"= ,:;()");
                   continue;
               }

               else if(!strcmp(palavra,"event") && controle == EVENT) controle=VARIABLE;

               else if(!strcmp(palavra,"}") && controle == EVENT) braces--;

               else if(!strcmp(palavra,"{") && controle == EVENT) braces++;

               else if( controle == EVENT && braces == 0 ) controle = INIT;

               else if(controle==VARIABLE)
                {
                fprintf(ptOUT,"%s.%s\n",objetoAtual,palavra);
                controle=EVENT;
                }

            }
            else
                if( strstr(palavra,"*/") != NULL)
                    controle = lastcontrol;

            palavra=strtok(NULL,"= ,:;()");

        }

    }

    fclose(ptIN);
    fclose(ptOUT);
    return ( RETURN_OK );
}
















