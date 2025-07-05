#include "floatvalue.h"

#include <stdio.h>
#include <limits.h>

#include <fstream>

#include "object_io.h"
#include "watchdog.h"
#include "acf.h"

const double FloatValue::DEFAULT_BOUND = 1e6;

void initFloatValue( void ) __attribute__ ((constructor));

Object::Info object_info =
{
    "floatvalue",
    "Float Value Samples",
    FLOATVALUE_HELP_MESSAGE,
    1, 0,
    ObjectBase<FloatValue>::help,
    ObjectBase<FloatValue>::create
};

FloatValue::FloatValue( const Name & name, const FloatValue & floatvalue )
          : ObjectBase<FloatValue>(name,this)
{
    int i;

    sample.size = floatvalue.sample.size;
    sample.data = 0;

    if( sample.size > 0 )
    {
        sample.data = new double[sample.size];
        for( i = 0; i < sample.size; i++ )
            sample.data[i] = floatvalue.sample.data[i];
    }

    lower = floatvalue.lower;
    upper = floatvalue.upper;
}

FloatValue::FloatValue( const Name & name )
          : ObjectBase<FloatValue>(name,this)
{
    sample.size = 0;
    sample.data = 0;

    lower = -DEFAULT_BOUND;
    upper =  DEFAULT_BOUND;
}

FloatValue::~FloatValue( void )
{
    DEBUG(LEVEL_INFO,"Destructing FloatValue sample (%s)\n",
          object_name.c_str());

    if( sample.data )
        delete sample.data;
}

bool FloatValue::truncate_data( double l, double u )
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

bool FloatValue::set_lower( const std::string & value,
                            const Index & idx )
{
    double new_lower;

    new_lower = atof( value.c_str( ) );

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

bool FloatValue::get_lower( std::string & value,
                            const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%lf", lower );
    value = buffer;

    return true;
}

bool FloatValue::set_upper( const std::string & value, const Index & idx )
{
    double new_upper;

    new_upper = atof( value.c_str( ) );

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

bool FloatValue::get_upper( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%lf", upper );
    value = buffer;

    return true;
}

bool FloatValue::get_max_bound( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%lf", DEFAULT_BOUND );
    value = buffer;

    return true;
}

bool FloatValue::set_size( const std::string & value, const Index & idx )
{
    int new_size;
    register int i;
    double *new_data;

    new_size = atol( value.c_str( ) );

    if( new_size > 0 )
    {
        new_data = new double[new_size];
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

bool FloatValue::get_size( std::string & value, const Index & idx )
{
    char buffer[32];

    sprintf( buffer, "%d", sample.size );
    value = buffer;

    return true;
}

bool FloatValue::set_data( const std::string & value, const Index & idx )
{
    int    i;
    double new_data_value;

    i = idx.front( );

    if( i < sample.size )
    {
        new_data_value = strtod( value.c_str( ), 0 );
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

bool FloatValue::get_data( std::string & value, const Index & idx )
{
    char buffer[32];
    int i;

    i = idx.front( );

    if( i < sample.size )
    {
        sprintf( buffer, "%lf", sample.data[i] );
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

bool FloatValue::display_sample( Terminal & term )
{
    int i;

    if( sample.size > 0 )
    {
        term.print( "Sample size = %d\n\n", sample.size );

        term.print( "Sample data =\n" );
        for( i = 0; i < sample.size; i++ )
            term.print( "\t[ %8lf ]\n", sample.data[i] );
    }
    else
        term.print( "Sample is empty\n\n" );

    return true;
}

bool FloatValue::display_bounds( Terminal & term )
{
    term.print( "Sample values are within [%lf,%lf]\n", lower, upper );

    return true;
}

bool FloatValue::display_stats( Terminal & term )
{
    register int i;
    double       mean, var, oldmean;
    double       min, max;

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

    term.print( "Minimum:  %lf\n", min  );
    term.print( "Maximum:  %lf\n", max  );
    term.print( "Mean:     %lf\n", mean );
    term.print( "Variance: %lf\n", var  );
    
    return true;
}

bool FloatValue::load( Arguments & args )
{
    FloatSample new_sample;
    register int i;
    WatchDog<double> sample_watcher;
    std::ifstream infile( args[0].text.c_str( ) );

    if( infile )
    {
        READ(infile,new_sample.size);
        LOWER_LIMIT(new_sample.size,1);

        new_sample.data = (double *)malloc( new_sample.size * sizeof(double) );
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

        Terminal::displayMessage( "Floatvalue was successfully loaded." );

        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       args[0].text.c_str( ) );
        return false;
    }
}

bool FloatValue::load_offset( Arguments & args )
{
    FloatSample new_sample;
    register int i;
    int offset, size;
    WatchDog<double> sample_watcher;
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
            size = new_sample.size - offset;

        new_sample.size = size;

        while( offset > 0 )
        {
            infile >> i;
            offset--;
        }

        new_sample.data = (double *)malloc( new_sample.size * sizeof(double) );
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
        return true;
    }
    else
    {
        Terminal::displayErrorMessage( "could not open file '%s' for reading",
                                       args[0].text.c_str( ) );
        return false;
    }
}

bool FloatValue::save( Arguments & args )
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

            Terminal::displayMessage( "Floatvalue was successfully saved in file %s.", args[0].text.c_str( ) );

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

bool FloatValue::save_offset( Arguments & args )
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

            Terminal::displayMessage( "Floatvalue was successfully saved in file %s.", args[0].text.c_str( ) );

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

bool FloatValue::truncate( Arguments & args )
{
    Arguments::const_iterator it;
    double l, u;

    l = strtod(args[0].text.c_str(), NULL );
    u = strtod(args[1].text.c_str(), NULL );

    return truncate_data( l, u );
}

bool FloatValue::autocorrelation( Arguments & args )
{
    int lag, start;
    Terminal *terminal;

    lag = atol( args[0].text.c_str( ) );

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
        acf( lag, start, terminal->getFile( ), &sample );
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

void * FloatValue::get_sample( void * args )
{
    return (&sample);
}

void * FloatValue::set_sample( void * args )
{
    FloatSample * s;
    int i;

    if( !args )
        return 0;

    s = (FloatSample *)args;

    if( sample.data )
        delete sample.data;

    sample.size = s->size;
    if( sample.size > 0 )
    {
        sample.data = new double[sample.size];
        for( i = 0; i < sample.size; i++ )
            sample.data[i] = s->data[i];
    }
    else
        sample.data = 0;

    return (&sample);
}

Object * FloatValue::createEmpty( const Name & name, Arguments & args )
{
    FloatValue * floatvalue;

    floatvalue = new FloatValue( name );

    return floatvalue;
}

Object * FloatValue::createLoad( const Name & name, Arguments & args )
{
    FloatValue * floatvalue;

    floatvalue = new FloatValue( name );
    if( !floatvalue->load( args ) )
    {
        delete floatvalue;
        floatvalue = 0;
    }

    return floatvalue;
}

void initFloatValue( void )
{
    DEBUG(LEVEL_INFO,"Initializing Float Valued Sample Library\n");

    FloatValue::addConstructor( &FloatValue::createEmpty,
                                 Prototype( ) );
    FloatValue::addConstructor( &FloatValue::createLoad,
                                 Prototype( 1, 1, TK_STRING ) );

    FloatValue::addOption( "lower", "bound for the smallest element",
                            0, &FloatValue::set_lower, &FloatValue::get_lower );
    FloatValue::addOption( "upper", "bound for the largest element",
                            0, &FloatValue::set_upper, &FloatValue::get_upper );
    FloatValue::addOption( "max_bound", "maximum absolut value that the elements can assume",
                            0, 0, &FloatValue::get_max_bound );
                            
    FloatValue::addOption( "size", "number of elements in the sample",
                            0, &FloatValue::set_size, &FloatValue::get_size );
    FloatValue::addOption( "data", "i-th element in the sample",
                            1, &FloatValue::set_data, &FloatValue::get_data );

    FloatValue::addDisplay( "sample", "contents of the sample",
                             &FloatValue::display_sample );
    FloatValue::addDisplay( "bounds", "lower and upper bounds",
                             &FloatValue::display_bounds );
    FloatValue::addDisplay( "stats", "sample statistics",
                             &FloatValue::display_stats );

    FloatValue::addCommand( "load", &FloatValue::load,
                             Prototype( 1, 1, TK_STRING ),
                             LOAD_HELP_MESSAGE );
    FloatValue::addCommand( "load", &FloatValue::load_offset,
                             Prototype( 2, 3, TK_STRING, TK_INTEGER, TK_INTEGER ),
                             LOAD_HELP_MESSAGE );

    FloatValue::addCommand( "save", &FloatValue::save,
                             Prototype( 1, 1, TK_STRING ),
                             SAVE_HELP_MESSAGE );
    FloatValue::addCommand( "save", &FloatValue::save_offset,
                             Prototype( 2, 3, TK_STRING, TK_INTEGER, TK_INTEGER ),
                             SAVE_HELP_MESSAGE );

    FloatValue::addCommand( "truncate", &FloatValue::truncate,
                             Prototype( 2, 2, TK_REAL, TK_REAL ),
                             TRUNCATE_HELP_MESSAGE );

    FloatValue::addCommand( "autocorrelation", &FloatValue::autocorrelation,
                             Prototype( 1, 2, TK_INTEGER, TK_INTEGER ),
                             AUTOCORRELATION_HELP_MESSAGE );

    FloatValue::addFunction( "get_sample", &FloatValue::get_sample );
    FloatValue::addFunction( "set_sample", &FloatValue::set_sample );
}
