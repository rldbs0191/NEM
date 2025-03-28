#ifndef _NODE_H_
#define _NODE_H_

#include "define.h"

typedef Matrix<double, Dynamic, Dynamic> GroupMatrix;
typedef Matrix<double, Dynamic, Dynamic> CXTable;
typedef Matrix<double, Dynamic, 1> MatrixG1;
typedef vector<vector<MatrixG1>> MatrixG1_2D;
typedef vector<vector<GroupMatrix>> GroupMatrix_2D;


class Node {
private:
    int REGION, DIM;
    Array<double, 1, Dynamic> WIDTH;
    Array<double, 1, Dynamic> FLUX;
    Array<int, Dynamic, 2> NEIGHBOR;
    Array<BOUNDARY_TYPE, Dynamic, 2> BOUNDARY;
    CXTable CX;
    GroupMatrix A;
    GroupMatrix D;
    GroupMatrix_2D Q;
    MatrixG1_2D C;
    MatrixG1_2D OUT_CURRENT;
    MatrixG1_2D DL;

public:
    Node( int region, int dim);
    ~Node() {}

    void setCX(const CXTable& table) { CX = table; }

    int getREGION() const { return REGION; }
    int getDIM() const { return DIM; }

    const Array<double, 1, Dynamic>& getWIDTH() const { return WIDTH; }
    const Array<double, 1, Dynamic>& getFLUX() const { return FLUX; }
    const Array<int, Dynamic, 2>& getNEIGHBOR() const { return NEIGHBOR; }
    const Array<double, Dynamic, Dynamic>& getCX() const { return CX; }
	const Array<BOUNDARY_TYPE, Dynamic, 2>& getBOUNDARY() const { return BOUNDARY; }
    const GroupMatrix getA() const { return A; }
    const GroupMatrix getD() const { return D; }
    GroupMatrix_2D getQ() const { return Q; }
    MatrixG1_2D getC() const { return C; }
    MatrixG1_2D getOUT_CURRENT() const { return OUT_CURRENT; }
};

#endif