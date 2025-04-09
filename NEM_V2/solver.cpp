#include "solver.h"

Solver::Solver()
{
	nDIM = 0;
	nGROUP = 0;
	WIDTH = nullptr;
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
	char OneChar, buffer[LINE_LEN];
	bool flag = false;
	cout << "Title: ";
	ins >> OneChar;
	while (flag == false)
	{
		ins >> buffer;
		string line;
		getline(ins, line);
		if (!strcmp(buffer, ENDSTR))
			flag = true;
		else
			cout << buffer << line << "\n";
	}
}

void Solver::ReadCondition(istream& ins)
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
			WIDTH = new double[nDIM];
		}
		else if (!strcmp(buffer, "GROUP_NUM")) {
			ins >> nGROUP;
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
	const auto& globalNodes = GEOMETRY.GetGlobalNode();

	int group = nGROUP;
	int dim = nDIM;
	double convCrit = 1e-6;
	bool converged = false;
	int iter = 0;
	double prevKeff, norm, denom, maxErr;
	maxErr = 0.0;
	// 중성자속 저장용
	map<tuple<int, int, int>, vector<double>> preFlux;

	do {
		// 1. 이전 중성자속 저장
		//cout << "preFlux\n";
		for (const auto& entry : globalNodes) {
			const auto& coord = entry.first;
			Node* node = entry.second;
			vector<double> flux(group);
			for (int g = 0; g < group; ++g)
				flux[g] = node->getold_FLUX(g);
			preFlux[coord] = flux;
			//cout << preFlux[coord][0] << " " << preFlux[coord][1] << " ";
		}
		//cout << endl;

		prevKeff = K_EFF;

		// 2. 모든 노드 계산
		for (const auto& entry : globalNodes) {
			int x = get<0>(entry.first);
			int y = get<1>(entry.first);
			int z = get<2>(entry.first);
			Node* node = entry.second;
			int region = node->getREGION() - 1;

			node->SetINCOM_CURRENT(x, y, z);
			node->SetCrossSection(CX.DIFFUSION[region],
				CX.REMOVAL[region],
				CX.SCATTERING[region],
				CX.FISSION[region],
				CX.CHI[region]);
			cout << scientific << setprecision(5);
			node->updateTransverseLeakage();
			node->makeOneDimensionalFlux();
			node->updateAverageFlux();
			node->updateOutgoingCurrent();

			//	PrintNodeInfo(x, y, z);
		}

		// 3. keff 계산용: <flux_new, flux_new> / <flux_new, flux_old>
		norm = 0.0;
		denom = 0.0;
		for (const auto& entry : globalNodes) {
			const auto& coord = entry.first;
			Node* node = entry.second;
			for (int g = 0; g < group; ++g) {
				double valNew = node->getFLUX(g);
				double valOld = preFlux[coord][g];
				norm += valNew * valNew;
				denom += valNew * valOld;
			}
		}

		K_EFF = prevKeff * norm / denom;

		// 4. 수렴 판단
		maxErr = 0.0;
		for (const auto& entry : globalNodes) {
			const auto& coord = entry.first;
			Node* node = entry.second;
			for (int g = 0; g < group; ++g) {
				double diff = fabs(node->getFLUX(g) - preFlux[coord][g]);
				double rel = diff / preFlux[coord][g];
				if (rel > maxErr)
					maxErr = rel;
			}
		}

		for (const auto& entry : globalNodes) {
			Node* node = entry.second;
			for (int g = 0; g < group; ++g) {
				node->setFLUX(g, node->getFLUX(g) / K_EFF);
			}
		}

		cout << "Iteration " << iter + 1 << ": K_EFF = " << K_EFF << ", Error = " << maxErr << endl;

		converged = (maxErr < convCrit);
		iter++;
	} while (!converged);

	cout << "Final keff = " << K_EFF << ", after " << iter << " iterations.\n";
}
