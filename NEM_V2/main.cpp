#include "solver.h"

int main(void) {
	Solver solver;
	string inputFile;
	cout << "Input file name: ";
	cin >> inputFile;
	solver.ReadInput(inputFile.c_str());
//	solver.PrintStructure();
//	solver.PrintCX();
//	cout << "====================\n";
	solver.Run();
	return 0;
}