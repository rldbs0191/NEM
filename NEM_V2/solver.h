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
	double K_EFF=1.0;
	CXManage CX;
	Geometry GEOMETRY;
public:
	Solver();
	~Solver();
	void ReadInput(const char* input);
	void ReadTitle(istream& ins);
	void ReadCondition(istream &ins);
	int GetTotalNodeCount() const {
		return GEOMETRY.GetTotalNodeCount();
	}
	void PrintStructure() const { GEOMETRY.PrintStructure(); }
	void PrintNodeNeighbors(int x, int y, int z) const { GEOMETRY.PrintNodeNeighbors(x,y,z); }
	void PrintCX() const { CX.PrintCX(); };
	void PrintNodeInfo(int x, int y, int z) const { GEOMETRY.PrintNodeInfo(x, y, z); }
	void Run();
};
#endif
