#ifndef RANDOM_H
#define RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

int get_random( int, double * );

#define IND(p) (drand48( ) < p)

#ifdef __cplusplus
};
#endif

#endif /* RANDOM_H */
