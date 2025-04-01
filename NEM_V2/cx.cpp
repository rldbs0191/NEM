#include "cx.h"
#include "solver.h"

CXManage::CXManage() : DIFFUSION(nullptr), REMOVAL(nullptr), SCATTERING(nullptr), FISSION(nullptr) {
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
	}
	delete[] DIFFUSION;
	delete[] REMOVAL;
	delete[] SCATTERING;
	delete[] FISSION;
}

void CXManage::ReadCX(istream& ins) {
	char oneChar, buffer[LINE_LEN];
	bool flag = false;

	int tempGroup = -1;
	bool regionSet = false;
	cout << "Read CX...." << "\n";
	ins >> oneChar;

	while (!flag && ins >> buffer) {
		if (!strcmp(buffer, "REGION_NUM")) {
			ins >> nRegion;
			regionSet = true;
		}
		else if (!strcmp(buffer, "GROUP")) {
			ins >> Group;
			tempGroup = Group;
		}
		else if (!strcmp(buffer, ENDSTR)) {
			flag = true;
		}

		if (regionSet && tempGroup != -1) {
			DIFFUSION = new double* [nRegion];
			REMOVAL = new double* [nRegion];
			SCATTERING = new double* [nRegion];
			FISSION = new double* [nRegion];

			for (int i = 0; i < nRegion; ++i) {
				DIFFUSION[i] = new double[Group] {};
				REMOVAL[i] = new double[Group] {};
				SCATTERING[i] = new double[Group] {};
				FISSION[i] = new double[Group] {};
			}
			ReadCXTable(ins, tempGroup-1);
			tempGroup = -1; // reset
		}
	}
}

void CXManage::ReadCXTable(istream& ins, int groupIndex) {
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	cout << "Read CXTable...." << "\n";
	ins >> oneChar;
	while (!flag)
	{
		ins >> buffer;
		if (!strcmp(buffer, "DIFFUSION"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> DIFFUSION[i][groupIndex];
		}
		else if (!strcmp(buffer, "REMOVAL"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> REMOVAL[i][groupIndex];
		}
		else if (!strcmp(buffer, "SCATTER"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> SCATTERING[i][groupIndex];
		}
		else if (!strcmp(buffer, "FISSION"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> FISSION[i][groupIndex];
		}
		else if (!strcmp(buffer, ENDSTR))
			flag = true;
	}
}

void CXManage::PrintCX() const {
	cout << "\n[CX]\n";
	cout << "Region ¼ö: " << nRegion << "\n";
	cout << "Group ¼ö: " << Group << "\n";
	cout << scientific << setprecision(2);
	cout << "\n[DIFFUSION]\n" << "\n";
	cout << "\n[REMOVAL]\n" << "\n";
	cout << "\n[SCATTERING]\n" << "\n";
	cout << "\n[FISSION]\n" << "\n";
	cout << defaultfloat;
}