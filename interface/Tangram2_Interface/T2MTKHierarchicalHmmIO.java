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
public class T2MTKHierarchicalHmmIO {

    public static T2MTKHierarchicalHmm read(String file){
        try {
            BufferedReader in = new BufferedReader(new FileReader(new File(file)));
            int hStates = Integer.parseInt(in.readLine());
            int symbols = Integer.parseInt(in.readLine());
            T2MTKHierarchicalHmm hhmm = new T2MTKHierarchicalHmm(hStates, symbols);
            int batchSize = 0;
            if(!T2MTKMain.modelType.equals("hmm_batch_variable")){
                batchSize = Integer.parseInt(in.readLine());
                hhmm.setBatchSize(batchSize);
            }
            for(int i=0;i<hhmm.getHStates();i++){
                hhmm.setHiddenStInitProb(i, Double.parseDouble(in.readLine()));
            }
            for(int i=0;i<hhmm.getHStates();i++){
                String line = in.readLine();
                StringTokenizer st = new StringTokenizer(line);
                for(int j=0;j<hhmm.getHStates();j++){
                    hhmm.setHiddenStTransProb(i, j, Double.parseDouble(st.nextToken()));
                }
            }
            for(int i=0;i<hhmm.getHStates();i++){
                String line = in.readLine();
                StringTokenizer st = new StringTokenizer(line);
                for(int j=0;j<hhmm.getSymbols();j++){
                  hhmm.setSymbolsInitProb(i, j, Double.parseDouble(st.nextToken()));
                }
                for(int j=0;j<hhmm.getSymbols();j++){
                    for(int k=0;k<hhmm.getSymbols();k++){
                        hhmm.setSymbolsTransProb(i, j, k, Double.parseDouble(st.nextToken()));
                    }
                }
            }
            return hhmm;

        } catch (FileNotFoundException ex) {
            T2MTKCommon.errorMessage("", "Cannot read file: "+file+". "+ex.getMessage());
        } catch (IOException ex) {
            T2MTKCommon.errorMessage("", "Cannot read file: "+file+". "+ex.getMessage());
        }

        return null;
    }

}
