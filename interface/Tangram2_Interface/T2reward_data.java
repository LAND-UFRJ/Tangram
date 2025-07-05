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
 * T2reward_data.java
 *
 * Created on January 21, 2003, 6:36 PM
 */

package Tangram2;

/**
 *
 * @author  develop
 */
public class T2reward_data
{
    public boolean generate_trace      = false;
    public String  reward_name[]       = null;
    public boolean reward_plot_set[]   = null;
    public String  maximum_y_range     = "1.0";
    public String  minimum_y_range     = "0.0";
    public String  x_range             = "1000.0";
    public String  grid_spacing        = "100.0";
    public String  samples_buffer_size = "1000";
    public String  polling_interval    = "50000";

    public T2reward_data( String names[] )
    {
        reward_name = names;
        reward_plot_set = new boolean[names.length];
        for( int i = 0; i < names.length; i++ )
            reward_plot_set[i] = false;
    }

    public String get_animplot_command( String curmodel )
    {
        boolean run_animplot = false;
        String file_names = "", command_line;

        for( int i = 0 ; i < reward_name.length; i++ )
            if( reward_plot_set[i] )
            {
                file_names += " -f " + curmodel + "." +
                                       reward_name[i] + ".CR.trace";
                run_animplot = true;
            }

        if( !run_animplot )
            return null;

        command_line = "( sleep  1 ; animplot " + file_names + " -x" +
                       " -m " + maximum_y_range +
                       " -n " + minimum_y_range +
                       " -r " + x_range +
                       " -g " + grid_spacing +
                       " -b " + samples_buffer_size +
                       " -i " + polling_interval +
                       " -e )";

        return command_line;
    }
}
