/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Tangram2;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author balves
 */
public class T2MTKGhmmIO {

    public static T2MTKGhmm read(String file){
        try {
            BufferedReader in = new BufferedReader(new FileReader(new File(file)));
            int hStates = Integer.parseInt(in.readLine());
            int batchSize = Integer.parseInt(in.readLine());
            T2MTKGhmm ghmm = new T2MTKGhmm(hStates, batchSize);
            for(int i=0;i<hStates;i++){
                 ghmm.setHStatesInitialProb(i, Double.parseDouble(in.readLine()));
            }
            for(int i=0;i<hStates;i++){
                String line = in.readLine();
                StringTokenizer st = new StringTokenizer(line);
                for(int j=0;j<hStates;j++){
                    double AtransProb = Double.parseDouble(st.nextToken());
                    ghmm.setHStatesTransitionMatrix(i, j, AtransProb);
                }
            }
            for(int i=0;i<hStates;i++){
                String line = in.readLine();
                StringTokenizer st = new StringTokenizer(line);
                ghmm.setSymbol1InitialProb(i, Double.parseDouble(st.nextToken()));
                ghmm.set_0to1TransitionProb(i, Double.parseDouble(st.nextToken()));
                ghmm.set_1to0TransitionProb(i, Double.parseDouble(st.nextToken()));
            }
            return ghmm;

        } catch (FileNotFoundException ex) {
            T2MTKCommon.errorMessage("", "Cannot read parameters from file: "+file+". "+ex.getMessage());
        } catch (IOException ex) {
            T2MTKCommon.errorMessage("", "Cannot read parameters from file: "+file+". "+ex.getMessage());
        }
        return null;
    }

}
