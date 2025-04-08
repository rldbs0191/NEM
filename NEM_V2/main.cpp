#include "solver.h"

int main(void) {
	Solver solver;
	solver.ReadInput("input.inp");
	cout << "====================\n";
	solver.PrintStructure();
	solver.PrintNodeInfo(0, 0, 0);
	solver.PrintNodeInfo(1, 0, 0);
	solver.PrintNodeInfo(7, 0, 0);
	cout << "====================\n";
	solver.PrintCX();
	cout << "====================\n";
	solver.Run();
	return 0;
}