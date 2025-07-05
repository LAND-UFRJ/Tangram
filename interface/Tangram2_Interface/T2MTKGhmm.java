/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Tangram2;

/**
 *
 * @author balves
 */
public class T2MTKGhmm {

    private int hStates;                           // N
    private int batchSize = 0;                     // B
    private double hStatesInitialProb[];           // Pi
    private double hStatesTransitionMatrix[][];    // A
    private double symbol1InitialProb[];           // r[i]
    private double _1to0TransitionProb[];          // q
    private double _0to1TransitionProb[];          // p

    public T2MTKGhmm(int numberOfHiddenStates, int batchSize){
        hStates=numberOfHiddenStates;
        this.batchSize = batchSize;
        hStatesInitialProb = new double[hStates];
        hStatesTransitionMatrix = new double[hStates][hStates];
        symbol1InitialProb = new double[hStates];
        _0to1TransitionProb = new double[hStates];
        _1to0TransitionProb = new double[hStates];
    }

    public void set_0to1TransitionProb(int hiddenState, double _0to1TransitionProb) {
        this._0to1TransitionProb[hiddenState] = _0to1TransitionProb;
    }

    public void set_1to0TransitionProb(int hiddenState, double _1to0TransitionProb) {
        this._1to0TransitionProb[hiddenState] = _1to0TransitionProb;
    }

    public void setBatchSize(int batchSize) {
        this.batchSize = batchSize;
    }

    public void setHStates(int hStates) {
        this.hStates = hStates;
    }

    public void setHStatesInitialProb(int hiddenState, double hStatesInitialProb) {
        this.hStatesInitialProb[hiddenState] = hStatesInitialProb;
    }

    public void setHStatesTransitionMatrix(int source, int target, double hiddenStatesTransitionProb) {
        this.hStatesTransitionMatrix[source][target] = hiddenStatesTransitionProb;
    }

    public void setSymbol1InitialProb(int hiddenState, double symbol1InitialProb) {
        this.symbol1InitialProb[hiddenState] = symbol1InitialProb;
    }

    public double get_0to1TransitionProb(int hiddenState) {
        return _0to1TransitionProb[hiddenState];
    }

    public double get_1to0TransitionProb(int hiddenState) {
        return _1to0TransitionProb[hiddenState];
    }

    public int getBatchSize() {
        return batchSize;
    }

    public int getHStates() {
        return hStates;
    }

    public double getHStatesInitialProb(int hiddenState) {
        return hStatesInitialProb[hiddenState];
    }

    public double getHStatesTransitionMatrix(int source, int target) {
        return hStatesTransitionMatrix[source][target];
    }

    public double getSymbol1InitialProb(int hiddenState) {
        return symbol1InitialProb[hiddenState];
    }
    
}
