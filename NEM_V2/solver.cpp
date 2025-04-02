#include "solver.h"

Solver::Solver()
{
	nDIM = 0;
	nGROUP = 0;
	WIDTH = nullptr;
	X = nullptr;
	CX = CXManage();
	GEOMETRY = Geometry();
}

Solver::~Solver() {
	if (WIDTH) {
		delete[] WIDTH;
	}
}

void Solver::ReadInput(const char* input)
{
	ifstream file(input);
	char buffer[LINE_LEN];
	bool flag = false;
	if (!file.is_open())
	{
		cerr << "Error: Cannot open the input file." << endl;
		exit(1);
	}
	while (file >> buffer) {
		if (!strcmp(buffer, "Title"))
			this->ReadTitle(file);
		else if (!strcmp(buffer, "Condition"))
		{
			this->ReadCondition(file);
		}
		else if (!strcmp(buffer, "CX")) {
			CX.SetSolver(this);
			CX.ReadCX(file);
		}
		else if (!strcmp(buffer, "Geometry")) {
			GEOMETRY.SetSolver(this);
			GEOMETRY.ReadGeometry(file);
		}
	}
	file.close();
	CX.SetCoefficient();
}

void Solver::ReadTitle(istream& ins)
{
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	ins >> oneChar;
	while (flag == false)
	{
		ins >> buffer;
		if (!strcmp(buffer, "TITLE"))
		{
			string line;
			getline(ins, line);
			cout << "Title: " << line << endl;
		}
		else if (!strcmp(buffer, ENDSTR))
			flag = true;
	}
}

void Solver::ReadCondition(istream &ins)
{
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	cout << "Read Condition...." << "\n";
	ins >> oneChar;

	while (flag == false)
	{
		ins >> buffer;

		if (!strcmp(buffer, "DIM")) {
			ins >> nDIM;
			WIDTH = new double [nDIM];
		}
		else if (!strcmp(buffer, "GROUP_NUM")) {
			ins >> nGROUP;
			X = new double[nGROUP];
			for (int i = 0; i < nGROUP; i++)
				X[i] = 1.0;
		}
		else if (!strcmp(buffer, "WIDTH"))
		{
			for (int i = 0; i < nDIM; i++)
			{
				ins >> WIDTH[i];
			}
		}
		else if (!strcmp(buffer, ENDSTR))
			flag = true;
	}	
}

void Solver::Run()
{
	cout << "Solver is running..." << endl;
}


