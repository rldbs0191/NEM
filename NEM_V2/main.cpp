#include "solver.h"

int main(void) {
	Solver solver;
	string inputFile;
	cout << "Input file name: ";
	cin >> inputFile;
	solver.ReadInput(inputFile.c_str());
	cout << "====================\n";
	solver.PrintStructure();
	cout << "====================\n";
	solver.PrintCX();
	cout << "====================\n";
	solver.Run();
	system("pause");
	return 0;
}