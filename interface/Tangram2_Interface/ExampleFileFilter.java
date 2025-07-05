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

import java.io.File;
import java.util.Hashtable;
import java.util.Enumeration;
import javax.swing.*;
import javax.swing.filechooser.*;

public class ExampleFileFilter extends FileFilter {

    private static String TYPE_UNKNOWN = "Type Unknown";
    private static String HIDDEN_FILE = "Hidden File";

    private Hashtable filters = null;
    private String description = null;
    private String fullDescription = null;
    private boolean useExtensionsInDescription = true;

    public ExampleFileFilter() {
  this.filters = new Hashtable();
    }

    public ExampleFileFilter(String extension) {
  this(extension,null);
    }

    public ExampleFileFilter(String extension, String description) {
  this();
  if(extension!=null) addExtension(extension);
   if(description!=null) setDescription(description);
    }

    public ExampleFileFilter(String[] filters) {
  this(filters, null);
    }

    public ExampleFileFilter(String[] filters, String description) {
  this();
  for (int i = 0; i < filters.length; i++) {
      // add filters one by one
      addExtension(filters[i]);
  }
   if(description!=null) setDescription(description);
    }

    public boolean accept(File f) {
  if(f != null) {
      if(f.isDirectory()) {
    return true;
      }
      String extension = getExtension(f);
      if(extension != null && filters.get(getExtension(f)) != null) {
    return true;
      };
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

    public void addExtension(String extension) {
  if(filters == null) {
      filters = new Hashtable(5);
  }
  filters.put(extension.toLowerCase(), this);
  fullDescription = null;
    }

    public String getDescription() {
  if(fullDescription == null) {
      if(description == null || isExtensionListInDescription()) {
     fullDescription = description==null ? "(" : description + " (";
    Enumeration extensions = filters.keys();
    if(extensions != null) {
        fullDescription += "." + (String) extensions.nextElement();
        while (extensions.hasMoreElements()) {
      fullDescription += ", " + (String) extensions.nextElement();
        }
    }
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
