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

import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;
import java.util.Vector;
import java.io.*;


public class TangramFilter extends FileFilter {
    Vector allow;
    Vector nAllow;
    String description;

    public TangramFilter( ) {
        allow = new Vector();
        nAllow = new Vector();
    }

    public void addExtension( String ext ) {
        allow.add( new String( ext.toUpperCase() ) );
    }

    public void addBarredExtensionPrefix( String ext ) {
        nAllow.add( new String( ext.toUpperCase() ) );
    }

    public boolean accept( File file ) {
        String filename = file.getName();

        if( file.isDirectory() == true )
            return true;


        try {
            String ext = filename.substring( filename.indexOf('.') ).toUpperCase();
            for( int i=0 ; i < allow.size() ; i++ ) {

                if( ext.indexOf( "." + (String) allow.get( i ) ) != -1 ) {
                    for( int j=0 ; j < nAllow.size() ; j++ ) {
                        if( ext.indexOf( "." + ((String) nAllow.get( j )) + "." ) != -1 ) {
                            return false;
                        }
                    }

                    return true;
                }
            }
        } catch (Exception e) { }

        return false;
    }

    public void setDescription(String desc) {
        description = new String( desc );
    }

    public String getDescription() {
        return this.description;
    }
}
