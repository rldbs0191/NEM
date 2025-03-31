#include "solver.h"

int main(void) {
	Solver solver;
	solver.ReadInput("input.inp");
	solver.PrintStructure();
	solver.PrintNodeNeighbors(0,0,0);
	solver.PrintCX();
	solver.Run();
	return 0;
}