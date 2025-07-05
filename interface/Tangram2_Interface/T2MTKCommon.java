/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author balves
 */

package Tangram2;

import java.awt.Rectangle;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.text.DecimalFormat;
import java.util.*;
import javax.swing.JOptionPane;
import javax.swing.JTable;
import javax.swing.JViewport;

public class T2MTKCommon {


    private static String cmdsFile = "/tmp/mtkCMDs";
    private static PrintWriter cmds = null;
    private static String mtkCmd = System.getenv("TANGRAM2_HOME")+"/bin/mtk -p " + System.getenv("TANGRAM2_HOME") + "/lib/mtk_plugins/";
    public static String currdir;
    public static String currmodel;
    public static String statusBar;

    public static String statusBarUpdate(){
        File curFile = new File(currdir+currmodel+".obj");    
        statusBar = "Directory:   "+T2functions.status_directory(currdir.substring(0,currdir.length()-1))+"        Model:   "+currmodel;
        return statusBar;
        
    }

    public static void createCmds(){
        clearCmds();
        try {
            cmds = new PrintWriter(new FileWriter(new File(cmdsFile)), true);
        } catch (IOException ex) {
            errorMessage("", "Error creating MTK commands file." + ex.getMessage());
        }
    }

    public static void continueCmds(){
        try {
            cmds = new PrintWriter(new FileWriter(new File(cmdsFile)), true);
        } catch (IOException ex) {
            errorMessage("", "Error creating MTK commands file." + ex.getMessage());
        }
    }

    public static T2MTKMessages exec(){
        
        
        String error = "";
        String output = "";
        T2MTKMessages msg = new T2MTKMessages();
        try {
            Runtime run = Runtime.getRuntime();
            Process p = null;
            p = run.exec(mtkCmd+" -f "+cmdsFile);
            BufferedReader out = new BufferedReader(new InputStreamReader(p.getInputStream()));
            BufferedReader err = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            
            String buf = "";
            while ((buf = out.readLine()) != null) {
                if(!buf.equals(""))  output += buf + " \n";
                
            }
            buf = "";
            while ((buf = err.readLine()) != null) {
                if(!buf.equals(""))  error += buf + " \n";
                
            }
            if(!error.equals("")){
                error = error.replaceAll("/tmp/mtkCMDs:(\\d*): error:", "Error:");
                error = error.replaceAll("/tmp/mtkCMDs:(\\d*): warning:", "Warning:");
                msg.setError(error);
            }
            if(!output.equals("")){
                msg.setOut(output);
            }
            cmds.close();
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(null, "Mtk execution error: "+ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
        return msg;
    }
    public static void addCmd(String cmd){
        if(!((new File(cmdsFile)).exists())){
            createCmds();
        }
        cmds.println(cmd);

    }

    public static void clearCmds(){
        File file = new File(cmdsFile);
        if(file.exists()){
            file.delete();
        }
    }

    public static void errorMessage(String title, String msg){
        JOptionPane.showMessageDialog(null, msg, "Tangram-II - Error "+title, JOptionPane.ERROR_MESSAGE);
    }
    public static void infoMessage(String title, String msg){
        JOptionPane.showMessageDialog(null, msg, "Tangram-II "+title, JOptionPane.INFORMATION_MESSAGE);
    }

    public static String parseCientificNot(Double value){

        String buf = value+"";

        if(buf.contains("E")){
            String prefix = buf.substring(0, 4);
            String sufix = buf.substring(buf.indexOf("E"), buf.length());
            buf = prefix+sufix;
        }else{
            DecimalFormat df = new DecimalFormat("#.##");
            buf = df.format(value);
        }


        return buf;

    }

    public static double[] randomProbGen(int sizeOfVector){
        double[] probs = new double[sizeOfVector];

        for(int i=0;i<sizeOfVector;i++){
            probs[i] = new Random().nextDouble();
        }

        return normalizeVector(probs);
    }

    public static double[] normalizeVector(double[] vector){

        double sum = 0;
        for(int i=0;i<vector.length;i++){
            sum += vector[i];
        }
        for(int i=0;i<vector.length;i++){
            vector[i] = vector[i]/sum;
        }
        return vector;

    }

    public static double sumVector(double[] vector){
        double sum=0;
        for(int i = 0;i<vector.length;i++){
            sum += vector[i];
        }
        return sum;
    }


}
