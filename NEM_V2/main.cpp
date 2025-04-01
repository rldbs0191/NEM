#include "solver.h"

int main(void) {
	Solver solver;
	cout << "[1] Reading input..." << endl;
	solver.ReadInput("input.inp");
	cout << "[2] Printing structure..." << endl;
	solver.PrintStructure();
	cout << "[3] Printing node neighbors..." << endl;
	solver.PrintNodeNeighbors(0, 0, 0);
	cout << "[4] Printing CX..." << endl;
	solver.PrintCX();
	cout << "[5] Running solver..." << endl;
	solver.Run();
	cout << "[6] Finished." << endl;
	return 0;
}