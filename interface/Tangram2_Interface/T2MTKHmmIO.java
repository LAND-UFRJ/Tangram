/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author balves
 */

package Tangram2;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;

public class T2MTKHmmIO {

    public static void hmmSave(T2MTKHmm hmm, String file){
        int i,j;
        String buf;
        try {

            PrintWriter hmmFile = new PrintWriter(new FileWriter(new File(file)), true);
            hmmFile.println(hmm.getHstates());
            hmmFile.println(hmm.getSymbols());

            //Writing Initial Probabilities of Hidden States

            buf = "";
            for(i=0;i<hmm.getHstates();i++){

                hmmFile.println(hmm.getInitStatesProb(i));

            }
            

            //Writing Hidden States Transition Probabilities

            for(i=0;i<hmm.getHstates();i++){
                buf = "";
                for(j=0;j<hmm.getHstates();j++){
                    buf += hmm.getHiddenStatesTransProb(i, j)+" ";
                }
                hmmFile.println(buf);
            }

   

            //Writing Symbol Emission Probability

            buf = "";
            for(i=0;i<hmm.getHstates();i++){
                buf = "";
                for(j=0;j<hmm.getSymbols();j++){
                    buf += hmm.getSymbolGenProb(i, j)+" ";
                }
                hmmFile.println(buf);
            }
            hmmFile.close();

        } catch (IOException ex) {
            T2MTKCommon.errorMessage("Error", "Error Writing Hmm File: "+ex.getMessage());
        }
    }

    public static T2MTKHmm hmmRead(String file){

        int i,j;
        T2MTKHmm hmm = null;

        try {

            BufferedReader in = new BufferedReader(new FileReader(file));
            StringTokenizer st = null;
            int hStates = Integer.parseInt(in.readLine());
            int symbols = Integer.parseInt(in.readLine());
            hmm = new T2MTKHmm(hStates, symbols);

            for(i=0;i<hStates;i++){

                hmm.setInitStatesProb(i, Double.parseDouble(in.readLine()));

            }

            for(i=0;i<hmm.getHstates();i++){

                st = new StringTokenizer(in.readLine());
                for(j=0;j<hmm.getHstates();j++){

                    hmm.setHiddenStatesTransProb(i, j, Double.parseDouble(st.nextToken()));
                }

            }

            for(i=0;i<hmm.getHstates();i++){
                st = new StringTokenizer(in.readLine());
                for(j=0;j<hmm.getSymbols();j++){
                    hmm.setSymbolGenProb(i, j, Double.parseDouble(st.nextToken()));
                }
                
            }

        } catch (FileNotFoundException ex) {
            T2MTKCommon.errorMessage("", "Cannot read Hmm file: "+ex.getMessage());
        } catch (IOException e){
            T2MTKCommon.errorMessage("", "Cannot read Hmm file: "+e.getMessage());
        }

        return hmm;

    }

}
