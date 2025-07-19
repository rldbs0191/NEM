#ifndef _CX_H_
#define _CX_H_

#include "define.h"

typedef double** CXArray;
class Solver;  

class CXManage {

//friend class Solver;
friend class Node;

private:
	int nRegion;
	int Group;
	Solver* SOLVER;
public:
	CXManage();
	~CXManage();
	CXArray DIFFUSION;
	CXArray REMOVAL;
	CXArray SCATTERING;	
	CXArray FISSION;
	CXArray CHI;
	void SetSolver(Solver* s);
	void ReadCX(istream& ins);
	void ReadCXTable(istream& ins, int groupIndex);
	void SetCoefficient();
	void PrintCX() const;
};

#endif
