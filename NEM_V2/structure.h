#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_

#include "define.h"
#include "node.h"
#include <map>

class Solver;

typedef vector<vector<vector<int>>> Cell;
typedef map<int, Cell> Cells;                                
typedef vector<vector<vector<int>>> Structure;    
typedef map<tuple<int, int, int>, Node*> NodeCoord;

class Geometry {
private:
	int CelID;
	NodeCoord GLOBAL_NODE;
	Cell CEL;
	Cells CELS;
	Structure STRUCTURE;
	Solver* SOLVER;

public:
	Geometry();
	~Geometry() {};
	const NodeCoord& GetGlobalNode() const { return GLOBAL_NODE; }
	void SetSolver(Solver* s) { SOLVER = s; }
	void ReadGeometry(istream& ins);
	void ReadCell(istream& ins);
	void ReadStructure(istream& ins);
	void SetNeighbors(int x, int y, int z);	
	void PrintStructure() const;
	const Structure& GetStructure() const { return STRUCTURE; }
	void PrintNodeNeighbors(int x, int y, int z) const;
	void PrintNodeInfo(int x, int y, int z) const;
	int GetTotalNodeCount() const {
		int totalNodes = 0;
		for (const auto& layer : STRUCTURE) {
			for (const auto& row : layer) {
				totalNodes += static_cast<int>(row.size());
			}
		}
		return totalNodes;
	}
};

#endif
