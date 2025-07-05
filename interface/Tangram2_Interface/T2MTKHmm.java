/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Bruno
 */

package Tangram2;

public class T2MTKHmm {

    private int hiddenStates;
    private int symbols;
    private double initStatesProbMatrix[];
    private double hiddenStatesMatrix[][];
    private double symbolsMatrix[][];
    
    
    public T2MTKHmm( int hstates, int symbols ){
        this.hiddenStates = hstates;
        this.symbols = symbols;
        this.initStatesProbMatrix = new double[hstates];
        this.hiddenStatesMatrix = new double[hstates][hstates];
        this.symbolsMatrix = new double[hstates][symbols];
        
    }

    public int getHstates(){
        return this.hiddenStates;
    }
    public int getSymbols(){
        return this.symbols;
    }
    public double getInitStatesProb(int sNumber){
        return this.initStatesProbMatrix[sNumber];
    }
    public double getSymbolGenProb(int hstate, int symbol){
        return this.symbolsMatrix[hstate][symbol];
    }
    public double getHiddenStatesTransProb(int hsource, int htarget){
        return this.hiddenStatesMatrix[hsource][htarget];
    }
    public void setInitStatesProb(int sNumber, double probValue){
        this.initStatesProbMatrix[sNumber] = probValue;
    }
    public void setSymbolGenProb(int hstate, int symbol, double probValue){
        this.symbolsMatrix[hstate][symbol] = probValue;
    }
    public void setHiddenStatesTransProb(int hsource, int htarget, double probValue){
        this.hiddenStatesMatrix[hsource][htarget] = probValue;
    }

}
