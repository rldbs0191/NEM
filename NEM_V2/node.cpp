#include "node.h"
#include "solver.h"

Node::Node() {
	REGION = 0;
	FLUX = nullptr;
	BETA = nullptr;
	WIDTH = nullptr;
	NEIGHBOR = nullptr;
	Q = nullptr;
	C = nullptr;
	OUT_CURRENT = nullptr;
	INCOM_CURRENT = nullptr;
	DL = nullptr;
	BOUNDARY = nullptr;
	SOLVER = nullptr;
}

Node::Node( int region, Solver* solver) {
	SOLVER = solver;
	REGION = region;
	int group = solver->nGROUP;
	int dim = solver->nDIM;

	WIDTH = new double[dim] ;
	for (int i = 0; i < dim; i++)
		WIDTH[i] = solver->WIDTH[i];
	FLUX = new double[group] {};
	for (int i = 0; i < group; i++)
		FLUX[i] = 1.0;
	BETA = new double[group] {};

	NEIGHBOR = new Node**[dim];
	BOUNDARY = new BOUNDARY_TYPE * [dim];
	for (int i = 0; i < dim; ++i) {
		NEIGHBOR[i] = new Node * [2];
		for (int j = 0; j < 2; j++) { NEIGHBOR[i][j] = nullptr; }
		BOUNDARY[i] = new BOUNDARY_TYPE[2];
	}

	Q = new double** [dim];
	C = new double** [dim];
	OUT_CURRENT = new double** [dim];
	INCOM_CURRENT = new double** [dim];
	DL = new double** [dim];

	for (int i = 0; i < dim; ++i) {
		Q[i] = new double* [4];
		C[i] = new double* [5];
		OUT_CURRENT[i] = new double* [2];
		INCOM_CURRENT[i] = new double* [2];
		DL[i] = new double* [3];

		for (int j = 0; j < 4; ++j)
			Q[i][j] = new double[group];
		for (int j = 0; j < 5; ++j)
			C[i][j] = new double[group];
		for (int j = 0; j < 2; ++j) {
			OUT_CURRENT[i][j] = new double[group];
			INCOM_CURRENT[i][j] = new double[group];
			for (int k = 0; k < group; k++)
			{
				OUT_CURRENT[i][j][k] = 1.0;
				INCOM_CURRENT[i][j][k] = 1.0;
			}
		}
		for (int j = 0; j < 3; ++j)
		{
			DL[i][j] = new double[group];
			for (int k = 0; k < group; k++)
				DL[i][j][k] = 1.0;
		}
	}
}

Node::~Node() {
	int dim = SOLVER ? SOLVER->nDIM : 0;
	int group = SOLVER ? SOLVER->nGROUP : 0;

	delete[] WIDTH;
	delete[] FLUX;
	delete[] BETA;

	if (NEIGHBOR) {
		for (int i = 0; i < dim; ++i)
			delete[] NEIGHBOR[i];
		delete[] NEIGHBOR;
	}

	if (BOUNDARY) {
		for (int i = 0; i < dim; ++i)
			delete[] BOUNDARY[i];
		delete[] BOUNDARY;
	}

	auto delete3D = [&](double***& arr, int outer, int inner) {
		if (!arr) return;
		for (int i = 0; i < outer; ++i) {
			for (int j = 0; j < inner; ++j) {
				delete[] arr[i][j];
			}
			delete[] arr[i];
		}
		delete[] arr;
		};

	delete3D(Q, dim, 4);
	delete3D(C, dim, 5);
	delete3D(OUT_CURRENT, dim, 2);
	delete3D(INCOM_CURRENT, dim, 2);
	delete3D(DL, dim, 3);
}

void Node::updateTransverseLeakage( int dim, int group ) {

}

void Node::update1Dflux() {

}

void Node::updateAverageFlux() {

}

void Node::updateOutgoingCurrent() {

}