#ifndef PARAM_H
#define PARAM_H

typedef struct
{
    unsigned int N, B;
    double * pi;
    double ** A;
    double * p;
    double * q;
    double * r;
    double * gamma;
} Parameter;

/* P[ X_{t+b} | Y_t = i ] */
#define PROB_FIRST_OBS(t,i) (obs[t] ? param->r[i] : 1-param->r[i])

/* P[ X_{t+b} | Y_t = i, X_t, ..., X_{t+b-1} ]  -  0 <= b < B */
#define PROB_OBS(t,b,i) (obs[t+b-1] ? \
                             (obs[t+b] ? 1-param->q[i] : param->q[i]) : \
                             (obs[t+b] ? param->p[i] : 1-param->p[i]))

/* P[ X_{t+b} | Y_t = i ] */
#define LOSS_RATE(i,b)     (eval_loss_rate(param,i,b))

/* \sum_{b=0}^{B-1} P[ X_{t+b} | Y_t = i ] / B */
#define BATCH_LOSS_RATE(i) (batch_loss(param,0,(i)))

double eval_loss_rate( Parameter *, int, int );
double batch_loss( Parameter *, double *, int );

void get_aggregate_matrix( Parameter *, double ** );

#endif /* PARAM_H */
