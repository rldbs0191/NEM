#ifndef _CX_H_
#define _CX_H_

#include "define.h"

typedef double** CXArray;

class CXManage {

friend class Solver;
friend class Node;

private:
	int nRegion;
	int Group;
	CXArray DIFFUSION;
	CXArray REMOVAL;
	CXArray SCATTERING;	
	CXArray FISSION;
	CXArray CHI;
	Solver* SOLVER;
public:
	CXManage();
	~CXManage();

	void SetSolver(Solver* s);
	void ReadCX(istream& ins);
	void ReadCXTable(istream& ins, int groupIndex);
	void SetCoefficient();
	void PrintCX() const;
};

#endif
