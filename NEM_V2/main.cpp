#include "solver.h"

int main(void) {
	Solver solver;
	std::ofstream debugFile("debug.txt", std::ios::out | std::ios::trunc); 
	debugFile.close();
	solver.ReadInput("input.inp");
	solver.PrintStructure();
	solver.PrintCX();
	cout << "====================\n";
	solver.Run();

	return 0;
}