#include "solver.h"

Solver::Solver()
{
	nDIM = 0;
	nGROUP = 0;
	CX = CXManage();
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
			this->ReadCondition(file);
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

	ins >> oneChar;

	while (flag == false)
	{
		ins >> buffer;

		if (!strcmp(buffer, "DIM"))
			ins >> nDIM;
		else if (!strcmp(buffer, "GROUP_NUM"))
			ins >> nGROUP;
		else if (!strcmp(buffer, "WIDTH"))
		{
			for (int i = 0; i < nDIM; ++i)
				for (int j=0; j < 2; ++j)
					ins >> WIDTH[i][j];
		}
		else if (!strcmp(buffer, ENDSTR))
			flag = true;
	}	
}

void Solver::Run()
{
	cout << "Solver is running..." << endl;
}


