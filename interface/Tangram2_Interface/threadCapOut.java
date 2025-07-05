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

/*
   This class implements a thread that captures output (stdout or stderr) from
'in' and appends it on file 'outputFileName'.
*/
public class threadCapOut extends Thread
{
    private java.io.BufferedReader In;
    private java.io.FileWriter    Out;
    private String Prefix;

    threadCapOut( java.io.InputStream in, String outputFileName )
    {
        Prefix = "";
        try
        {
          In  = new java.io.BufferedReader( new java.io.InputStreamReader( in ) );
                    
          /*
          if ( outputFileName.indexOf("/dev/pts") != -1 || 
              outputFileName.indexOf("/dev/tty") != -1 ||
              outputFileName.indexOf("/dev/stderr") != -1)
          {
            System.err.println("setting output to standard error");
            Out = new java.io.FileWriter( java.io.FileDescriptor.err );
            System.err.println("ok");
          }
          else 
          {
            System.err.println("before FileWriter()");   */
          
            Out = new java.io.FileWriter( outputFileName, true );
            
          /*
           System.err.println("after FileWriter()");   
          } */
          
        } catch( Exception ex )
        {
            ex.printStackTrace();
        }
    }
    threadCapOut( java.io.InputStream in, String outputFileName, String prefix )
    {
        this( in, outputFileName );
        Prefix = prefix;
    }

    public void run()
    {
        String line;
        java.io.BufferedWriter out;

        out = new java.io.BufferedWriter( Out );
        try
        {
          while( (line = In.readLine()) != null )
          {
              out.write( Prefix + line );
              out.newLine();
              out.flush();
          }
          out.close();
          Out.close();
        }
        catch( Exception ex )
        {
          ex.printStackTrace();
        }
   }


/**********************************************************************************/
/**************** take output of the process Function *****************************/

  static public void takeoutput_process (Process                Procaux,
                                           String          outputFileName,
                                           String                from_msg,
                                           java.awt.Component     obj ) {
    try {
      //get output
      java.io.BufferedReader In  = new java.io.BufferedReader( new java.io.InputStreamReader( Procaux.getInputStream() ));
      //destination file
      java.io.FileWriter     Out = new java.io.FileWriter( outputFileName, true );
      java.io.BufferedWriter out = new java.io.BufferedWriter( Out );

      //get input
      java.io.BufferedWriter User_input  = new java.io.BufferedWriter ( new java.io.OutputStreamWriter( Procaux.getOutputStream() ) );


      String msgout_output = In.readLine();

      //writes the output
      while (msgout_output != null) {
        out.write(msgout_output);
        out.newLine();
        out.flush();

        if (msgout_output.indexOf("Restriction violation") != -1) {
          String message = "";
          msgout_output = In.readLine();
          out.write(msgout_output);
          out.newLine();
          out.flush();

          while (msgout_output.compareTo("Choose one option:") != 0){
            message += msgout_output + "\n";
            msgout_output = In.readLine();
            out.write(msgout_output);
            out.newLine();
            out.flush();
          }

          int selectedValue = T2inputdialog.showInputDialog( null, true, "Restriction violation", message,
                              new String[] {"[0] - Cancel",
                                            "[1] - Calculate lower bound on measure by shifting",
                                            "[2] - Calculate upper bound on measure by shifting"} );

          User_input.write(selectedValue + "\n");
          User_input.flush();
        }
        msgout_output = In.readLine();
      }
    }
    catch( Exception ex ) {
      ex.printStackTrace();
    }
  }
}
