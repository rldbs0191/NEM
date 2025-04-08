#include "solver.h"

int main(void) {
	Solver solver;
	std::ofstream debugFile("debug.txt", std::ios::out | std::ios::trunc); 
	debugFile.close();
	solver.ReadInput("input.inp");
	solver.PrintStructure();
	cout << "====================\n";
	solver.PrintNodeInfo(0, 0, 0);
	solver.PrintNodeInfo(1, 0, 0);
	solver.PrintNodeInfo(7, 0, 0);
	cout << "====================\n";
	solver.PrintCX();
	cout << "====================\n";
	solver.Run();
	solver.PrintNodeInfo(0, 0, 0);
	solver.PrintNodeInfo(1, 0, 0);
	solver.PrintNodeInfo(7, 0, 0);
	return 0;
}