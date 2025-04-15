#include "solver.h"

int main(void) {
	Solver solver;
	string inputFile;
	clock_t start_point, end_point;
	double caltime;
	cout << "Input file name: ";
	cin >> inputFile;
	solver.ReadInput(inputFile.c_str());
	cout << "====================\n";
	solver.PrintStructure();
	cout << "====================\n";
	solver.PrintCX();
	cout << "====================\n";
	start_point = clock();
	solver.Run();
	end_point = clock();
	caltime = (end_point - start_point) / (double)(CLOCKS_PER_SEC);
	cout << fixed << setprecision(3);
	cout << caltime << " sec\n";
	system("pause");
	return 0;
}