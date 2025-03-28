#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "cx.h"
#include "structure.h"

class Solver {

friend class Geometry;
friend class CXManage;

private:
	int nDIM;
	int nGROUP;
	Array<double, 1, Dynamic> WIDTH;
	CXManage CX;
	Geometry GEOMETRY;
public:
	Solver();
	~Solver() {}
	void ReadInput(const char* input);
	void ReadTitle(istream& ins);
	void ReadCondition(istream &ins);
	void PrintStructure() const { GEOMETRY.PrintStructure(); };
	void PrintCX() const { CX.PrintCX(); };
	void Run();
};
#endif
