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

package Tangram2;


/**

   This class implements a thread that wait for a process and do something
   after it finished.
   One of the parameters are the label to indicate what to do.

   a) label tgif, used in modelenv, waits for tgif and update figs according
   the existence of the files.

   b) label make, used in intsimul, waits for gcc and call tgif and mark_find
   to process interactive simulation.

*/

import javax.swing.*;

public class threadWaitFor extends Thread
{
  private Process proc;
  private String which;
  private T2modelenv modelenv;
  private T2intsimulation intsimul;

  threadWaitFor( T2modelenv Modelenv, Process Proc, String Which )
  {
    proc     = Proc;
    modelenv = Modelenv;
    which    = Which;
  }

  threadWaitFor( T2intsimulation IntSimul, Process Proc, String Which )
  {
    proc     = Proc;
    intsimul = IntSimul;
    which    = Which;
  }

  
  public void run()
  {
    String line;
    
    //---------------------------------------------------------------------------------------
    if (which=="tgif") {

      //which == tgif means that this thread will wait for tgif!
        
      try {
      proc.waitFor();
      }
      catch(Exception exception) {
        javax.swing.JOptionPane.showMessageDialog(modelenv,"TGIF not responding!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
      }
      
            
      //update the figs and buttons!
      modelenv.update_obj();
      modelenv.tgif = null;
    }
    //---------------------------------------------------------------------------------------
    if (which=="make") {
      try {
        proc.waitFor();
      }
      catch(Exception exception) {
        javax.swing.JOptionPane.showMessageDialog(modelenv,"TGIF not responding!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
      }
      
      if (intsimul.make_error == 0) {
        try {
         
          // call tgif and get it's input to stablish the comunication
         
          //System.out.println("Initializing Interactive Simulation ...");
           
          intsimul.tgif    = Runtime.getRuntime().exec( new String[]{ "bash","-c", "tgif " + T2main_app.tgif_flags + "-exec - "+ intsimul.simulation.modelenv.curmodel  } );
          
          intsimul.intgif  = new java.io.BufferedReader( new java.io.InputStreamReader( intsimul.tgif.getInputStream() ) );
          
          intsimul.outtgif = new java.io.PrintWriter( intsimul.tgif.getOutputStream() );
        }          
        catch( Exception ex )
        {
            System.out.println("Error launching tgif.");
            System.out.flush();
            ex.printStackTrace();
        }

        try {
          
          
          
          String parameter = new String( intsimul.simulation.get_param_interactive() );
                    
          T2intsimulation.send_command_to_tgif("call_simple_shortcut(ToggleShowMenubar)",
            intsimul.intgif,
            intsimul.outtgif);
          
          
          T2intsimulation.send_command_to_tgif("call_simple_shortcut(ToggleShowChoicebar)",
            intsimul.intgif,
            intsimul.outtgif);
          
          
          T2intsimulation.send_command_to_tgif("call_one_arg_shortcut(Tangram2StartSimulator,\""+ parameter+"\")",
            intsimul.intgif,
            intsimul.outtgif);

          /* waiting tgif reach the function that create a socket */
          
          
          while( (line = intsimul.intgif.readLine()) != null )
          {
            if ( line.indexOf("TGIF: ready") != -1 )
            {
              break;
            } 
          }
          
          //System.out.println("Interactive Simulation in Progress ...");

          // call mark_find to simulate
          
          // Bruno / Felipe -> the -o option should be configured via the interface
          
          intsimul.mark_find = Runtime.getRuntime().exec( new String[] { "./mark_find","-S","-I","-f" + intsimul.simulation.modelenv.curmodel, "-oout","-d " + intsimul.simulation.modelenv.debuglevel } );
          
          
                    
          java.io.BufferedReader inmark_find;
          inmark_find  = new java.io.BufferedReader( new java.io.InputStreamReader( intsimul.mark_find.getErrorStream() ) );
          
          if (intsimul.simulation.modelenv.debuglevel == 0)
            intsimul.mark_find.waitFor();
          
          
          /*  The following lines were commented because now, with fprintf() 
           *
           *  enabled in Tangram-II,
           *
           *  we can't be sure that something shown at stderr is in fact
           *
           *  an error.
           */
          
           /*
            
          while( (line = inmark_find.readLine()) != null )
          {
            System.err.println(line); System.err.flush();
            
            if (intsimul.simulation.modelenv.debuglevel == 0)
            {
                javax.swing.JOptionPane.showMessageDialog(modelenv,(java.lang.Object)line,"Interactive Simulation",javax.swing.JOptionPane.ERROR_MESSAGE);
                break;
            }
          } 
            
            */
          
          if (intsimul.simulation.modelenv.debuglevel != 0)
            intsimul.mark_find.waitFor();
          
          //System.out.println("Interactive Simulation Finished!!!");
          
          
          intsimul.simulation_finished();
        }  
        catch( Exception ex )
        {
            System.out.println("Error running interactive simulation.");            
            System.out.flush();
            ex.printStackTrace();
        }
        
      }
      else
      {/*
        javax.swing.JOptionPane.showMessageDialog(this, "Please, look for error messages in the text terminal.","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
     */ }
    }
    
    //---------------------------------------------------------------------------------------
  }
}

/**********************************************************************************/
