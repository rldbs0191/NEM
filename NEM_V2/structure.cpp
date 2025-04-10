#include "structure.h"
#include "solver.h"

Geometry::Geometry()
{
	CelID = 0;
	CEL.clear();
	CELS.clear();
	STRUCTURE.clear();
	SOLVER = nullptr;
}

void Geometry::ReadGeometry(istream& ins)
{
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	cout << "Read Geometry...." << "\n";
	ins >> oneChar;
	while (!flag && ins >> buffer)
	{
		if (!strcmp(buffer, "CEL")) {
			ReadCell(ins);
		}
		else if (!strcmp(buffer, "Structure")) {
			ReadStructure(ins);
		}
		else if (!strcmp(buffer, ENDSTR)) {
			flag = true;
		}
	}
}

void Geometry::ReadCell(istream& ins)
{
	cout << "Read Cell...." << "\n";
	char oneChar;
	string line;

	ins >> CelID;
	ins >> oneChar;

	CEL.clear();
	vector<vector<int>> layer;

	while (getline(ins, line))
	{
		if (line.find(ENDSTR) != string::npos)
			break;

		if (line.empty()) {
			if (!layer.empty()) {
				CEL.push_back(layer);
				layer.clear();
			}
			continue;
		}

		istringstream ss(line);
		string token;
		vector<int> row;

		while (ss >> token)
		{
			try {
				if (token == ".") {
					row.push_back(-1); // vaccum
				}
				else if (token == "0") {
					row.push_back(0); // reflective
				}
				else {
					int region = stoi(token);
					row.push_back(region);
				}
			}
			catch (const invalid_argument&) {
				cerr << "Warning: invalid token in CEL block: " << token << endl;
			}
		}

		if (!row.empty())
			layer.push_back(row);
	}

	if (!layer.empty()) {
		CEL.push_back(layer);
	}

	CELS[CelID] = CEL;
}

void Geometry::ReadStructure(istream& ins)
{
	cout << "Read Structure...." << "\n";
	char oneChar;
	string line;

	ins >> oneChar;
	STRUCTURE.clear();
	GLOBAL_NODE.clear();

	vector<vector<string>> structure2DLines;
	vector<string> currentLine;

	while (getline(ins, line))
	{
		if (line.find(ENDSTR) != string::npos)
			break;

		if (line.empty()) {
			if (!currentLine.empty()) {
				structure2DLines.push_back(currentLine);
				currentLine.clear();
			}
			continue;
		}

		currentLine.push_back(line);
	}
	if (!currentLine.empty()) {
		structure2DLines.push_back(currentLine);
	}

	auto sampleCell = CELS.begin()->second;
	size_t K = sampleCell.size();
	size_t I = sampleCell[0].size();
	size_t J = sampleCell[0][0].size();

	size_t Z = structure2DLines.size();
	size_t Y = structure2DLines[0].size();

	STRUCTURE.resize(Z * K);

	for (size_t z = 0; z < Z; ++z) {
		for (size_t y = 0; y < structure2DLines[z].size(); ++y) {
			istringstream ss(structure2DLines[z][y]);
			string token;
			int x = 0;

			while (ss >> token) {
				int cellID = stoi(token);
				if (CELS.find(cellID) == CELS.end()) {
					cerr << "Invalid CEL ID: " << cellID << endl;
					continue;
				}

				const auto& templateCell = CELS[cellID];

				for (size_t k = 0; k < K; ++k) {
					for (size_t i = 0; i < I; ++i) {
						for (size_t j = 0; j < J; ++j) {
							int globalZ = static_cast<int>(z * K + k);
							int globalY = static_cast<int>(y * I + i);
							int globalX = static_cast<int>(x * J + j);

							if (STRUCTURE[globalZ].size() <= globalY)
								STRUCTURE[globalZ].resize(globalY + 1);
							if (STRUCTURE[globalZ][globalY].size() <= globalX)
								STRUCTURE[globalZ][globalY].resize(globalX + 1);
							STRUCTURE[globalZ][globalY][globalX] = templateCell[k][i][j];
						}
					}
				}
				for (size_t k = 0; k < K; ++k) {
					for (size_t i = 0; i < I; ++i) {
						for (size_t j = 0; j < J; ++j) {
							int globalZ = static_cast<int>(z * K + k);
							int globalY = static_cast<int>(y * I + i);
							int globalX = static_cast<int>(x * J + j);
							if (templateCell[k][i][j] == -1 || templateCell[k][i][j] == 0) continue;
							Node* node = new Node(templateCell[k][i][j], SOLVER);
							node->SetCoefficient();
							GLOBAL_NODE[{globalX, globalY, globalZ}] = node;
						}
					}
				}
				++x;
			}
		}
	}
	for (const auto& entry : GLOBAL_NODE) {
		int x = get<0>(entry.first);
		int y = get<1>(entry.first);
		int z = get<2>(entry.first);
		SetNeighbors(x, y, z);
	}

	for (const auto& entry : GLOBAL_NODE) {
		int x = get<0>(entry.first);
		int y = get<1>(entry.first);
		int z = get<2>(entry.first);
		Node* node = entry.second;
		node->SetBOUNDARY(x, y, z);
	}

	for (const auto& entry : GLOBAL_NODE) {
		int x = get<0>(entry.first);
		int y = get<1>(entry.first);
		int z = get<2>(entry.first);
		Node* node = entry.second;
		node->SetINCOM_CURRENT(x, y, z);
	}
	
}

void Geometry::SetNeighbors(int x, int y, int z) {
	auto it = GLOBAL_NODE.find({ x, y, z });
	if (it == GLOBAL_NODE.end()) return;

	Node* node = it->second;
	int dim = SOLVER->nDIM;

	if (dim > 0) {
		auto left = GLOBAL_NODE.find({ x - 1, y, z });
		node->setNEIGHBOR(X_dir, Left_side, (left != GLOBAL_NODE.end()) ? left->second : nullptr);

		auto right = GLOBAL_NODE.find({ x + 1, y, z });
		node->setNEIGHBOR(X_dir, Right_side, (right != GLOBAL_NODE.end()) ? right->second : nullptr);
	}
	if (dim > 1) {
		auto down = GLOBAL_NODE.find({ x, y - 1, z });
		node->setNEIGHBOR(Y_dir, Left_side, (down != GLOBAL_NODE.end()) ? down->second : nullptr);

		auto up = GLOBAL_NODE.find({ x, y + 1, z });
		node->setNEIGHBOR(Y_dir, Right_side, (up != GLOBAL_NODE.end()) ? up->second : nullptr);
	}
	if (dim > 2) {
		auto back = GLOBAL_NODE.find({ x, y, z - 1 });
		node->setNEIGHBOR(Z_dir, Left_side, (back != GLOBAL_NODE.end()) ? back->second : nullptr);

		auto front = GLOBAL_NODE.find({ x, y, z + 1 });
		node->setNEIGHBOR(Z_dir, Right_side, (front != GLOBAL_NODE.end()) ? front->second : nullptr);
	}
}

void Geometry::PrintStructure() const
{
	cout << "\n[STRUCTURE]\n";

	for (size_t z = 0; z < STRUCTURE.size(); ++z)
	{
		cout << "Layer z = " << z << "\n";
		for (size_t y = 0; y < STRUCTURE[z].size(); ++y)
		{
			cout << "y = " << y << "\n";
			for (size_t x = 0; x < STRUCTURE[z][y].size(); ++x)
			{
				cout << setw(3) << STRUCTURE[z][y][x] << " ";
			}
			cout << "\n";
		}
		cout << "----\n";
	}
}

void Geometry::PrintNodeNeighbors(int x, int y, int z) const
{
	auto it = GLOBAL_NODE.find({ x, y, z });
	if (it == GLOBAL_NODE.end()) {
		cout << "Node (" << x << "," << y << "," << z << ") not found.\n";
		return;
	}
	Node* node = it->second;

	cout << "Neighbors of (" << x << "," << y << "," << z << "):\n";
	for (int d = 0; d < SOLVER->nDIM; ++d) {
		cout << "  Direction " << d << ":\n";
		for (int s = 0; s < 2; ++s) {
			Node* nb = node->getNEIGHBOR(d, s);
			if (nb)
				cout << "    Side " << s << " REGION = " << nb->getREGION() << "\n";
			else
				cout << "    Side " << s << " (null)\n";
		}
	}
}

void Geometry::PrintNodeInfo(int x, int y, int z) const {
	auto it = GLOBAL_NODE.find({ x, y, z });
	if (it == GLOBAL_NODE.end()) {
		cout << "Node (" << x << ", " << y << ", " << z << ") not found.\n";
		return;
	}

	Node* node = it->second;
	int group = SOLVER->nGROUP;
	int dim = SOLVER->nDIM;

	cout << "===== Node (" << x << ", " << y << ", " << z << ") =====\n";
	cout << "REGION: " << node->REGION << "\n";

	cout << "WIDTH: ";
	for (int d = 0; d < dim; ++d)
		cout << node->WIDTH[d] << " ";
	cout << "\n";
	cout << scientific << setprecision(5);
	cout << "FLUX: ";
	for (int g = 0; g < group; ++g)
		cout << node->FLUX[g] << " ";
	cout << "\n";

	cout << "D: ";
	for (int g = 0; g < group; ++g)
		cout << node->D_c[g] << " ";
	cout << "\n";

	cout << "BETA: ";
	for(int u=0;u<dim;u++)
		for (int g = 0; g < group; ++g)
			cout << node->BETA[u][g] << " ";
	cout << "\n";

	const char* dirName[] = { "X", "Y", "Z" };
	const char* sideName[] = { "Left", "Right" };
	for (int d = 0; d < dim; ++d) {
		for (int s = 0; s < 2; ++s) {
			cout << "[Boundary] " << dirName[d] << "-" << sideName[s] << " | ";
			cout << (node->getBOUNDARY(d, s) == VACUUM ? "VACUUM" : "REFLECTIVE");

			Node* neighbor = node->getNEIGHBOR(d, s);
			if (neighbor)
				cout << ", Neighbor REGION: " << neighbor->getREGION();
			else
				cout << ", No neighbor";
			cout << "\n";
		}
	}
	cout << "A Matrix:\n";
	for (int i = 0; i < group; ++i) {
		for (int j = 0; j < group; ++j) {
			cout << node->A[i][j] << " ";
		}
		cout << "\n";
	}

	cout << "Q Matrix:\n";
	for (int i = 0; i < dim; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < group; k++)
				cout << node->Q[i][j][k] << " ";
			cout << "\n";
		}
		cout << "\n";
	}

	cout << "C Matrix:\n";
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < group; k++) {
				cout<<node->C[i][j][k] << " ";
			}
			cout << "\n";
		}
		cout << "\n";
	}

	cout << "out_current:\n";
	for (int i = 0; i < dim; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < group; k++)
				cout << node->OUT_CURRENT[i][j][k] << " ";
			cout << "\n";
		}
		cout << "\n";
	}

	cout << "incom_current:\n";
	for (int i = 0; i < dim; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < group; k++)
				cout << node->INCOM_CURRENT[i][j][k] << " ";
			cout << "\n";
		}
		cout << "\n";
	}
}