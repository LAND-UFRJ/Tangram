#include "intvalue.h"

#include <stdio.h>
#include <limits.h>

#include <fstream>

#include "object_io.h"
#include "watchdog.h"
#include "acf.h"

void initIntValue( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "intvalue",
    "Integer Value Samples",
    INTVALUE_HELP_MESSAGE,
    1, 0,
    ObjectBase<IntValue>::help,
    ObjectBase<IntValue>::create
};

IntValue::IntValue( const Name & name, const IntValue & intvalue )
         : ObjectBase<IntValue>(name,this)
{
    int i;

    sample.size = intvalue.sample.size;
    sample.data = 0;

    if( sample.size > 0 )
    {
        sample.data = new int[sample.size];
        for( i = 0; i < sample.size; i++ )
            sample.data[i] = intvalue.sample.data[i];
    }

    lower = intvalue.lower;
    upper = intvalue.upper;
}

/* Constructor used when no arguments are passed */
IntValue::IntValue( const Name & name )
         : ObjectBase<IntValue>(name,this)
{
    sample.size = 0;
    sample.data = 0;

    lower = INT_MIN;
    upper = INT_MAX;
}

IntValue::~IntValue( void )
{
    DEBUG(LEVEL_INFO,"Destructing IntValue sample (%s)\n",
          object_name.c_str());

    if( sample.data )
        delete sample.data;
}

bool IntValue::truncate_data( int l, int u )
{
    int i;

    if( u < l )
    {
        Terminal::displayErrorMessage(
            "lower bound must be smaller than or equal to upper bound\n" );
        return false;
    }

    for( i = 0; i < sample.size; i++ )
    {
        if( sample.data[i] > u )
            sample.data[i] = u;
        else if( sample.data[i] < l )
            sample.data[i] = l;
    }

    return false;
}

bool IntValue::set_lower( const std::string & value,
                          const Index & idx )
{
    int new_lower;

    new_lower = atol( value.c_str( ) );

    if( new_lower <= upper )
    {
        lower = new_lower;
        truncate_data( lower, upper );
        return true;
    }
    else
        Terminal::displayErrorMessage(
          "lower bound must be smaller than or equal to the upper bound %d",
          upper );

    return false;
}

bool IntValue::get_lower( std::string & value,
                          const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", lower );
    value = buffer;

    return true;
}

bool IntValue::set_upper( const std::string & value,
                          const Index & idx )
{
    int new_upper;

    new_upper = atol( value.c_str( ) );

    if( new_upper >= lower )
    {
        upper = new_upper;
        truncate_data( lower, upper );
        return true;
    }
    else
        Terminal::displayErrorMessage(
          "upper bound must be greater than or equal to the lower bound %d",
          lower );

    return false;
}

bool IntValue::get_upper( std::string & value,
                          const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", upper );
    value = buffer;

    return true;
}

bool IntValue::set_size( const std::string & value, const Index & idx )
{
    int new_size, * new_data, i;

    new_size = atol( value.c_str( ) );

    if( new_size > 0 )
    {
        new_data = new int[new_size];
        for( i = 0; i < sample.size && i < new_size; i++ )
            new_data[i] = sample.data[i];
        for( ; i < new_size; i++ )
            new_data[i] = lower;

        if( sample.data )
            delete sample.data;

        sample.size = new_size;
        sample.data = new_data;
        return true;
    }
    else if( new_size == 0 )
    {
        if( sample.data )
            delete sample.data;

        sample.size = 0;
        sample.data = 0;
        return true;
    }
    else
        Terminal::displayErrorMessage( "sample size must be non negative" );

    return false;
}

bool IntValue::get_size( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", sample.size );
    value = buffer;

    return true;
}

bool IntValue::set_data( const std::string & value, const Index & idx )
{
    int i, new_data_value;

    i = idx.front( );

    if( i < sample.size )
    {
        new_data_value = strtol( value.c_str( ), 0, 0 );
        sample.data[i] = new_data_value;
        if( new_data_value >= lower && new_data_value <= upper )
        {
            sample.data[i] = new_data_value;
            return true;
        }
        else
        {
            Terminal::displayErrorMessage(
                                 "data value must be within bounds [%d,%d]",
                                 lower, upper );
            return false;
        }
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than sample size" );
        return false;
    }
}

bool IntValue::get_data( std::string & value, const Index & idx )
{
    char buffer[32];
    int i;

    i = idx.front( );

    if( i < sample.size )
    {
        sprintf( buffer, "%d", sample.data[i] );
        value = buffer;
        return true;
    }
    else
    {
        Terminal::displayErrorMessage(
                        "index must be smaller than sample size" );
        return false;
    }
}

bool IntValue::display_sample( Terminal & term )
{
    int i;

    if( sample.size > 0 )
    {
        term.print( "Sample size = %d\n\n", sample.size );

        term.print( "Sample data =\n" );
        for( i = 0; i < sample.size; i++ )
            term.print( "\t[ %8d ]\n", sample.data[i] );
    }
    else
        term.print( "Sample is empty\n\n" );

    return true;
}

bool IntValue::display_bounds( Terminal & term )
{
    term.print( "Sample values are within [%d,%d]\n", lower, upper );
    return true;
}

bool IntValue::display_stats( Terminal & term )
{
    int i;
    double mean, var, oldmean;
    int min, max;

    if( sample.size < 1 )
    {
        Terminal::displayErrorMessage(
            "cannot calculate statistics on an empty sample\n" );
        return false;
    }

    mean = min = max = sample.data[0];
    var = 0.0;

    for( i = 1; i < sample.size; i++ )
    {
        oldmean = mean;
        mean = ((i*mean)+sample.data[i])/(i+1);
        var  = ((1.0 - (1.0/i))*var) + (i+1)*((mean-oldmean)*(mean-oldmean));
        if( sample.data[i] < min )
            min = sample.data[i];
        if( sample.data[i] > max )
            max = sample.data[i];
    }

    term.print( "Minimum:  %d\n", min );
    term.print( "Maximum:  %d\n", max );
    term.print( "Mean:     %g\n", mean );
    term.print( "Variance: %g\n", var );
    
    return true;
}

bool IntValue::load( Arguments & args )
{
    Sample new_sample;
    int i;
    WatchDog<int> sample_watcher;
    std::ifstream infile( args[0].text.c_str( ) );

    if( infile )
    {
        READ(infile,new_sample.size);
        LOWER_LIMIT(new_sample.size,1);

        new_sample.data = (int *)malloc( new_sample.size * sizeof(int) );
        sample_watcher.addWatch( new_sample.data );

        for( i = 0; i < new_sample.size; i++ )
        {
            READ(infile,new_sample.data[i]);
            LOWER_LIMIT(new_sample.data[i],lower);
            UPPER_LIMIT(new_sample.data[i],upper);
        }

        if( sample.data )
            delete sample.data;

        sample = new_sample;
        sample_watcher.removeAllWatches( );

        Terminal::displayMessage( "Intvalue was successfully loaded." );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       args[0].text.c_str( ) );

        return false;
    }
}

bool IntValue::load_offset( Arguments & args )
{
    Sample new_sample;
    int i;
    int offset, size;
    WatchDog<int> sample_watcher;
    std::ifstream infile( args[0].text.c_str( ) );

    if( infile )
    {
        READ(infile,new_sample.size);
        LOWER_LIMIT(new_sample.size,1);

        offset = atol( args[1].text.c_str( ) );
        LOWER_LIMIT(offset,0);
        UPPER_LIMIT(offset,new_sample.size-1);

        if( args.size( ) == 3 )
        {
            size = atol( args[2].text.c_str( ) );
            LOWER_LIMIT(size,1);
            UPPER_LIMIT(offset+size,new_sample.size);
        }
        else
            size = new_sample.size-offset;

        new_sample.size = size;

        while( offset > 0 )
        {
            infile >> i;
            offset--;
        }

        new_sample.data = (int *)malloc( new_sample.size * sizeof(int) );
        sample_watcher.addWatch( new_sample.data );

        for( i = 0; i < new_sample.size; i++ )
        {
            READ(infile,new_sample.data[i]);
            LOWER_LIMIT(new_sample.data[i],lower);
            UPPER_LIMIT(new_sample.data[i],upper);
        }

        if( sample.data )
            delete sample.data;

        sample = new_sample;
        sample_watcher.removeAllWatches( );

        Terminal::displayMessage( "Intvalue was successfully loaded." );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       args[0].text.c_str( ) );
        return false;
    }
}

bool IntValue::save( Arguments & args )
{
    int i;
    std::ofstream outfile( args[0].text.c_str( ) );

    if( outfile )
    {
        if( sample.data )
        {
            WRITE(outfile,sample.size);
            outfile << std::endl;

            for( i = 0; i < sample.size; i++ )
            {
                WRITE(outfile,sample.data[i]);
                outfile << std::endl;
            }

            Terminal::displayMessage( "Intvalue was successfully saved in file %s.", args[0].text.c_str( ) );

            return true;
        }
        else
        {
            Terminal::displayErrorMessage( "no data to write to file" );
            return false;
        }
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       args[0].text.c_str( ) );
        return false;
    }
}

bool IntValue::save_offset( Arguments & args )
{
    int i, offset, size;
    std::ofstream outfile( args[0].text.c_str( ) );

    offset = atol( args[1].text.c_str( ) );
    LOWER_LIMIT(offset,0);
    UPPER_LIMIT(offset,sample.size-1);

    if( args.size( ) == 3 )
    {
        size = atol( args[2].text.c_str( ) );
        LOWER_LIMIT(size,1);
        UPPER_LIMIT(offset+size,sample.size);
    }
    else
        size = sample.size-offset;

    if( outfile )
    {
        if( sample.data )
        {
            WRITE(outfile,size);
            outfile << std::endl;

            for( i = 0; i < size; i++ )
            {
                WRITE(outfile,sample.data[offset+i]);
                outfile << std::endl;
            }

            Terminal::displayMessage( "Intvalue was successfully saved in file %s.", args[0].text.c_str( ) );

            return true;
        }
        else
        {
            Terminal::displayErrorMessage( "no data to write to file" );
            return false;
        }
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       args[0].text.c_str( ) );
        return false;
    }
}

bool IntValue::truncate( Arguments & args )
{
    Arguments::const_iterator it;
    int l, u;

    l = strtol(args[0].text.c_str(), NULL, 0);
    u = strtol(args[1].text.c_str(), NULL, 0);

    return truncate_data( l, u );
}

bool IntValue::autocorrelation( Arguments & args )
{
    int lag, start;
    Terminal * terminal;

    lag = atoi( args[0].text.c_str( ) );

    if( args.size( ) > 1 )
    {
        start = atoi( args[1].text.c_str( ) );
        if( start < 0 )
        {
            Terminal::displayErrorMessage( "start must be positive" );
            return false;
        }
    }
    else
        start = 0;

    terminal = Terminal::getTerminal( );
        
    if( terminal )
    {
        terminal->open( );
        int_acf( lag, start, terminal->getFile( ), &sample );
        terminal->close( );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not get a valid terminal" );
        return false;
    }

    return true;        
}

void * IntValue::get_sample( void * args )
{
    return (&sample);
}

void * IntValue::set_sample( void * args )
{
    Sample * s;
    int i;

    if( !args )
        return 0;

    s = (Sample *)args;

    if( sample.data )
        delete sample.data;

    sample.size = s->size;
    if( sample.size > 0 )
    {
        sample.data = new int[sample.size];
        for( i = 0; i < sample.size; i++ )
            sample.data[i] = s->data[i];
    }
    else
        sample.data = 0;

    return (&sample);
}

/* Creating Intvalue with no arguments */
Object * IntValue::createEmpty( const Name & name, Arguments & args )
{
    IntValue * intvalue;

    intvalue = new IntValue( name );

    return intvalue;
}

/* Creating Intvalue with data file passed as argument */
Object * IntValue::createLoad( const Name & name, Arguments & args )
{
    IntValue * intvalue;

    intvalue = new IntValue( name );

    if( !intvalue->load( args ) )
    {
        delete intvalue;
        intvalue = 0;
    }

    return intvalue;
}

void initIntValue( void )
{
    DEBUG(LEVEL_INFO,"Initializing Integer Valued Sample Library\n");

    IntValue::addConstructor( &IntValue::createEmpty,
                              Prototype( ) );
    IntValue::addConstructor( &IntValue::createLoad,
                              Prototype( 1, 1, TK_STRING ) );

    IntValue::addOption( "lower", "bound for the smallest element",
                         0, &IntValue::set_lower, &IntValue::get_lower );
    IntValue::addOption( "upper", "bound for the largest element",
                         0, &IntValue::set_upper, &IntValue::get_upper );

    IntValue::addOption( "size", "number of elements in the sample",
                         0, &IntValue::set_size, &IntValue::get_size );
    IntValue::addOption( "data", "i-th element in the sample",
                         1, &IntValue::set_data, &IntValue::get_data );

    IntValue::addDisplay( "sample", "contents of the sample",
                          &IntValue::display_sample );
    IntValue::addDisplay( "bounds", "lower and upper bounds",
                          &IntValue::display_bounds );
    IntValue::addDisplay( "stats", "sample statistics",
                          &IntValue::display_stats );

    IntValue::addCommand( "load", &IntValue::load,
                          Prototype( 1, 1, TK_STRING ),
                          LOAD_HELP_MESSAGE );
    IntValue::addCommand( "load", &IntValue::load_offset,
                          Prototype( 2, 3, TK_STRING, TK_INTEGER, TK_INTEGER ),
                          LOAD_HELP_MESSAGE );

    IntValue::addCommand( "save", &IntValue::save,
                          Prototype( 1, 1, TK_STRING ),
                          SAVE_HELP_MESSAGE );
    IntValue::addCommand( "save", &IntValue::save_offset,
                          Prototype( 2, 3, TK_STRING, TK_INTEGER, TK_INTEGER ),
                          SAVE_HELP_MESSAGE );

    IntValue::addCommand( "truncate", &IntValue::truncate,
                          Prototype( 2, 2, TK_INTEGER, TK_INTEGER ),
                          TRUNCATE_HELP_MESSAGE );

    IntValue::addCommand( "autocorrelation", &IntValue::autocorrelation,
                          Prototype( 1, 2, TK_INTEGER, TK_INTEGER ),
                          AUTOCORRELATION_HELP_MESSAGE );

    IntValue::addFunction( "get_sample", &IntValue::get_sample );
    IntValue::addFunction( "set_sample", &IntValue::set_sample );
}
