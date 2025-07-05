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

public class T2functions extends Object {

/**********************************************************************************/
/********************* Is_Initial_Probability_valid Function **********************/

// This function verifies if the Initial Probability specified (sInitial_Prob) is valid,
// and if it is it creates the file, else it shows the error message.

  static public boolean Is_Initial_Probability_valid(String sInitial_Prob, String basename, java.awt.Component obj) {

    int status;

    Entrada ent = new Entrada();

      if ((status = ent.initial_probs(sInitial_Prob, basename)) != 0){
        switch(status){
          case -1: javax.swing.JOptionPane.showMessageDialog(obj,
                   "Can't create initial probabilities file!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                   break;

          case -2: javax.swing.JOptionPane.showMessageDialog(obj,
                   "Invalid probability specification!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                   break;

          case -3: javax.swing.JOptionPane.showMessageDialog(obj,
                   "Probability value greater or equal to 1 doesn't make sense!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                   break;

          case -4: javax.swing.JOptionPane.showMessageDialog(obj,
                   "Invalid character at initial probability description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                   break;

          case -5: javax.swing.JOptionPane.showMessageDialog(obj,
                   "Sum of probability values must be 1!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                   break;

          default: javax.swing.JOptionPane.showMessageDialog(obj,
                   "Problems with initial probabilities description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
        }
        return (false);
      }
     return (true);
  }

/**********************************************************************************/
/************************** Is_Precision_valid Function ***************************/

// This function verifies if the Precision specified (sPrecision) is valid,
// else it shows the error message.

  static public boolean Is_Precision_valid(String sPrecision, java.awt.Component obj) {

    int status;

    Entrada ent = new Entrada();

    if ((status = ent.precision(sPrecision)) == -1) {
          javax.swing.JOptionPane.showMessageDialog (obj,
          "Invalid precision description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
      return (false);
    }
    return (true);
  }

/**********************************************************************************/
/************************** Is_Partition_valid Function ***************************/

// This function verifies if the Partition specified (sPartition) is valid,
// and if it is it creates the file, else it shows the error message.

  static public boolean Is_Partition_valid(String sPartition, String basename, java.awt.Component obj) {

    int status;

    Entrada ent = new Entrada();

    if ((status = ent.partition(sPartition, basename)) != 0) {
      switch(status){
        case -1: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Can't create partitions file!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        case -2: javax.swing.JOptionPane.showMessageDialog(obj,
                 "States must be in crescent order!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        case -3: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Invalid character at partition description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        default: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Problems with partitions description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
      }
      return (false);
    }
    return (true);
  }

/**********************************************************************************/
/************************ Is_Time_Intervals_valid Function ************************/

// This function verifies if the Time Intervals specified (sTime_Intervals) is valid,
// and if it is it creates the file, else it shows the error message.

  static public boolean Is_Time_Intervals_valid(String sTime_Intervals, String basename, java.awt.Component obj) {

    int status;

    Entrada ent = new Entrada();

    if ((status = ent.intervals(sTime_Intervals, basename)) != 0){
      switch(status){
        case -1: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Can't create intervals file!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        case -2: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Invalid time interval specification!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        case -3: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Time intervals must be in increasing order!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        case -4: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Invalid character at intervals description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 break;

        default: javax.swing.JOptionPane.showMessageDialog(obj,
                 "Problems with intervals description!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
      }
      return (false);
    }
    return (true);
  }

/**********************************************************************************/
/************************** fexist Function ***************************************/

// The function fexist uses the class ExampleFileFilterEx Function
// to verify if the file(s) exists.

  static public boolean fexist( String curmodel, String ext ) {
    java.io.File f = new java.io.File(".");
    String[] dirlist;

    ExampleFileFilterEx filter = new ExampleFileFilterEx(0,curmodel, ext, "found file");
    dirlist = f.list(filter);
    return (dirlist.length > 0);
  }

  static public boolean fexist( String curmodel, String[] exts ) {

    java.io.File f = new java.io.File(".");
    String[] dirlist;

    ExampleFileFilterEx filter = new ExampleFileFilterEx(0,curmodel, exts, "found files");
    dirlist = f.list(filter);
    return (dirlist.length > 0);
  }

/**********************************************************************************/
/******************* Is_some_field_missing Function *******************************/

//  Description: Searchs for an empty text field in the current panel,
//  or in one of its children, recursively.  Gives an error msg if found.
//  Input: the panel
//  Output: true, if there is some text field empty
//          false, otherwise

  static public boolean Is_some_field_missing(javax.swing.JComponent panel)
  {
    int ncomp, i=0;
    boolean empty_text_field_found = false;
    boolean label_found = false ;
    String ComponentName;

    ncomp=panel.getComponentCount();

    /* Processes all the components in the panel */

    for (i=0; i < ncomp && !empty_text_field_found; i++) {

      javax.swing.JComponent aux;
      /* Auxiliary component of index i*/
      aux = (javax.swing.JComponent)panel.getComponent(i);
      ComponentName=aux.getClass().getName();

      /* If the auxiliary component is an empty JtextField (and is Enabled) ... */
      if ( (ComponentName.indexOf("JTextField")>-1)  &&
            (((javax.swing.JTextField)aux).getText().trim().compareTo("") == 0)  &&
            ((javax.swing.JTextField)aux).isEnabled() ) {

            java.awt.Point point;
            javax.swing.JComponent label;
            double height,coordx,coordy;

            empty_text_field_found = true;

            /* Get its coordx and coordy and looks for the label */
            point  = ((javax.swing.JTextField)aux).getLocation();
            height = ((javax.swing.JTextField)aux).getHeight();
            coordy = point.getY() + (height/2);
            coordx = point.getX();
            for (; !label_found && coordx > 0; coordx--) {
               label = (javax.swing.JComponent)panel.getComponentAt((int)coordx,(int)coordy);
               if (label.getClass().getName().indexOf("Label") > -1) {
                  label_found = true;
                  int index;

                  /* Displays an error message. */
                  if ((index = ((javax.swing.JLabel)label).getText().indexOf(":")) > -1) {
                      javax.swing.JOptionPane.showMessageDialog(panel,((javax.swing.JLabel)label).getText().substring(0,index) + " required!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  }
                  else {
                      javax.swing.JOptionPane.showMessageDialog(panel,((javax.swing.JLabel)label).getText() + " required!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  }
               }
            }  /* ends the for that searchs for a label */

            if (label_found == false )  { /* Shows the default error message */
               javax.swing.JOptionPane.showMessageDialog(panel,"A required field is empty!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            }

            ((javax.swing.JTextField)aux).setRequestFocusEnabled(true);
            ((javax.swing.JTextField)aux).requestFocus();

            return true;  /* There is an empty required field */

      } /* end if */

      if (ComponentName.indexOf("JPanel") >-1) { /* If the auxiliary component is a Jpanel  */
            /* The routine is called recursively */
            empty_text_field_found = Is_some_field_missing((javax.swing.JComponent)aux);
      }

    } /* ends for that searchs for an empty text field */

    return empty_text_field_found;

  }

/**********************************************************************************/
/******************* Is_some_field_missing Function *******************************/
//  Description: Searchs for an empty text field in the current panel,
//  or in one of its childs, recursively.  Gives an error msg if found.

//  Input:  - the panel
//          - a vector pointing to optional text fields in the panel
//  Output: true, if there is some required text field empty
//          false, otherwise

  static public boolean Is_some_field_missing(javax.swing.JComponent panel, java.util.Vector NotReqObjVector) {
    int ncomp, i=0;
    boolean empty_text_field_found = false;
    boolean label_found = false ;
    String ComponentName;

    ncomp = panel.getComponentCount();

    /* Processes all the components in the panel */

    for (i = 0; i < ncomp && !empty_text_field_found; i++) {

      javax.swing.JComponent aux;  /* Auxiliary component */
      aux = (javax.swing.JComponent)panel.getComponent(i);
      ComponentName = aux.getClass().getName();

      /* If the auxiliary component is an empty JtextField ... */

      if ( (ComponentName.indexOf("JTextField") > -1)  &&
            (((javax.swing.JTextField)aux).getText().trim().compareTo("") == 0) &&
            !(NotReqObjVector.contains(aux)) && ((javax.swing.JTextField)aux).isEnabled() )  {

            java.awt.Point point;
            javax.swing.JComponent label;
            double height,coordx,coordy;

            empty_text_field_found = true;

            point = ((javax.swing.JTextField)aux).getLocation();
            height = ((javax.swing.JTextField)aux).getHeight();
            coordy = point.getY() + (height/2);

            coordx = point.getX();

            for ( ; !label_found && coordx > 0 ; coordx-- ) {
               label = (javax.swing.JComponent)panel.getComponentAt((int)coordx,(int)coordy);

               if (label.getClass().getName().indexOf("Label")>-1) {

                  label_found = true;

                  /* Displays an error message. */
                  int index;

                  if ((index = ((javax.swing.JLabel)label).getText().indexOf(":"))>-1) {
                      javax.swing.JOptionPane.showMessageDialog(panel,((javax.swing.JLabel)label).getText().substring(0,index) + " required!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  }
                  else {
                      javax.swing.JOptionPane.showMessageDialog(panel,((javax.swing.JLabel)label).getText() + " required!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  }
               }
            }  /* ends the for that searchs for a label */

            if (label_found == false )  { /* Shows the default error message */
               javax.swing.JOptionPane.showMessageDialog(panel,"A required field is empty!","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            }

            ((javax.swing.JTextField)aux).setRequestFocusEnabled(true);
            ((javax.swing.JTextField)aux).requestFocus();

            return true;  /* There is an empty required field */

      } /* end if */

      if (ComponentName.indexOf("JPanel") >- 1) {  /* If the auxiliary component is a Jpanel  */
            /* The routine is called recursively */
            empty_text_field_found = Is_some_field_missing((javax.swing.JComponent)aux,NotReqObjVector);
      }

    } /* ends for that searchs for an empty text field */

    return empty_text_field_found;

  }

/**********************************************************************************/
/********************** status_directory Function *********************************/
// This function checks if the modelenv.curdir fits in the Status Bar, if not it
// cuts some letters and send the new string that fits in the Status Bar

  static public String status_directory ( String curdir ) {

    if (curdir.length() > 25) {
      String modeldir = curdir.substring(curdir.length() - 25);
      int index = modeldir.indexOf("/"); // take the first / char in the substring
       
      if( index >= 0 ) 
          return ("..." + modeldir.substring(index));
      else    //directory string size greater than 35, it is not possible to be shown completly in status bar 
         return ("..." + modeldir);
    }
    else return (curdir);
  }

/**********************************************************************************/
/************************* Is_not_number Function *********************************/
// This function checks if the string is a number and prints the message if the string
// is not a number or it is negative
// Options: 0 means verify if it is integer
//          1 means verify if it is float
// Returns:
// 0 - if it is a number
// 1 - if it is not a number
// 2 - if it is negative

  static public int Is_not_number ( String value, int option, java.awt.Component obj, String parameter) {
    switch (option) {
      case 0: try {
                if (Integer.parseInt(value) < 0) {
                  javax.swing.JOptionPane.showMessageDialog(obj, parameter + " must be a positive integer.","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  return 2;
                }
              } catch (Exception e) {
                  javax.swing.JOptionPane.showMessageDialog(obj, "Invalid argument: " + value,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  return 1;
              }
      case 1: try {
                if (Float.parseFloat(value) < 0) {
                  javax.swing.JOptionPane.showMessageDialog(obj, parameter + " must be a positive number.","Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                  return 2;
                }
             } catch (Exception e) {
                 javax.swing.JOptionPane.showMessageDialog(obj, "Invalid argument: " + value,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
                 return 1;
             }
    }
    return 0;
  }


/**********************************************************************************/
/********************** Is_valid_parameter Function *******************************/

// This function checks if the string is a number
// Options: 0 means verify if it is integer
//          1 means verify if it is float
// Returns:
// 0 - if it is a number
// 1 - if it is not a number
// 2 - if it is negative

  static public int Is_valid_parameter ( String value, int option ) {
    switch (option) {
      case 0: try {
                if (Integer.parseInt(value) < 0) {
                  return 2;
                }
              } catch (Exception e) {
                  return 1;
              }
      case 1: try {
                if (Float.parseFloat(value) < 0) {
                  return 2;
                }
             } catch (Exception e) {
                 return 1;
             }
    }
    return 0;
  }

/**********************************************************************************/
/********************** Print_Error_Message Function ******************************/

  static public void Print_Error_Message( int return_code, java.awt.Component obj ){

    switch( return_code )
        {
          case 0:
//            javax.swing.JOptionPane.showMessageDialog(obj,"Success" ,"Ok",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;

          case 10:
            javax.swing.JOptionPane.showMessageDialog(obj,"Unknown error (10)." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 11:
//            javax.swing.JOptionPane.showMessageDialog(obj,"Solution method canceled by user." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 20:
            javax.swing.JOptionPane.showMessageDialog(obj,"Wrong number of arguments." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 21:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid arguments." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 30:
            javax.swing.JOptionPane.showMessageDialog(obj,"Error openning input file." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 31:
            javax.swing.JOptionPane.showMessageDialog(obj,"Error openning output file." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 32:
            javax.swing.JOptionPane.showMessageDialog(obj,"Error reading file." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 33:
            javax.swing.JOptionPane.showMessageDialog(obj,"Error writing file." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 34:
            javax.swing.JOptionPane.showMessageDialog(obj,"File not found." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 35:
            javax.swing.JOptionPane.showMessageDialog(obj,"End of trace file found." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 40:
            javax.swing.JOptionPane.showMessageDialog(obj,"Memory allocation failed." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 41:
            javax.swing.JOptionPane.showMessageDialog(obj,"Memory release failed." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 50:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid matrix description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 51:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid vector description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 52:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid matrix or vector description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 53:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid partitions description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 54:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid reward description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 55:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid initial probabilities description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 56:
            javax.swing.JOptionPane.showMessageDialog(obj,"Wrong vector dimensions." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 57:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid parameter." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 58:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid option." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 59:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid intervals description." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 70:
            javax.swing.JOptionPane.showMessageDialog(obj,"Embedded chain state not found." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 71:
            javax.swing.JOptionPane.showMessageDialog(obj,"Absorb embedded chain state not found." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 72:
            javax.swing.JOptionPane.showMessageDialog(obj,"Deterministic event enumeration error." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 80:
            javax.swing.JOptionPane.showMessageDialog(obj,"Matrix not initialized." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 81:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid row or column to the impulse matrix." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 82:
            javax.swing.JOptionPane.showMessageDialog(obj,"Precision too low." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 83:
            javax.swing.JOptionPane.showMessageDialog(obj,"Max number of iterations too big." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 84:
            javax.swing.JOptionPane.showMessageDialog(obj,"Impossible to calculate the max number of iterations." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 85:
            javax.swing.JOptionPane.showMessageDialog(obj,"Iterative method failed." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 86:
            javax.swing.JOptionPane.showMessageDialog(obj,"Solution method failed." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 87:
            javax.swing.JOptionPane.showMessageDialog(obj,"Descriptor calculation failed." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 88:
            javax.swing.JOptionPane.showMessageDialog(obj,"Invalid time interval." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 89:
            javax.swing.JOptionPane.showMessageDialog(obj,"Method reached the max number of iterations." ,"Warning",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 100:
            javax.swing.JOptionPane.showMessageDialog(obj,"Abnormal program execution." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 110:
            javax.swing.JOptionPane.showMessageDialog(obj,"Not implemented yet." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
            break;
          case 999:
            break;
          default:
            javax.swing.JOptionPane.showMessageDialog(obj,"Unknown error ("+ return_code +")." ,"Error",javax.swing.JOptionPane.ERROR_MESSAGE);
        }
  }

}
