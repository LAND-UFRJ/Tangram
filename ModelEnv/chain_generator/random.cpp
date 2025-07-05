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
*  @file random.cpp
*  @brief <b>Random object for pseudo-random number generation</b>.
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
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>

#ifdef __APPLE__
  #include <limits.h>
  #include <float.h>
  #ifndef MAXDOUBLE
    #define MAXDOUBLE DBL_MAX
  #endif
#else
// The values.h is deprecated.
#include <values.h>
#endif

#include "prototypes.h"
#include "general.h"
#include "tangram_types.h"
#include "random.h"
#include "seeds_table.h"

//------------------------------------------------------------------------------
//     C O N S T R U C T O R    << . >>    D E S T R U C T O R
//------------------------------------------------------------------------------
//  Constructor initializes seed parameter for random number generation
//------------------------------------------------------------------------------
Random_obj::Random_obj()
{
    struct timeval tv;
    int index, time_high, time_med, time_low;

    gettimeofday(&tv, NULL);
    time_high = (tv.tv_usec & 0xFFF00000) >> 16;
    time_med  = (tv.tv_usec & 0x000FFF00) >> 8;
    time_low  = tv.tv_usec & 0x000000FF;

    do
    {
        index = rand() % SEEDS_TABLE_SIZE;
    } while( used_seeds[index] );

    used_seeds[index] = 1;
    memcpy( (char *)seed, (char *)random_seeds[index], sizeof(seed) );

    /* sum wallclock to seed[] - don't want to use the same 512 seeds always */
    seed[0] += time_high;
    seed[1] += time_med;
    seed[2] += time_low;

    memcpy( (char *)lastx, (char *)seed, sizeof(seed) );

    debug(4,"Random::Random: creating object");
}
//------------------------------------------------------------------------------
//  Constructor initializes seed parameter with user defined value
//------------------------------------------------------------------------------
Random_obj::Random_obj( unsigned short seed_value[3] )
{
    memcpy( (char *)seed, (char *)seed_value, sizeof(seed) );
    memcpy( (char *)lastx, (char *)seed, sizeof(seed) );

    debug(4,"Random::Random: creating object");
}
//------------------------------------------------------------------------------
// Destructor for Random
//------------------------------------------------------------------------------
Random_obj::~Random_obj()
{
  /* do nothing */
  debug(4,"Random::~Random(): destroying object");
}

//------------------------------------------------------------------------------
//     M E T H O D S
//------------------------------------------------------------------------------
void Random_obj::get_seed( unsigned short seed_value[3] )
{
    memcpy( (char *)seed_value, (char *)seed, sizeof(seed) );
/*    return seed;*/
}
//------------------------------------------------------------------------------
/*      Generates U[0,1]     */
TGFLOAT Random_obj::next_randval()
{
    TGFLOAT value;
    value = nrand48(lastx);
    return (value / NUMMAX);
    debug(4,"Random::next_randval() Initializing Random Variable generation seed");
}
//------------------------------------------------------------------------------
