#include "solver.h"

int main(void) {
	Solver solver;
	solver.ReadInput("input.inp");
	solver.PrintStructure();
	solver.PrintNodeNeighbors(0, 0, 0);
	solver.PrintCX();
	solver.PrintNodeInfo(1, 0, 0);
	solver.Run();
	return 0;
}