#include "node.h"

Node::Node( int region, int dim) : REGION(region), DIM(dim) {
    FLUX.setZero();
    A.setZero();
    D.setZero();

	NEIGHBOR.resize(DIM, 2);
    NEIGHBOR.setZero();

    WIDTH.resize(DIM);
    WIDTH.setZero();

	BOUNDARY.resize(DIM, 2);
	for (int i = 0; i < DIM; ++i)
		for (int j = 0; j < 2; ++j)
			BOUNDARY(i, j) = REFLECTIVE;

    Q.resize(DIM, vector<GroupMatrix>(4));
    for (int i = 0; i < DIM; ++i)
        for (int j = 0; j < 4; ++j)
            Q[i][j].setZero();

    C.resize(DIM, vector<MatrixG1>(5));
    for (int i = 0; i < DIM; ++i)
        for (int j = 0; j < 5; ++j)
            C[i][j].setZero();

    OUT_CURRENT.resize(DIM, vector<MatrixG1>(2));
    for (int i = 0; i < DIM; ++i)
        for (int j = 0; j < 2; ++j)
            OUT_CURRENT[i][j].setZero();

	DL.resize(DIM, vector<MatrixG1>(2));
	for (int i = 0; i < DIM; ++i)
		for (int j = 0; j < 2; ++j)
			DL[i][j].setZero();
}
