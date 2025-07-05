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
 * This class was modified to adequate it's funcionality to the requirements
 * of the Tangram2 tool.
 * The class now, searches for a substring inside the name of the file and
 * filter according it. It is necessary because we need a filter to files
 * like this: model.IM.queue or model.TS.source ...
 * It also disallow the display of directories.  0 - files_only.
 *                                               1 - files and directories.
 *
 * By Flavio Pimentel Duarte and Kelvin Reinhardt
 * Date 10/02/2000
 */

package Tangram2;

import java.io.File;
import javax.swing.*;
import javax.swing.filechooser.*;

public class ExampleFileFilterEx extends FileFilter implements java.io.FilenameFilter {
    private static String TYPE_UNKNOWN = "Type Unknown";
    private static String HIDDEN_FILE = "Hidden File";

    private String modelName;
    private String filters[] = null;
    private String description = null;
    private String fullDescription = null;
    private String Hide = null;
    private int Dir;
    private boolean useExtensionsInDescription = true;

    public ExampleFileFilterEx() {
      //this.filters = new String;
    }

    public ExampleFileFilterEx(int dir, String modelname, String extension) {
      this(dir,modelname,extension,null);
    }

    public ExampleFileFilterEx(int dir, String modelname, String extension, String description)
    {
      this();
      Dir = dir;
      filters = new String[ 1 ];
      modelName = modelname+".";
      if(extension!=null) addExtension(extension);
      if(description!=null) setDescription(description);
    }

    public ExampleFileFilterEx(int dir, String modelname, String[] exts, String description) {
      this( dir, modelname, exts, null, description );
    }

    public ExampleFileFilterEx(int dir, String modelname, String[] exts, String hide, String description) {

      this();
      Dir  = dir;
      Hide = hide;

      if (modelname == "")
        modelName = modelname;
      else
        modelName = modelname+".";

      filters = new String[ exts.length ];
      for (int i = 0; i < exts.length; i++) {
        addExtension(i, exts[i]);
      }
      if(description!=null) setDescription(description);
    }

    public boolean accept(File f) {
      if(f != null) {
        if(f.isDirectory()) {
          if ( Dir == 0 ) return false;
          else return true;
        }
        return( accept( f, f.getName() ) );
      }
      return( false );
    }

    public boolean accept(File dir, String name) {
      for( int i = 0; i < filters.length; i++ )
      {
        if ( name.startsWith( modelName ) && name.indexOf( filters[ i ] ) != -1)
        {
          if( Hide != null )
          {
            if ( name.endsWith( Hide ) )
              return false;
          }
          return true;
        }
      }
      return false;
    }

     public String getExtension(File f) {
  if(f != null) {
      String filename = f.getName();
      int i = filename.lastIndexOf('.');
      if(i>0 && i<filename.length()-1) {
    return filename.substring(i+1).toLowerCase();
      };
  }
  return null;
    }

    private void addExtension(String extension) {
      filters[0] = extension;
      fullDescription = null;
    }

    private void addExtension(int i, String extension) {
        filters[i] = extension;
    }

    public String getDescription() {
  if(fullDescription == null) {
      if(description == null || isExtensionListInDescription()) {
     fullDescription = description==null ? "(" : description + " (";
    fullDescription += ")";
      } else {
    fullDescription = description;
      }
  }
  return fullDescription;
    }

    public void setDescription(String description) {
  this.description = description;
  fullDescription = null;
    }

    public void setExtensionListInDescription(boolean b) {
  useExtensionsInDescription = b;
  fullDescription = null;
    }

    public boolean isExtensionListInDescription() {
  return useExtensionsInDescription;
    }
}
