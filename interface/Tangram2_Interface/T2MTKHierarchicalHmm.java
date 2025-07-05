/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Tangram2;

/**
 *
 * @author balves
 */
public class T2MTKHierarchicalHmm {

    private int hStates;  // N
    private int symbols;  // M
    private int batchSize;  // B for fixed batch case
    private double hiddenStInitProb[]; // Pi
    private double hiddenStTransMatrix[][]; // A
    private double symbolsInitProb[][];  // r[i][j]
    private double symbolsTransMatrix[][][];  // p[i][j][k]

    public T2MTKHierarchicalHmm(int hStates, int symbols){
        this.hStates = hStates;
        this.symbols = symbols;
        hiddenStInitProb = new double[hStates];
        hiddenStTransMatrix = new double[hStates][hStates];
        symbolsInitProb = new double[hStates][symbols];
        symbolsTransMatrix = new double[hStates][symbols][symbols];
    }

    public int getBatchSize() {
        return batchSize;
    }

    public int getHStates() {
        return hStates;
    }

    public double getHiddenStInitProb(int hState) {
        return hiddenStInitProb[hState];
    }

    public double getHiddenStTransProb(int source, int target) {
        return hiddenStTransMatrix[source][target];
    }

    public int getSymbols() {
        return symbols;
    }

    public double getSymbolsInitProb(int hState, int symbol) {
        return symbolsInitProb[hState][symbol];
    }

    public double getSymbolsTransProb(int hState, int sourceSymbol, int targetSymbol) {
        return symbolsTransMatrix[hState][sourceSymbol][targetSymbol];
    }

    public void setBatchSize(int batchSize) {
        this.batchSize = batchSize;
    }

    public void setHStates(int hStates) {
        this.hStates = hStates;
    }

    public void setHiddenStInitProb(int hState, double hiddenStInitProb) {
        this.hiddenStInitProb[hState] = hiddenStInitProb;
    }

    public void setHiddenStTransProb(int source, int target, double hiddenStTransProb) {
        this.hiddenStTransMatrix[source][target] = hiddenStTransProb;
    }

    public void setSymbols(int symbols) {
        this.symbols = symbols;
    }

    public void setSymbolsInitProb(int hState, int symbol, double symbolInitProb) {
        this.symbolsInitProb[hState][symbol] = symbolInitProb;
    }

    public void setSymbolsTransProb(int hState, int sourceSymbol, int targetSymbol, double symbolTransProb) {
        this.symbolsTransMatrix[hState][sourceSymbol][targetSymbol] = symbolTransProb;
    }



}
