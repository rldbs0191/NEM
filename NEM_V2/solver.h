#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "cx.h"
#include "structure.h"

class Solver {

friend class Geometry;
friend class CXManage;
friend class Node;

private:
	int nDIM;
	int nGROUP;
	double* WIDTH;
	CXManage CX;
	Geometry GEOMETRY;
public:
	Solver();
	~Solver();
	void ReadInput(const char* input);
	void ReadTitle(istream& ins);
	void ReadCondition(istream &ins);
	void PrintStructure() const { GEOMETRY.PrintStructure(); };
	void PrintNodeNeighbors(int x, int y, int z) const { GEOMETRY.PrintNodeNeighbors(x,y,z); };
	void PrintCX() const { CX.PrintCX(); };
	void Run();
};
#endif
