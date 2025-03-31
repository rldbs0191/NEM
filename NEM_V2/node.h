#ifndef _NODE_H_
#define _NODE_H_

#include "define.h"

class Node {
private:
    int REGION, DIM;
    double** WIDTH;
    double* FLUX;
    Node*** NEIGHBOR;
    BOUNDARY_TYPE** BOUNDARY;
    double** BETA;
    double*** Q;
    double*** C;
    double*** OUT_CURRENT;
    double*** INCOM_CURRENT;
    double*** DL;

public:
    Node();
    Node( int region, int dim);
    ~Node() {}

    int getREGION() const { return REGION; }
    int getDIM() const { return DIM; }

    const double getWIDTH(int dir, int side) { return WIDTH[dir][side]; }
    const double* getFLUX() { return FLUX; }
    Node* getNEIGHBOR(int dir, int side ) const { return NEIGHBOR[dir][side]; }
    Node* accessNEIGHBOR( int dir, int side ) { return NEIGHBOR[dir][side]; }
    void setNEIGHBOR(int dir, int side, Node* node) { NEIGHBOR[dir][side] = node; }
	const BOUNDARY_TYPE getBOUNDARY(int dir, int side) { return BOUNDARY[dir][side]; }
    double*** getQ() { return Q; }
    double*** getC() { return C; }
    double*** getOUT_CURRENT() { return OUT_CURRENT; }

    void updateTransverseLeakage(int dim, int group );
    void update1Dflux();
    void updateAverageFlux();
    void updateOutgoingCurrent();
};

#endif