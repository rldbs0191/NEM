#ifndef _NODE_H_
#define _NODE_H_

#include "define.h"

class Solver;

class Node {
	friend class Geometry;
private:
	int REGION;
	double* WIDTH;
	double* FLUX;
	double* old_FLUX;
	Node*** NEIGHBOR;
	BOUNDARY_TYPE** BOUNDARY;
	double* D_c;
	double** BETA;
	double** A;
	double*** Q;
	double*** C;
	double*** OUT_CURRENT;
	double*** INCOM_CURRENT;
	double*** DL;
	double* SRC1, * SRC2, * SRC;
	double** M1, ** M2, *** M3, *** M4, ** MM;
	Solver* SOLVER;

public:
	Node(int region, Solver* solver);
	~Node();

	int getREGION() const { return REGION; }
	void SetSolver(Solver* s) { SOLVER = s; }
	const double getWIDTH(int dir) { return WIDTH[dir]; }
	const double getFLUX(int group) { return FLUX[group]; }
	const double getold_FLUX(int group) { return old_FLUX[group]; }
	void setFLUX(int group, double flux) { FLUX[group] = flux; }
	Node* getNEIGHBOR(int dir, int side) const { return NEIGHBOR[dir][side]; }
	void setNEIGHBOR(int dir, int side, Node* node) { NEIGHBOR[dir][side] = node; }
	const BOUNDARY_TYPE getBOUNDARY(int dir, int side) { return BOUNDARY[dir][side]; }
	double*** getOUT_CURRENT() { return OUT_CURRENT; }
	void SetCrossSection(double* D, double* R, double* S, double* F, double* CHI);
	void updateA(double* R, double* S, double* F, double* CHI);
	void SetINCOM_CURRENT(int x, int y, int z);
	void SetBOUNDARY(int x, int y, int z);

	double getSurfaceNetCurrent(int dim, int side, int group) { return OUT_CURRENT[dim][side][group] - INCOM_CURRENT[dim][side][group]; }
	double getSurfaceFlux(int dim, int side, int group) { return INCOM_CURRENT[dim][side][group] + OUT_CURRENT[dim][side][group]; }

	void add_product(double* SRC, double* M1, double* C, int group);
	void GaussianElimination(double** M_in, double* C, double* SRC, int group);

	void updateTransverseLeakage();
	void makeOneDimensionalFlux();
	void updateAverageFlux();
	void updateOutgoingCurrent();
};

#endif