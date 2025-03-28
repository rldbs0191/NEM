#ifndef _CX_H_
#define _CX_H_

#include "define.h"

typedef Array<double, Dynamic, Dynamic> CXArray;

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
	Solver* SOLVER;
public:
	CXManage();
	~CXManage() {};

	void SetSolver(Solver* s) { SOLVER = s; }
	void ReadCX(istream& ins);
	void ReadCXTable(istream& ins, int groupIndex);
	void PrintCX() const;
};

#endif
