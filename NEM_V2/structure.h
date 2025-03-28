// structure.h
#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_

#include "define.h"
#include "node.h"
#include <map>

class Solver;

typedef std::vector<std::vector<std::vector<Node>>> Cell;         
typedef std::map<int, Cell> Cells;                                
typedef std::vector<std::vector<std::vector<Cell>>> Structure;    

class Geometry {

	friend class Solver;

private:
	int CelID;
	Cell CEL;
	Cells CELS;
	Structure STRUCTURE;
	Solver* SOLVER;

public:
	Geometry();
	~Geometry() {};

	void SetSolver(Solver* s) { SOLVER = s; }
	void ReadGeometry(std::istream& ins);
	void ReadCell(std::istream& ins);
	void ReadStructure(std::istream& ins);
	void PrintStructure() const;
};

#endif
