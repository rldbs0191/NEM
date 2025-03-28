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

void Geometry::ReadGeometry(std::istream& ins)
{
	char oneChar, buffer[LINE_LEN];
	bool flag = false;
	ins >> oneChar;
	while (!flag && ins >> buffer)
	{
		if (!strcmp(buffer, "CEL")) {
			this->ReadCell(ins);
		}
		else if (!strcmp(buffer, "Structure")) {
			this->ReadStructure(ins);
		}
		else if (!strcmp(buffer, ENDSTR)) {
			flag = true;
		}
	}
}

void Geometry::ReadCell(std::istream& ins)
{
	char oneChar;
	std::string line;

	ins >> CelID;
	ins >> oneChar;

	CEL.clear();
	std::vector<std::vector<Node>> layer;

	while (std::getline(ins, line))
	{
		if (line.find(ENDSTR) != std::string::npos)
			break;

		if (line.empty()) {
			if (!layer.empty()) {
				CEL.push_back(layer);
				layer.clear();
			}
			continue;
		}

		std::istringstream ss(line);
		std::string token;
		std::vector<Node> row;

		while (ss >> token)
		{
			try {
				int region = std::stoi(token);
				Node node(region, SOLVER->nDIM);
				row.push_back(node);
			}
			catch (const std::invalid_argument&) {
				std::cerr << "Warning: invalid token in CEL block: " << token << std::endl;
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

void Geometry::ReadStructure(std::istream& ins)
{
	char oneChar;
	std::string line;

	ins >> oneChar;
	STRUCTURE.clear();
	std::vector<std::vector<Cell>> layer;

	while (std::getline(ins, line))
	{
		if (line.find(ENDSTR) != std::string::npos)
			break;

		if (line.empty()) {
			if (!layer.empty()) {
				STRUCTURE.push_back(layer);
				layer.clear();
			}
			continue;
		}

		std::istringstream ss(line);
		std::string token;
		std::vector<Cell> row;

		while (ss >> token)
		{
			try {
				int cellIndex = std::stoi(token);

				if (CELS.find(cellIndex) != CELS.end()) {
					row.push_back(CELS[cellIndex]);
				}
				else {
					std::cerr << "Warning: CEL index not found: " << cellIndex << std::endl;
				}
			}
			catch (const std::invalid_argument&) {
				std::cerr << "Warning: invalid token in Structure block: " << token << std::endl;
			}
		}

		if (!row.empty())
			layer.push_back(row);
	}

	if (!layer.empty()) {
		STRUCTURE.push_back(layer);
	}
}

void Geometry::PrintStructure() const
{
	std::cout << "\n[STRUCTURE]\n";

	for (size_t z = 0; z < STRUCTURE.size(); ++z)
	{
		std::cout << "Layer z = " << z << "\n";
		for (size_t y = 0; y < STRUCTURE[z].size(); ++y)
		{
			for (size_t x = 0; x < STRUCTURE[z][y].size(); ++x)
			{
				const Cell& cell = STRUCTURE[z][y][x];
				std::cout << " Cell (" << x << ", " << y << ", " << z << "):\n";
				for (size_t k = 0; k < cell.size(); ++k) {
					for (size_t i = 0; i < cell[k].size(); ++i) {
						for (size_t j = 0; j < cell[k][i].size(); ++j) {
							std::cout << std::setw(3) << cell[k][i][j].getREGION() << " ";
						}
						std::cout << "\n";
					}
					std::cout << "---\n";
				}
				std::cout << "\n";
			}
		}
	}
}
