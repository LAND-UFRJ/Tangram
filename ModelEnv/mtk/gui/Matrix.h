#ifndef MATRIX_H
#define MATRIX_H

#include <string>

#include <qtable.h>

class Matrix: public QTable
{
    private:
        /* Used to compare two double values */
        const static double epsilon = 1e-6;

    public:
        void   setCell( int, int, double );
        void   setCell( int, int, std::string );
        double getCell( int, int );

        void   commitChanges( void );
        bool   isSthocastic( void );
        void   makeSthocastic( void );
        bool   isIndependentProb( void );
        bool   hasCellEmpty( void );
        void   setReadOnly( bool );

        Matrix( QWidget *, int, int, double ** = NULL );
        Matrix( QWidget *, int,      double  * = NULL );
};

#endif // MATRIX_H
