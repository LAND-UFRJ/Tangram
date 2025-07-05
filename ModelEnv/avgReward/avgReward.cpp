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


/*
	Receives as arguments the files:
        argv[1]     ==> -r | -i
        argv[2]     ==> SSFile
	    argv[3]     ==> generator_mtx file name
        argv[4]     ==> output index for result file name    
        argv[4]     ==> reward files        impulse files

*/

#include <stdio.h>

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
extern "C" { 
#include "sparseMatrix.h" 
}

int g_return = 0;

class Vector
{
public:
    // is limited by [0,max_num-1]
    double *array; 
    int max_num;
    
    
    // You should call this constructor for the state probabilities and for the .TS. or .SS.
    Vector( char *filename , char kind_of_vector = 's' ); 		
    // and this for reward probabilities
    Vector( char *filename , int max_size );
    void Print_Vector( void );

private:
    FILE *fp;
};

//element of the ended queue of Impulse
class Impulse
{
public:
    int current;
    int target;
    double probability;
    double reward;
    Impulse *next;
    Impulse( int _current , int _target , double _reward , double _probability  );
};

class Impulse_List
{
public:
    //public constructor
    Impulse_List( char * filename );
    double Calculate_Impulse_Measure( Vector *ts, Matrix *Probability_Matrix, double lambda );
    void Print_FIFO( void );
    
private:
    
    Impulse* first;
    Impulse* last;
    int max_num;
    FILE *fp;
    
    void Insert( int current_state , int next_state , double reward , double probability );
};

double Internal_Product( Vector& array1 , Vector &array2 );
void CutModelName( char* mname , const char* fname );
void CutImpulseName( char* iname , const char* fname );
void CutRewardName( char* rname , const char* fname );
/*
    Receives as argument the probabilities file
     and contructs:

    if ( kind_of_vector == 'p' ) the vector of probabilities
    if ( kind_of_vector == 's' ) the vector of .TS. or .SS.

*/

Vector::Vector( char *filename , char kind_of_vector )
{
    char string[200];
    int index = 0, current_size = 0;
    double probability = 0.0;

    array = new double;
    fp=fopen( filename,"r" );
    if ( fp == NULL )
    {
        cout<<"The file "<< filename <<" could not be opened"<<endl;
        g_return = 30;
        return;
    }

    if ( kind_of_vector == 'p' )
    {

        while ( !feof ( fp ) )
        {
            if ( fgets( string , 200 * sizeof ( char ) , fp ) == NULL) break;

            //if it's a pi line reallocates the vector and put its value on its place
            if ( string[0] == 'p' )
            {
                sscanf ( string ,"pi[%d]= %lf",&index,&probability );

                if( current_size + 1 != index )
	            {
                    g_return = 51;
                    return;
                }
	    
                current_size++;
                array = ( double* ) realloc( array , current_size * sizeof( double ) );
                array[index-1] = probability;	        
            }
            else
            {
                continue;
            }
        }
    
    }else
    {
        while( !feof( fp ) )
        {
            if ( fgets( string , 200 * sizeof ( char ) , fp ) == NULL ) break;

            //if it's a pi line reallocates the vector and put its value on its place
            if ( isdigit( string[0] ) )
            {
                sscanf ( string ,"%d %lf",&index,&probability );

                if( current_size + 1 != index )
                {
                    // My vector of probabilities isn't complete or isn't ordered
                    g_return = 51;
                    return;
                }    
	    
                current_size++;
                array = ( double* ) realloc ( array , current_size * sizeof ( double ) );
                array[index-1] = probability;	        
            }
            else
            {
                continue;
            }
        }
    }
    max_num = current_size;
    fclose( fp );
    
}

void Vector::Print_Vector(void)
{
    int i;
//    printf( "Vector size is: %d", max_num );
    for ( i = 0 ; i < max_num ; i++ )
    {
        printf( "Vector[%d]: %e \n" , i +1 , array[i] );
    }
   
}


/*
    Receives as argument the reward filename and it's maximum size
    and contructs the reward vector
*/


Vector::Vector( char *filename, const int max_size )
{
    char string[80];
    int index = 0, i = 0;
    double probability = 0.0;

    fp=fopen( filename, "r" );
    if ( fp == NULL )
    {
        cout<<"The file "<< filename <<" could not be opened"<<endl;
        g_return = 30;
        return;
    }

    array = new double[max_size];

    // all the values will have its value turned to 0. 

    for ( i = 0 ; i < max_size ; i++ )
    {
        array[i] = 0;
    }

    while ( !feof ( fp ) )
    {
        if ( fgets( string , 80 * sizeof( char ) , fp ) == NULL ) break;
        if ( isalpha ( string[0] ) || string[0] == ' ' || string[0] == '#' )
        {
            continue;
        }
        else if ( isdigit ( string[0] ) )
        {
            sscanf ( string ,"%d %lf", &index, &probability );
//            printf ( "Peguei %d %e \n", index , probability );
            array[index - 1] = probability;
            if ( max_size == index )
            {
                max_num = max_size;
                return;
            }

        }else
        {
            g_return =  51;
            cout << "The state after" << index << " of the file " << filename << " is not being recognized"<<endl;
            return;
        }
    }
    max_num = index;
    fclose( fp );
    
}

/*
    Calculates the internal product between two vectors of the same size
*/

Impulse::Impulse( int _current , int _target , double _reward , double _probability  )
{
    current = _current;
    target = _target;
    probability = _probability;
    reward = _reward;
    next = NULL;
    
}

Impulse_List::Impulse_List( char *filename )
{
    char string[200];
    int current_size = 0 , current_state = 0 , next_state = 0;
    double probability = 0.0 , reward = 0.0;
    
    first = NULL;
    last = NULL;

    fp=fopen( filename ,"r" );
    if ( fp == NULL )
    {
        g_return = 30;
        cout<<"File "<<filename<<" not found"<<endl;
        return;
    }
    while( !feof(fp) )
    {
        if ( fgets( string , 180 * sizeof( char ) , fp ) == NULL ) break;
        if( isdigit( string[0] ) )
        {
            sscanf( string , "%d %d %lf %lf" , &current_state , &next_state , &probability, &reward );
            Insert( current_state , next_state , reward , probability );	
            current_size++;
        }
    }
    max_num = current_size;
    fclose( fp );
}
/************************************************************************/
/*                                                                      */
/*      Impulse_List::Calculate_Impulse_Measure( Vector * ts )			*/
/*		Average Impulse Reward Calculation                              */
/*									                                    */
/* Modified in Novebember, 2006 by Guilherme Dutra G. Jaime in order to	*/
/* solve an issue in which the average impulse reward was  being        */
/* calculated in a wrong way.    								        */
/*									                                    */ 
/*                                  									*/
/************************************************************************/
double Impulse_List::Calculate_Impulse_Measure( Vector * ts, Matrix * Probability_Matrix, double lambda )
{
    double accumulate = 0.0;
    double increase = 0.0;
    double P_ij    = 0.0; /*  */
    Impulse *aux = first;

    while( aux != NULL )
    { 
     /*    aux->reward      :Impulse Reward Value taken from the 3rd column of the <MODEL_BASE_NAME>.impulse_reward.<METRIC> file  */
     /*    aux->probability :Impulse Reward Fraction taken from the  4th column of the <MODEL_BASE_NAME>.impulse_reward.<METRIC> file  */
     /*                       This fraction represents the fraction of aux->reward gained when the desired event triggers       */
     /*                      and this fraction is related to the "action" probability in the source code of the model              */
     /*ts->array[(aux->current)-1]: PI_i  (the i-1 index is related to diferences shown in the <MODEL_BASE_NAME.states file */
     /*   P_ij              :Probability of transitioning from state i to state j */
     /*   lambda            : Uniformization rate returned from the uniformize_matrix() function (diagonal of the Q Matrix + 0.001) */

        P_ij = get_matrix_position( aux->current, aux->target, Probability_Matrix );
        
        increase = ( aux->reward ) * P_ij * ( aux->probability ) * ( ts->array[ ( aux->current ) -1 ] );
        accumulate += increase;
        aux = aux->next;
    }

    return accumulate*lambda;

}

void Impulse_List::Insert( int current_state , int next_state , double reward , double probability )
{
    if ( first == NULL )
    {
        first = new Impulse( current_state , next_state , reward , probability );
        last = first; 
    }
    else
    {
        last->next = new Impulse( current_state , next_state , reward , probability );
        last=last->next;
    }
    
}

void Impulse_List::Print_FIFO( void )
{
    Impulse *aux = first;
    
    cout<<"FIFO"<<endl;
    while ( aux != NULL )
    {
        cout << aux->current << " " << aux->target << " " << aux->probability << " " << aux->reward << endl;
        aux = aux -> next;
    }
}


double Internal_Product( Vector *array1 , Vector *array2 )
{
    double sum = 0;
    int i = 0;

    if ( array1->max_num < array2->max_num )
    {
        cout <<"Error: The two files don't have the same vector size"<<endl;
        g_return = 56;
    }
    else
    {
        for ( i = 0 ; i < array1->max_num ; i++ )
        {
           sum = sum + array1->array[i] * array2->array[i];
//           printf( "Passo %d %e\n", i , sum );
        }
    }
    return sum ;
}

void CutModelName( char* mname , const char* fname )
{
    int i = 0;
    
    while( fname[i] != '.' )
    {
        mname[i]=fname[i];
        i++;
    }
    
    mname[i] = '\0';

}

/*
    Receives a reward filename 
*/
void CutRewardName( char* rname , const char* fname )
{
    //position in the reward filename and in the reward name 
    int fi = 0, ri = 0;
    
    
    //"model_name."
    while ( fname[fi] != '.' ) fi++;
    fi++; // for the punctuation
    
    //"rate_reward."
    while ( fname[fi] != '.' ) fi++;
    fi++; // for the punctuation
    
    //"reward_measure_name"
    while ( fname[fi] != '\0' )
    {
        rname[ri] = fname[fi];
        fi++;
        ri++;
    }
    
    rname[ri] = '\0';
}

void CutImpulseName( char* iname , const char* fname)
{
    int fi = 0 , ii = 0;

    //"model_name."
    while ( fname[fi] != '.' ) fi++;
    fi++; // for the punctuation
    
    //"impulse_reward."
    while ( fname[fi] != '.' ) fi++;
    fi++; // for the punctuation
    
    //"reward_measure_name"
    while ( fname[fi] != '\0' )
    {
        iname[ii] = fname[fi];
        fi++;
        ii++;
    }
    
    iname[ii] = '\0';

}


void Save_Measure( char *filename , double return_value )
{
    FILE * fp;

    if ( ( fp = fopen ( filename ,"w" ) ) == NULL)
    {
        cout<<"Error opening file:"<< filename<<endl;
        g_return = 31;
        return;
    }

    fprintf( fp , "# Reward value:\n" );
    fprintf( fp , "# %.10E" , return_value );
    fclose( fp );

}


int main( int argc, char *argv[] )
{
    Vector* vector1;
    Vector* vector2;
    Impulse_List * imp1;
    char mat_type;         /* matrix type: 'Q' - Rate Matrix and 'P' - Probability matrix  */
    char model_name[100];
    char ir_name[100];
    char output[200];
    FILE *fp_rate_matrix; /* Pointer to the rate matrix file */
    Matrix *Rate_Matrix, *Prob_Matrix;  /* Pointer to the rate matrix and probability data structure, respectively */
    double lambda;               /* uniformization rate                    */
    
    double return_value = 0;
    int i;

    if ( argc < 6 )
    {
        cerr<<endl<<"Invalid number of parameters"<<endl<<"For reward measures you should type:"<<endl;
        cerr<<endl<<"avgReward -r [SS]Filename  [generator_mtx]Filename  OutputExtension RewardFilename1  RewardFilename2  RewardFilename3 ..."<<endl;
        cerr<<endl<<"or"<<endl;
        cerr<<endl<<"avgReward -i [SS]Filename  [generator_mtx]Filename  OutputExtension ImpulseFilename1 ImpulseFilename2 ImpulseFilename3 ..."<<endl<<endl;
        return 20;
    }

    CutModelName( model_name , argv[2] );

    // Open the rate matrix file
    if (! (fp_rate_matrix = fopen(argv[3], "r")) ) 
    {
        fprintf(stderr,"Average Reward: Error while opening file %s\n",argv[3]);
        return(1);
    }
    // Build the Rate_Matrix using values from fp_rate_matrix file 
    Rate_Matrix = get_trans_matrix(fp_rate_matrix);
    mat_type = 'Q';
    generate_diagonal(mat_type, Rate_Matrix);
    /* Uniformizing matrix */
    Prob_Matrix = uniformize_matrix(Rate_Matrix, &lambda); /* Lambda is automatically chosen from the Rate_Matrix diagonal */

    //for average reward 
    if ( strcmp( argv[1] , "-r" ) == 0 )
    {
        //for use in the output name
        vector1 = new Vector( argv[2] );
//        vector1->Print_Vector();

        for( i = 5 ; i < argc ; i++ )
        {
            CutRewardName( ir_name , argv[i] );

            cout<< " processing " << argv[i] << endl;
            vector2 = new Vector( argv[i] , vector1->max_num );
//            vector2->Print_Vector();

            return_value = Internal_Product ( vector1 , vector2 );

            strcpy( output , model_name );
            strcat( output , "." );
            strcat( output , ir_name );
            strcat( output , ".avgReward.IM." );
            strcat( output , argv[3] );
            Save_Measure( output , return_value );
            delete vector2;
        }
        delete vector1;
    }
    //for impulse rewards
    else if ( strcmp( argv[1], "-i" ) == 0 )
    {
        vector1 = new Vector( argv[2] );
        for( i = 5 ; i < argc ; i++ )
        {
            CutImpulseName( ir_name , argv[i] );
            
            cout<< " processing " << argv[i] << endl;
            imp1 = new Impulse_List( argv[i] );
            return_value = imp1 -> Calculate_Impulse_Measure( vector1, Prob_Matrix, lambda );

            strcpy( output , model_name );
            strcat( output , "." );
            strcat( output , ir_name );
            strcat( output , ".IM." );
            strcat( output , argv[4] );
            Save_Measure( output , return_value );
            delete imp1;
        }
        
        delete vector1;

    }else
    {
        cout<<"Error: The first argument has to be the kind of measure to be calculated";
        cout<<"-r for reward average and -i impulse measure"<<endl;
        return 21;
    }
    return g_return;
}

