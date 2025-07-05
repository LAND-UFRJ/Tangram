#ifndef PARAM_H
#define PARAM_H

typedef struct
{
    unsigned int N, // Number of hidden states
                 M; // Number of observations
    double * pi;
    double * gamma;
    double ** A;
    double *** p; // p[i][j][k] = P[ X_t = k | X_{t-1} = j, Y_t = i ]
    double ** r; // r[i][j] = P[ X_{t,1} = j | Y_t = i ]
} Parameter;

// Somente para não dar erro de compilação ...
void get_aggregate_matrix( Parameter *param, double ** B );

/* P[ X_{t+b} | Y_t = i ] */
#define PROB_FIRST_OBS(t,i) (param->r[i][obs[t]])

/* P[ X_{t+b} | Y_t = i, X_t, ..., X_{t+b-1} ]  -  0 <= b < batch size */
#define PROB_OBS(t,b,i) (param->p[i][obs[t+b-1]][obs[t+b]])

#endif /* PARAM_H */
