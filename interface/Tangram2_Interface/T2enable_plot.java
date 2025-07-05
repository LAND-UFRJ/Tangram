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

public class T2enable_plot extends Thread {
  T2enable_plot(Process proc, String name, String[] extensions, javax.swing.JButton jPlot, String dir) {
    Proc = proc;
    Name = name;
    Exts = extensions;
    Plot = jPlot;
    Directory = dir;
  }

  public void run() {
    try {
    /*System.out.println("criei a thread e estou dormindo...");*/
      Proc.waitFor();
    /*System.out.println("voltei e vou checar os arquivos....");*/

      java.io.File f = new java.io.File(Directory);
      ExampleFileFilterEx objFilter_aux = new ExampleFileFilterEx(0,Name, Exts, "Plot Files");
      String[] dirlist = f.list(objFilter_aux);
      if ((dirlist != null) && (dirlist.length > 0))
        Plot.setEnabled(true);
      else
        Plot.setEnabled(false);
    }catch (Exception e) {
       System.out.println("Error " + e.toString());
    }
  }

  private Process Proc;
  private String Name, Directory;
  private String Exts[];
  private javax.swing.JButton Plot;
}
