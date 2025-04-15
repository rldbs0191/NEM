#include "solver.h"
#include <omp.h>

Solver::Solver()
{
	nDIM = 0;
	nGROUP = 0;
	WIDTH = nullptr;
	ALBEDO = nullptr;
	CX = CXManage();
	GEOMETRY = Geometry();
}

Solver::~Solver() {
	delete[] WIDTH;
	delete[] ALBEDO;
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
		else if (!strcmp(buffer, "ALBEDO")) {
			ALBEDO = new double* [nDIM];
			for (int i = 0; i < nDIM; i++)
			{
				ALBEDO[i] = new double[2];
				for (int j = 0; j < 2; j++)
				{
					ins >> ALBEDO[i][j];
				}
			}
		}
		else if (!strcmp(buffer, ENDSTR))
			flag = true;
	}
}

void Solver::Run()
{
	cout << "Solver is running with OpenMP..." << endl;
	int group = nGROUP;
	int dim = nDIM;
	double convCrit = 1e-6;
	bool converged = false;
	int iter = 0;
	double prevKeff, norm, denom, maxErr = 0.0;

	const auto& globalNodes = GEOMETRY.GetGlobalNode();
	std::vector<Node*> nodeVec;
	std::vector<std::tuple<int, int, int>> coordVec;
	nodeVec.reserve(globalNodes.size());
	coordVec.reserve(globalNodes.size());

	for (const auto& entry : globalNodes) {
		coordVec.push_back(entry.first);
		nodeVec.push_back(entry.second);
	}

	std::vector<std::vector<double>> preFlux(nodeVec.size(), std::vector<double>(group));

	for (int i = 0; i < nodeVec.size(); ++i) {
		int region = nodeVec[i]->getREGION() - 1;
		nodeVec[i]->SetCrossSection(CX.DIFFUSION[region], CX.REMOVAL[region], CX.SCATTERING[region], CX.FISSION[region], CX.CHI[region]);
	}

	do {
#pragma omp parallel for
		for (int i = 0; i < nodeVec.size(); ++i) {
			for (int g = 0; g < group; ++g)
				preFlux[i][g] = nodeVec[i]->getold_FLUX(g);
		}

		prevKeff = K_EFF;

#pragma omp parallel for
		for (int i = 0; i < nodeVec.size(); ++i) {
			int x = std::get<0>(coordVec[i]);
			int y = std::get<1>(coordVec[i]);
			int z = std::get<2>(coordVec[i]);
			int region = nodeVec[i]->getREGION() - 1;

			nodeVec[i]->SetINCOM_CURRENT(x, y, z);
			nodeVec[i]->updateA(CX.REMOVAL[region], CX.SCATTERING[region], CX.FISSION[region], CX.CHI[region]);
			nodeVec[i]->updateTransverseLeakage();
			nodeVec[i]->makeOneDimensionalFlux();
			nodeVec[i]->updateAverageFlux();
			nodeVec[i]->updateOutgoingCurrent();
		}

		norm = 0.0;
		denom = 0.0;
#pragma omp parallel for reduction(+:norm, denom)
		for (int i = 0; i < nodeVec.size(); ++i) {
			for (int g = 0; g < group; ++g) {
				double valNew = nodeVec[i]->getFLUX(g);
				double valOld = preFlux[i][g];
				norm += valNew * valNew;
				denom += valNew * valOld;
			}
		}

		K_EFF = prevKeff * norm / denom;

		maxErr = 0.0;
#pragma omp parallel
		{
			double threadMax = 0.0;
#pragma omp for nowait
			for (int i = 0; i < nodeVec.size(); ++i) {
				for (int g = 0; g < group; ++g) {
					double diff = fabs(nodeVec[i]->getFLUX(g) - preFlux[i][g]);
					double rel = diff / preFlux[i][g];
					if (rel > threadMax)
						threadMax = rel;
				}
			}
#pragma omp critical
			{
				if (threadMax > maxErr)
					maxErr = threadMax;
			}
		}

#pragma omp parallel for
		for (int i = 0; i < nodeVec.size(); ++i) {
			for (int g = 0; g < group; ++g)
				nodeVec[i]->setFLUX(g, nodeVec[i]->getFLUX(g) / K_EFF);
		}

		cout << fixed << setprecision(5);
		cout << "Iteration " << iter + 1 << ": K_EFF = " << K_EFF;
		cout << scientific << setprecision(5);
		cout << ", Error = " << maxErr << endl;

		converged = (maxErr < convCrit);
		iter++;

	} while (!converged);

	cout << fixed << setprecision(5);
	cout << "Converged after " << iter << " iterations.  K_EFF: " << K_EFF << "  ";
	cout << scientific << setprecision(5);
	cout << "Error: " << maxErr << endl;

	const auto& structure = GEOMETRY.GetStructure();
	std::vector<std::ofstream> fluxFiles(group);
	for (int g = 0; g < group; ++g) {
		string fileName = "flux_group_" + to_string(g + 1) + ".txt";
		fluxFiles[g].open(fileName, std::ios::trunc);
		fluxFiles[g] << scientific << setprecision(2);

		for (int z = 0; z < structure.size(); ++z) {
			fluxFiles[g] << "Z = " << z << "\n";
			for (int y = 0; y < structure[z].size(); ++y) {
				for (int x = 0; x < structure[z][y].size(); ++x) {
					int region = structure[z][y][x];
					if (region <= 0) {
						fluxFiles[g] << "\t\t\t";
					}
					else {
						Node* node = GEOMETRY.GetGlobalNode().at({ x, y, z });
						fluxFiles[g] << node->getFLUX(g) << "\t";
					}
				}
				fluxFiles[g] << "\n";
			}
			fluxFiles[g] << "\n";
		}
		fluxFiles[g].close();
	}
}