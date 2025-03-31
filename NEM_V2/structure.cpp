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
				int region = stoi(token);
				row.push_back(region);
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
	char oneChar;
	string line;

	ins >> oneChar; 
	STRUCTURE.clear();

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
	size_t X = 0;

	// 전체 구조 크기 정의
	STRUCTURE.resize(Z * K);
	for (int z = 0; z < Z * K; ++z) {
		STRUCTURE[z].resize(Y * I);
		for (int y = 0; y < Y * I; ++y)
			STRUCTURE[z][y].resize(0);
	}

	for (int z = 0; z < Z; ++z) {
		for (int y = 0; y < structure2DLines[z].size(); ++y) {
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

				for (int k = 0; k < K; ++k) {
					for (int i = 0; i < I; ++i) {
						for (int j = 0; j < J; ++j) {
							int globalZ = static_cast<int>(z * K + k);
							int globalY = static_cast<int>(y * I + i);
							int globalX = static_cast<int>(x * J + j);

							if (STRUCTURE.size() <= globalZ)
								STRUCTURE.resize(globalZ + 1);

							if (STRUCTURE[globalZ].size() <= globalY)
								STRUCTURE[globalZ].resize(globalY + 1);

							if (STRUCTURE[globalZ][globalY].size() <= globalX)
								STRUCTURE[globalZ][globalY].resize(globalX + 1);

							STRUCTURE[globalZ][globalY][globalX] = templateCell[k][i][j];
							GLOBAL_NODE[{globalX, globalY, globalZ}] = Node(templateCell[k][i][j], SOLVER->nDIM);
							;
						}
					}
				}
				++x;
			}
		}
	}
	SetNeighbors();
}

void Geometry::SetNeighbors()
{
	int dim = SOLVER->nDIM;

	map<tuple<int, int, int>, Node>::iterator it;
	for (it = GLOBAL_NODE.begin(); it != GLOBAL_NODE.end(); ++it)
	{
		const tuple<int, int, int>& coord = it->first;
		Node& node = it->second;

		int x = get<0>(coord);
		int y = get<1>(coord);
		int z = get<2>(coord);

		if (dim > 0) {
			map<tuple<int, int, int>, Node>::iterator left =
				GLOBAL_NODE.find(make_tuple(x - 1, y, z));
			map<tuple<int, int, int>, Node>::iterator right =
				GLOBAL_NODE.find(make_tuple(x + 1, y, z));
			if (left != GLOBAL_NODE.end())
				node.setNEIGHBOR(X_dir, Left_side, &left->second);
			if (right != GLOBAL_NODE.end())
				node.setNEIGHBOR(X_dir, Right_side, &right->second);
		}

		if (dim > 1) {
			map<tuple<int, int, int>, Node>::iterator down =
				GLOBAL_NODE.find(make_tuple(x, y - 1, z));
			map<tuple<int, int, int>, Node>::iterator up =
				GLOBAL_NODE.find(make_tuple(x, y + 1, z));
			if (down != GLOBAL_NODE.end())
				node.setNEIGHBOR(Y_dir, Left_side, &down->second);
			if (up != GLOBAL_NODE.end())
				node.setNEIGHBOR(Y_dir, Right_side, &up->second);
		}

		if (dim > 2) {
			map<tuple<int, int, int>, Node>::iterator back =
				GLOBAL_NODE.find(make_tuple(x, y, z - 1));
			map<tuple<int, int, int>, Node>::iterator front =
				GLOBAL_NODE.find(make_tuple(x, y, z + 1));
			if (back != GLOBAL_NODE.end())
				node.setNEIGHBOR(Z_dir, Left_side, &back->second);
			if (front != GLOBAL_NODE.end())
				node.setNEIGHBOR(Z_dir, Right_side, &front->second);
		}
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
	const Node& node = it->second;

	cout << "Neighbors of (" << x << "," << y << "," << z << "):\n";
	for (int d = 0; d < node.getDIM(); ++d) {
		cout << "  Direction " << d << ":\n";
		for (int s = 0; s < 2; ++s) {
			Node* nb = node.getNEIGHBOR(d,s);
			if (nb)
				cout << "    Side " << s << " → REGION = " << nb->getREGION() << "\n";
			else
				cout << "    Side " << s << " → (null)\n";
		}
	}
}