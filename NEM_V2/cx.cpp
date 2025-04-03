#include "cx.h"
#include "solver.h"

CXManage::CXManage() : DIFFUSION(nullptr), REMOVAL(nullptr), SCATTERING(nullptr), FISSION(nullptr), CHI(nullptr) {
	nRegion = 0;
	Group = 0;
	SOLVER = nullptr;
}

CXManage::~CXManage() {
	for (int i = 0; i < nRegion; ++i) {
		delete[] DIFFUSION[i];
		delete[] REMOVAL[i];
		delete[] SCATTERING[i];
		delete[] FISSION[i];
		delete[] CHI[i];
	}
	delete[] DIFFUSION;
	delete[] REMOVAL;
	delete[] SCATTERING;
	delete[] FISSION;
	delete[] CHI;
}

void CXManage::SetSolver(Solver* s){
	SOLVER = s;
	Group = s->nGROUP;
}

void CXManage::ReadCX(istream& ins) {
	char oneChar, buffer[LINE_LEN];
	bool flag = false;

	cout << "Read CX...." << "\n";
	ins >> oneChar;

	while (!flag && ins >> buffer) {
		if (!strcmp(buffer, "REGION_NUM")) {
			ins >> nRegion;
			DIFFUSION = new double* [nRegion];
			REMOVAL = new double* [nRegion];
			SCATTERING = new double* [nRegion];
			FISSION = new double* [nRegion];
			CHI = new double* [nRegion];
			for (int i = 0; i < nRegion; ++i) {
				DIFFUSION[i] = new double[Group]();
				REMOVAL[i] = new double[Group]();
				SCATTERING[i] = new double[Group]();
				FISSION[i] = new double[Group]();
				CHI[i] = new double[Group]();
			}
		}
		else if (!strcmp(buffer, "GROUP")) {
			int groupIndex;
			ins >> groupIndex;
			ReadCXTable(ins, groupIndex - 1);
		}
		else if (!strcmp(buffer, ENDSTR)) {
			flag = true;
		}
	}
}

void CXManage::ReadCXTable(istream& ins, int groupIndex) {
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	cout << "Read CXTable...." << "\n";
	ins >> oneChar;

	while (!flag) {
		ins >> buffer;

		if (!strcmp(buffer, "DIFFUSION")) {
			for (int i = 0; i < nRegion; ++i) {
				ins >> DIFFUSION[i][groupIndex];
			}
		}
		else if (!strcmp(buffer, "REMOVAL")) {
			for (int i = 0; i < nRegion; ++i) {
				ins >> REMOVAL[i][groupIndex];
			}
		}
		else if (!strcmp(buffer, "SCATTER")) {
			for (int i = 0; i < nRegion; ++i) {
				ins >> SCATTERING[i][groupIndex];
			}
		}
		else if (!strcmp(buffer, "FISSION")) {
			for (int i = 0; i < nRegion; ++i) {
				ins >> FISSION[i][groupIndex];
			}
		}
		else if (!strcmp(buffer, "CHI")) {
			for (int i = 0; i < nRegion; ++i) {
				ins >> CHI[i][groupIndex];
			}
		}
		else if (!strcmp(buffer, ENDSTR)) {
			flag = true;
		}
	}
}

void CXManage::SetCoefficient() {
	const auto& globalNodes = SOLVER->GEOMETRY.GetGlobalNode();
	for (auto& entry : globalNodes) {
		Node* node = entry.second;
		
		int region = node->getREGION()-1;
		node->SetCrossSection(
			DIFFUSION[region],
			REMOVAL[region],
			SCATTERING[region],
			FISSION[region],
			CHI[region]
		);
	}
}

void CXManage::PrintCX() const {
	cout << "\n[CX]\n";
	cout << "Region : " << nRegion << "\n";
	cout << "Group : " << Group << "\n";
	cout << scientific << setprecision(2);
	cout << "\n[DIFFUSION]\n" << "\n";
	for (int i = 0; i < nRegion; i++) {
		for (int j = 0; j < Group; j++)
			cout << DIFFUSION[i][j]<<" ";
		cout << "\n";
	}
	cout << "\n";
	cout << "\n[REMOVAL]\n" << "\n";
	for (int i = 0; i < nRegion; i++) {
		for (int j = 0; j < Group; j++)
			cout << REMOVAL[i][j] << " ";
		cout << "\n";
	}
	cout << "\n";
	cout << "\n[SCATTERING]\n" << "\n";
	for (int i = 0; i < nRegion; i++) {
		for (int j = 0; j < Group; j++)
			cout << SCATTERING[i][j] << " ";
		cout << "\n";
	}
	cout << "\n";
	cout << "\n[FISSION]\n" << "\n";
	for (int i = 0; i < nRegion; i++) {
		for (int j = 0; j < Group; j++)
			cout << FISSION[i][j] << " ";
		cout << "\n";
	}
	cout << "\n";
	cout << "\n[CHI]\n" << "\n";
	for (int i = 0; i < nRegion; i++) {
		for (int j = 0; j < Group; j++)
			cout << CHI[i][j] << " ";
		cout << "\n";
	}
	cout << "\n";
	cout << defaultfloat;
}