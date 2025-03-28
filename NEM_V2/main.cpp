#include "solver.h"

int main(void) {
	Solver solver;
	solver.ReadInput("input.inp");
	solver.PrintStructure();
	solver.PrintCX();
	solver.Run();
	return 0;
}