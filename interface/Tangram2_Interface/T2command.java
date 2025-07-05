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

public class T2command extends Object {
  // default constructor for arrays of arguments
  public T2command( String [] argv, String output, String error )
  {
    arguments_vector = argv;
    outputStream = output;
    errorStream = error;
  }

  // optional constructor for a single command line string
  public T2command( String arguments, String output, String error )
  {
    int i = 0;
    java.util.StringTokenizer strtok = new java.util.StringTokenizer( arguments, " " );

    arguments_vector = new String[strtok.countTokens()];

    while( strtok.hasMoreTokens() )
      arguments_vector[i++] = strtok.nextToken();

    outputStream = output;
    errorStream = error;
  }

  public String [] getArguments()
  {
    return arguments_vector;
  }
  public String getOutputStream()
  {
    return outputStream;
  }
  public String getErrorStream()
  {
    return errorStream;
  }

  private String [] arguments_vector;
  private String outputStream;
  private String errorStream;
}
