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

import javax.swing.*;
import javax.swing.plaf.basic.*;

/**
 *
 * @author  develop
 */

public class myCombo extends javax.swing.JComboBox {
    
    public myCombo(){
        super();
        setUI(new myComboUI());
    }//end of default constructor
                           
    public class myComboUI extends javax.swing.plaf.metal.MetalComboBoxUI{
        protected ComboPopup createPopup(){
            BasicComboPopup popup = new BasicComboPopup(comboBox){
                protected JScrollPane createScroller() {
                    return new JScrollPane( list, ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED,
                    ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED );
                }//end of method createScroller
            };
            return popup;
        }//end of method createPopup
    }//end of inner class myComboUI
    
}
