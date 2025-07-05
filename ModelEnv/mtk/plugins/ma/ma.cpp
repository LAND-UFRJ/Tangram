#include "ma.h"

#include <stdio.h>
#include <stdlib.h>

#include "object_io.h"
#include "watchdog.h"
#include "white_noise.h"

void initMA( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "ma",
    "Moving Average order 1 model plugin",
    MA_HELP_MESSAGE,
    1, 0,
    ObjectBase<MA>::help,
    ObjectBase<MA>::create
};

////////////////////////////////////////
// Plugin programmer code starts here //
////////////////////////////////////////

/**
 * Plugin attribute get/set
 */ 
bool MA::set_q( const std::string &value, const Index &idx )
{
    q = strtol( value.c_str( ), 0, 0 );
    
    build_theta();

    return true;
}

bool MA::get_q( std::string &value, const Index &idx )
{
    char buffer[32];
    
    sprintf( buffer, "%d", q );
    value = buffer;

    return true;
}

bool MA::set_variance( const std::string &value, const Index &idx )
{
    variance = strtod( value.c_str( ), 0 );

    return true;
}

bool MA::get_variance( std::string &value, const Index &idx )
{
    char buffer[32];
    
    sprintf( buffer, "%lf", variance );
    value = buffer;

    return true;
}

bool MA::set_theta( const std::string &value, const Index &idx )
{
    unsigned int i;
    bool         status;

    i = idx.front( );

    if( i < q )
    {
        theta[i] = strtod( value.c_str( ), 0 );
        status = true;
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than MA order q" );
        status = false;
    }

    return status;
}

bool MA::get_theta( std::string &value, const Index &idx )
{
    unsigned int i;
    char         buffer[32];
    bool         status;

    i = idx.front( );

    if( i < q )
    {
        sprintf( buffer, "%lf", theta[i] );
        value = buffer;
        status = true;
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than MA order q" );
        status = false;
    }

    return status;
}

/**
 * Plugin displays
 */ 
bool MA::display_q( Terminal &term )
{
    term.print( "q = %d\n", q );

    return true;
}

bool MA::display_variance( Terminal &term )
{
    term.print( "variance = %lf\n", variance );

    return true;
}

bool MA::display_theta( Terminal &term )
{
    register unsigned int i;

    term.print( "Theta weigths vector:\n" );

    for( i = 0; i < q; i++ )
        term.print( "[ % lf ]\n", theta[i] );

    return true;
}

/**
 * Plugin commands
 */ 
bool MA::simulate( Arguments &args )
{
    register unsigned int i, j;
    unsigned int          steps, np; // np == noise pointer
    double               *z, sample;    
    
    if( args.size() == 1 )
        steps = strtol( args[0].text.c_str( ), 0, 0 );
    else
    {
        Terminal::displayErrorMessage( "Invalid number of parameters" );
        return false;
    }

    // Do simulation
    z  = (double *)calloc( q + 1, sizeof( double ) );
    np = 0;
    
    printf( "t\tX(t)\n" );
    
    for( i = 1; i <= steps; i++ )
    {
        z[np]  = white_noise( 0, variance );
        
        sample = z[np];
        
        for( j = 1; ( j <= q ) && ( i > j ); j++ )
            sample += theta[ j - 1 ] * z[ ( np - j ) % ( q + 1 ) ];

        printf( "%d\t% lf\n", i, sample );        
    
        np = ( np + 1 ) % ( q + 1 );
    }

    if( z )
        free( z );

    return true;
}

/**
 * Plugin onstructors
 */ 
MA::MA( const Name & name, const MA & ma )
        : ObjectBase<MA>( name, this )
{
    variance = ma.variance;
    theta    = ma.theta;
}

Object *MA::createEmpty( const Name &name, Arguments &args )
{
    MA *ma;

    ma = new MA( name );

    return ma;
}

Object *MA::createWithOperators( const Name &name, Arguments &args )
{
    MA    *ma;
    Index  idx;

    ma = new MA( name );
    
    ma->set_q(        args[0].text, idx );
    ma->set_variance( args[1].text, idx );
    ma->build_theta();

    return ma;
}

/**
 * libmtk communication methods
 */ 
MA::MA( const Name & name )
   :ObjectBase<MA>( name, this )
{
    q        = 1;
    variance = 0.0;
    theta    = NULL;
}

MA::~MA( void )
{
    DEBUG( LEVEL_INFO, "Destructing MA object (%s)\n",
           object_name.c_str());
}

/**
 * Allocates memory and resamples theta vector
 */
void MA::build_theta( void )
{
    register unsigned int i;

    theta = (double *)realloc( theta, q * sizeof( double ) );
    
    for( i = 0; i < q; i++ )
        theta[i] = drand48();
}

void initMA( void )
{
    DEBUG( LEVEL_INFO, "Initializing MA plugin\n");

    MA::addConstructor( &MA::createEmpty,
                         Prototype( ) );
    MA::addConstructor( &MA::createWithOperators,
                         Prototype( 2, 2, TK_INTEGER, TK_REAL ) );

    MA::addOption( "q", "Moving average order",
                    0, &MA::set_q, &MA::get_q );
    MA::addOption( "variance", "White noise variance",
                    0, &MA::set_variance, &MA::get_variance );
    MA::addOption( "theta", "Theta weigths",
                    1, &MA::set_theta, &MA::get_theta );

    MA::addDisplay( "q", "Moving average order value",
                     &MA::display_q );
    MA::addDisplay( "variance", "White noise variance value",
                     &MA::display_variance );
    MA::addDisplay( "theta", "Theta weigths values",
                     &MA::display_theta );

    MA::addCommand( "simulate", &MA::simulate,
                     Prototype( 1, 1, TK_INTEGER ),
                     SIMULATE_HELP_MESSAGE );
}

