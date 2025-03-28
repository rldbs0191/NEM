#include "cx.h"
#include "solver.h"

CXManage::CXManage() {
	nRegion = 0;
	Group = 0;
	SOLVER = nullptr;
	DIFFUSION.setZero();
	REMOVAL.setZero();
	SCATTERING.setZero();
	FISSION.setZero();
}

void CXManage::ReadCX(istream& ins) {
	char oneChar, buffer[LINE_LEN];
	bool flag = false;

	int tempGroup = -1;
	bool regionSet = false;

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

		// resize 이후에만 ReadCXTable 실행
		if (regionSet && tempGroup != -1) {
			DIFFUSION.resize(nRegion, SOLVER->nGROUP);
			REMOVAL.resize(nRegion, SOLVER->nGROUP);
			SCATTERING.resize(nRegion, SOLVER->nGROUP);
			FISSION.resize(nRegion, SOLVER->nGROUP);
			ReadCXTable(ins, tempGroup-1);
			tempGroup = -1; // reset
		}
	}
}

void CXManage::ReadCXTable(istream& ins, int groupIndex) {
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	ins >> oneChar;
	while (!flag)
	{
		ins >> buffer;
		if (!strcmp(buffer, "DIFFUSION"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> DIFFUSION(i, groupIndex);
		}
		else if (!strcmp(buffer, "REMOVAL"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> REMOVAL(i, groupIndex);
		}
		else if (!strcmp(buffer, "SCATTER"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> SCATTERING(i, groupIndex);
		}
		else if (!strcmp(buffer, "FISSION"))
		{
			for (int i = 0; i < nRegion; ++i)
				ins >> FISSION(i, groupIndex);
		}
		else if (!strcmp(buffer, ENDSTR))
			flag = true;
	}
}

void CXManage::PrintCX() const {
	cout << "\n[CX]\n";
	cout << "Region 수: " << nRegion << "\n";
	cout << "Group 수: " << Group << "\n";
	cout << scientific << setprecision(2);
	cout << "\n[DIFFUSION]\n" << DIFFUSION << "\n";
	cout << "\n[REMOVAL]\n" << REMOVAL << "\n";
	cout << "\n[SCATTERING]\n" << SCATTERING << "\n";
	cout << "\n[FISSION]\n" << FISSION << "\n";
	cout << defaultfloat;
}