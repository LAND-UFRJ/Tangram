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

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;


public class T2progressDialog extends javax.swing.JDialog {

  private static T2progressDialog dialog;
  private static int retval;
  
  // constructor with default parameters
  private T2progressDialog( )
  {
    this( null, "Process Evolution", "Processing...", null, false, false );
  }

  // original constructor
  private T2progressDialog(javax.swing.JFrame holder, String title, String msgtop, Vector commands,
                    boolean waitstart, boolean closewin) {

    super ((Frame)holder, title, true);

    Waitstart = waitstart;
    Closewin  = closewin;
    Msgtop    = msgtop;
    
    initComponents ();

    //Create a timer.
    task = new BackgroundTask( commands );

    elapsed_time = 0;
//  jProgressBar.setMaximum(commands.size());

    timer = new javax.swing.Timer( 1000, new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        elapsed_time ++;
        // jProgressBar.setValue(task.getCurrent());
        jProgressBar.setValue((jProgressBar.getValue() + 10 )% jProgressBar.getMaximum());
        jLabelMsg.setText("Elapsed time : " + elapsed_time +" seconds");
        if (task.done()) {
          timer.stop();
          if (Closewin)
             jButtonOkActionPerformed(null);

          jButtonOk.setEnabled(true);
          jButtonCancel.setEnabled(false);
          if( task.wasCanceled() )
            jLabelMsgtop.setText( "Cancelled" );
          else
          {
            jProgressBar.setValue(jProgressBar.getMaximum());
            jLabelMsgtop.setText("Finished");
          }

        }
      }
    } );

    jLabelMsgtop.setText( "Click 'Start' to run" );
    jLabelMsg.setText("");

    if( !Waitstart )
    {
      jLabelMsgtop.setText( Msgtop );
      jB_start.setVisible( false );

      task.go();
      timer.start();
    }

    pack ();
  }

  //Progress Dialog with time assign
  private T2progressDialog(javax.swing.JFrame holder, String title, String msgtop, Vector commands,
                    boolean waitstart, boolean closewin, int total_time) {

    super ((Frame)holder, title, true);

    Waitstart = waitstart;
    Closewin  = closewin;
    Msgtop    = msgtop;

    initComponents ();

    //Create a timer.
    task = new BackgroundTask( commands );

    elapsed_time = 0;
    jProgressBar.setMaximum( total_time );

    timer = new javax.swing.Timer( 1000, new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        elapsed_time ++;
        // jProgressBar.setValue(task.getCurrent());
        jProgressBar.setValue((jProgressBar.getValue() + 10 )% jProgressBar.getMaximum());
        jLabelMsg.setText("Elapsed time : " + elapsed_time +" seconds");
        if (task.done()) {
          timer.stop();
          if (Closewin)
             jButtonOkActionPerformed(null);

          jButtonOk.setEnabled(true);
          jButtonCancel.setEnabled(false);
          if( task.wasCanceled() )
            jLabelMsgtop.setText( "Cancelled" );
          else
          {
            jProgressBar.setValue(jProgressBar.getMaximum());
            jLabelMsgtop.setText("Finished");
          }

        }
      }
    } );

    jLabelMsgtop.setText( "Click 'Start' to run" );
    jLabelMsg.setText("");

    if( !Waitstart )
    {
      jLabelMsgtop.setText( Msgtop );
      jB_start.setVisible( false );

      task.go();
      timer.start();
    }

    pack ();
  }
  
  /** Initializes the Form */
  private T2progressDialog(java.awt.Frame parent, boolean modal) {
    super (parent, modal);
    initComponents ();
    pack ();
  }

  public static int showDialog(JFrame holder, String title, String msgtop, Vector commands,
                    boolean waitstart, boolean closewin)
  {

    dialog = new T2progressDialog(holder, title, msgtop, commands, waitstart, closewin);

    dialog.setValue(1);
    dialog.setLocationRelativeTo((Component)holder);
    dialog.setVisible(true);

    return retval;
  }
  
  /*
  public static int showDialog(JFrame holder, String title, String msgtop, Vector commands,
                    boolean waitstart, boolean closewin , int total_time, String killRemoteCommand)
  {

    dialog = new T2progressDialog(holder, title, msgtop, commands, waitstart, closewin, total_time);

    dialog.setValue(1);
    dialog.setLocationRelativeTo((Component)holder);
    dialog.setVisible(true);
    dialog.setKillRemoteCommand(killRemoteCommand);
    
    return retval;
  }
  */
  
  public static int showDialog(JFrame holder, String title, String msgtop, Vector commands,
                    boolean waitstart, boolean closewin , int total_time)
  {

    dialog = new T2progressDialog(holder, title, msgtop, commands, waitstart, closewin, total_time);

    dialog.setValue(1);
    dialog.setLocationRelativeTo((Component)holder);
    dialog.setVisible(true);

    return retval;

  }
  
  /*
  private void setKillRemoteCommand(String cmd)
  {
     task.setKillRemoteCommand(cmd);
  }
  */
  
  private void setValue(int v)
  {
    retval = v;
  }  

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the FormEditor.
   */
    private void initComponents() {//GEN-BEGIN:initComponents
        jProgressBar = new javax.swing.JProgressBar();
        jButtonCancel = new javax.swing.JButton();
        jButtonOk = new javax.swing.JButton();
        jLabelMsgtop = new javax.swing.JLabel();
        jLabelMsg = new javax.swing.JLabel();
        jPanel1 = new javax.swing.JPanel();
        jB_start = new javax.swing.JButton();

        getContentPane().setLayout(new org.netbeans.lib.awtextra.AbsoluteLayout());

        setBackground(new java.awt.Color(199, 199, 199));
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                closeDialog(evt);
            }
        });

        getContentPane().add(jProgressBar, new org.netbeans.lib.awtextra.AbsoluteConstraints(10, 30, 280, 16));

        jButtonCancel.setFont(new java.awt.Font("Dialog", 1, 10));
        jButtonCancel.setText("Cancel");
        jButtonCancel.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonCancelActionPerformed(evt);
            }
        });

        getContentPane().add(jButtonCancel, new org.netbeans.lib.awtextra.AbsoluteConstraints(120, 80, 80, 22));

        jButtonOk.setFont(new java.awt.Font("Dialog", 1, 10));
        jButtonOk.setText("Ok");
        jButtonOk.setEnabled(false);
        jButtonOk.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonOkActionPerformed(evt);
            }
        });

        getContentPane().add(jButtonOk, new org.netbeans.lib.awtextra.AbsoluteConstraints(210, 80, 80, 22));

        jLabelMsgtop.setText("Current Action Message");
        jLabelMsgtop.setFont(new java.awt.Font("Dialog", 1, 10));
        getContentPane().add(jLabelMsgtop, new org.netbeans.lib.awtextra.AbsoluteConstraints(10, 10, 280, 16));

        jLabelMsg.setText("Error/Status Message");
        jLabelMsg.setFont(new java.awt.Font("Dialog", 1, 10));
        getContentPane().add(jLabelMsg, new org.netbeans.lib.awtextra.AbsoluteConstraints(10, 55, 280, 16));

        getContentPane().add(jPanel1, new org.netbeans.lib.awtextra.AbsoluteConstraints(295, 107, 1, 1));

        jB_start.setFont(new java.awt.Font("Dialog", 1, 10));
        jB_start.setText("Start");
        jB_start.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jB_startActionPerformed(evt);
            }
        });

        getContentPane().add(jB_start, new org.netbeans.lib.awtextra.AbsoluteConstraints(30, 80, 80, 22));

    }//GEN-END:initComponents

  private void jB_startActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jB_startActionPerformed
    // Add your handling code here:
    jLabelMsgtop.setText( Msgtop );
    task.go();
    timer.start();
    jB_start.setEnabled(false);
  }//GEN-LAST:event_jB_startActionPerformed

  private void jButtonOkActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonOkActionPerformed
    // Add your handling code here:
    dispose();
  }//GEN-LAST:event_jButtonOkActionPerformed

  private void jButtonCancelActionPerformed (java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonCancelActionPerformed
    task.stop();

    if (Closewin)
       jButtonOkActionPerformed(null);

    jButtonOk.setEnabled(true);
    jButtonCancel.setEnabled(false);
  }//GEN-LAST:event_jButtonCancelActionPerformed

  /** Closes the dialog */
  private void closeDialog(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_closeDialog
    setVisible (false);
    dispose ();
  }//GEN-LAST:event_closeDialog


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jB_start;
    private javax.swing.JButton jButtonCancel;
    private javax.swing.JButton jButtonOk;
    private javax.swing.JLabel jLabelMsg;
    private javax.swing.JLabel jLabelMsgtop;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JProgressBar jProgressBar;
    // End of variables declaration//GEN-END:variables

  private boolean Waitstart;
  private boolean Closewin;
  private String Msgtop;
  private javax.swing.Timer timer;
  private BackgroundTask task;
  private int elapsed_time;

// End of variables declaration

  // Inner class declaration
  private class BackgroundTask {
      //private String killRemoteCommand;
      private int lengthOfTask;
      private int current = 0;
      private String statMessage;
      private boolean stopped;
      private boolean running;
      private Process task_proc;
      Vector commands;

      BackgroundTask( Vector com ) {
          commands = com;
          lengthOfTask = commands.size();
      }

      // Called to start the thread
      void go() {
          current = 0;
          stopped = false;
          running = false;
          final SwingWorker worker = new SwingWorker() {
              public Object construct() {
                  return new ActualTask();
              }
          };
          worker.start();
      }
      
      /*
      private void setKillRemoteCommand(String cmd)
      {
         killRemoteCommand = cmd;
      }
      
      private void killRemoteTask(String cmd)
      {
          try
          {
                java.lang.Process kill = Runtime.getRuntime().exec(cmd);      
          }
          catch( java.io.IOException e )
          {      
                System.err.println("Unable to finalize remote task.\n");
                e.printStackTrace();            
          }
      }
      */
      
      private boolean wasCanceled()
      {
        return stopped;
      }

      // Called to find out how much work needs to be done.
      int getLengthOfTask() {
          return lengthOfTask;
      }

      // Called to find out how much has been done.
      int getCurrent() {
          return current;
      }

      void stop() {
          stopped = true;
          if( running )
          {
            task_proc.destroy();
          }
      }
      
      // Called to find out if the task has completed.
      boolean done() {
          if (current >= lengthOfTask || stopped)
              return true;
          else
              return false;
      }

      String getMessage() {
          return statMessage;
      }

      // The actual long running task.  This runs in a SwingWorker thread.
      class ActualTask {
        ActualTask () {
          while (current < lengthOfTask && !stopped) {
            T2command cur_command = (T2command)commands.get(current);
            
            try
            {
              task_proc = Runtime.getRuntime().exec(cur_command.getArguments());
              running = true;

              try
              {
                new threadCapOut( task_proc.getInputStream(), cur_command.getOutputStream() ).start();

                new threadCapOut( task_proc.getErrorStream(), cur_command.getErrorStream() ).start();
              }
              catch (java.lang.SecurityException e)
              {
                System.out.println("security exception");
              }

//              System.out.println("ok");

              task_proc.waitFor();
              running = false;
            }
            catch(Exception e)
            {
              System.out.println("Exception running " + e.toString());
              for ( int i=0; i < (cur_command.getArguments()).length; i++ )
                 System.out.println( (cur_command.getArguments())[i] );
            }


            if( !stopped )
            {
              //System.out.println( "...Finished" );
              current++;
            }
/*
            else
            {                           
              System.out.println( "...Cancelled" );
            }
*/
          }
        }
      }
  }
}
