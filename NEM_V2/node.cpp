#include "node.h"
#include "solver.h"

Node::Node(int region, Solver* solver) {
	SOLVER = solver;
	REGION = region;
	int group = solver->nGROUP;
	int dim = solver->nDIM;

	WIDTH = new double[dim]();
	for (int i = 0; i < dim; i++)
		WIDTH[i] = solver->WIDTH[i];
	FLUX = new double[group] {};
	for (int i = 0; i < group; i++)
		FLUX[i] = 1.0;
	old_FLUX = new double[group] {};
	for (int i = 0; i < group; i++)
		old_FLUX[i] = 1.0;
	D_c = new double[group]();
	BETA = new double* [dim];
	for (int i = 0; i < dim; i++)
		BETA[i] = new double[group]();

	A = new double* [group];
	for (int i = 0; i < group; i++)
		A[i] = new double[group]();

	NEIGHBOR = new Node * *[dim];
	BOUNDARY = new BOUNDARY_TYPE * [dim];
	for (int i = 0; i < dim; ++i) {
		NEIGHBOR[i] = new Node * [2];
		for (int j = 0; j < 2; j++) { NEIGHBOR[i][j] = nullptr; }
		BOUNDARY[i] = new BOUNDARY_TYPE[2];
	}

	Q = new double** [dim];
	C = new double** [dim];
	OUT_CURRENT = new double** [dim];
	INCOM_CURRENT = new double** [dim];
	DL = new double** [dim];

	for (int i = 0; i < dim; ++i) {
		Q[i] = new double* [4];
		C[i] = new double* [5];
		OUT_CURRENT[i] = new double* [2];
		INCOM_CURRENT[i] = new double* [2];
		DL[i] = new double* [3];

		for (int j = 0; j < 4; ++j)
			Q[i][j] = new double[group]();

		for (int j = 0; j < 5; ++j)
			C[i][j] = new double[group]();

		for (int j = 0; j < 2; ++j) {
			OUT_CURRENT[i][j] = new double[group];
			INCOM_CURRENT[i][j] = new double[group];
			for (int k = 0; k < group; k++)
			{
				OUT_CURRENT[i][j][k] = 1.0;
				INCOM_CURRENT[i][j][k] = 1.0;
			}
		}
		for (int j = 0; j < 3; ++j)
		{
			DL[i][j] = new double[group];
			for (int k = 0; k < group; k++)
				DL[i][j][k] = 1.0;
		}
	}
	SRC1 = new double[group]();
	SRC2 = new double[group]();
	SRC = new double[group]();

	M1 = new double* [dim];
	M2 = new double* [dim];
	for (int i = 0; i < dim; i++) {
		M1[i] = new double[group]();
		M2[i] = new double[group]();
	}
	M3 = new double** [dim];
	M4 = new double** [dim];
	for (int i = 0; i < dim; i++) {
		M3[i] = new double* [group];
		M4[i] = new double* [group];
		for (int j = 0; j < group; j++) {
			M3[i][j] = new double[group]();
			M4[i][j] = new double[group]();
		}
	}
	MM = new double* [group];
	for (int i = 0; i < group; i++)
		MM[i] = new double[group]();
}

Node::~Node() {
	int dim = SOLVER ? SOLVER->nDIM : 0;
	int group = SOLVER ? SOLVER->nGROUP : 0;

	delete[] WIDTH;
	delete[] FLUX;
	delete[] BETA;

	if (NEIGHBOR) {
		for (int i = 0; i < dim; ++i)
			delete[] NEIGHBOR[i];
		delete[] NEIGHBOR;
	}
	if (A) {
		for (int i = 0; i < group; ++i)
			delete[] A[i];
		delete[] A;
	}

	if (BOUNDARY) {
		for (int i = 0; i < dim; ++i)
			delete[] BOUNDARY[i];
		delete[] BOUNDARY;
	}

	auto delete3D = [&](double***& arr, int outer, int inner) {
		if (!arr) return;
		for (int i = 0; i < outer; ++i) {
			for (int j = 0; j < inner; ++j) {
				delete[] arr[i][j];
			}
			delete[] arr[i];
		}
		delete[] arr;
		};

	delete3D(Q, dim, 4);
	delete3D(C, dim, 5);
	delete3D(OUT_CURRENT, dim, 2);
	delete3D(INCOM_CURRENT, dim, 2);
	delete3D(DL, dim, 3);
}

void Node::SetCrossSection(double* D, double* R, double* S, double* F, double* CHI) {
	int group = SOLVER->nGROUP;
	int dim = SOLVER->nDIM;
	double k = SOLVER->K_EFF;
	for (int i = 0; i < group; i++) {
		D_c[i] = D[i];
		SRC[i] = CHI[i] * ( F[0]*FLUX[0] + F[1]*FLUX[1]);
	}
		

	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < group; j++)
			BETA[i][j] = D[j] / WIDTH[i];
	}

	for (int i = 0; i < group; ++i) {
		for (int j = 0; j < group; ++j) {
			double removal = (i == j) ? R[i] : 0.0;
			double fission_term = (1.0 / k) * CHI[i] * F[j];
			double scatter = (i != j) ? S[i] : 0.0;

			A[i][j] = removal -fission_term - scatter;
		}
	}


	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < group; j++)
		{
			Q[i][0][j] = BETA[i][j] / (1 + 12 * BETA[i][j]);
			Q[i][1][j] = BETA[i][j] / (1 + 4 * BETA[i][j]);
			Q[i][2][j] = 8 * BETA[i][j] / ((1 + 12 * BETA[i][j]) * (1 + 4 * BETA[i][j]));
			Q[i][3][j] = (1 - 48 * BETA[i][j]* BETA[i][j]) / ((1 + 12 * BETA[i][j]) * (1 + 4 * BETA[i][j]));
		}
	}

	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < group; j++) {
			for (int k = 0; k < group; k++) {
				M3[i][j][k] = A[j][k] / 10.0;
				M4[i][j][k] = A[j][k] / 14.0;
				if (j == k) {
					M3[i][j][k] *= 6.0 * D[j] / (WIDTH[i] * WIDTH[i]);
					M4[i][j][k] *= 10.0 * D[j] / (WIDTH[i] * WIDTH[i]);
				}
			}
		}
	}
}

void Node::SetINCOM_CURRENT(int x, int y, int z) {
	int dim = SOLVER->nDIM;
	int group = SOLVER->nGROUP;
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < group; k++) {
				if (NEIGHBOR[i][j] != nullptr)
					INCOM_CURRENT[i][j][k] = NEIGHBOR[i][j]->OUT_CURRENT[i][j][k];
				else {
					if (BOUNDARY[i][j] == REFLECTIVE)
						INCOM_CURRENT[i][j][k] =OUT_CURRENT[i][j][k];
					else if (BOUNDARY[i][j] == VACUUM)
						INCOM_CURRENT[i][j][k] = 0.0;
				}
			}
		}
	}
}

void Node::SetBOUNDARY(int x, int y, int z) {
	int dim = SOLVER->nDIM;
	const auto& structure = SOLVER->GEOMETRY.GetStructure();

	for (int d = 0; d < dim; ++d) {
		for (int s = 0; s < 2; ++s) {
			int dx = 0, dy = 0, dz = 0;
			if (d == 0) dx = (s == 0) ? -1 : 1;
			if (d == 1) dy = (s == 0) ? -1 : 1;
			if (d == 2) dz = (s == 0) ? -1 : 1;

			int nx = x + dx;
			int ny = y + dy;
			int nz = z + dz;

			BOUNDARY_TYPE bc = REFLECTIVE;  // 기본값

			if (nz >= 0 && nz < static_cast<int>(structure.size())) {
				const auto& layer = structure[nz];
				if (ny >= 0 && ny < static_cast<int>(layer.size())) {
					const auto& row = layer[ny];
					if (nx >= 0 && nx < static_cast<int>(row.size())) {
						if (row[nx] == -1) {
							bc = VACUUM;
						}
					}
				}
			}

			BOUNDARY[d][s] = bc;
		}
	}
}

void Node::updateTransverseLeakage() {
	int dim = SOLVER->nDIM;
	int group = SOLVER->nGROUP;

	for (int u = 0; u < dim; u++)
	{
		for (int g = 0; g < group; g++)
		{
			DL[u][0][g] = 0.0;
			for (int i = 0; i < dim; i++) {
				int v = (u + i) % dim;
				double node_width = WIDTH[v];
				DL[u][0][g] += (getSurfaceNetCurrent(v, Right_side, g) - getSurfaceNetCurrent(v, Left_side, g)) / node_width;
			}
			Node* l_node = getNEIGHBOR(u, Left_side);
			Node* r_node = getNEIGHBOR(u, Right_side);
			double h_c = WIDTH[u];
			double beta_c = BETA[u][g];
			double D = D_c[g];
			double DL0_c = DL[u][0][g];
			double h_l = 1.0;
			double h_r = 1.0;
			double beta_l = 0.0;
			double beta_r = 0.0;
			double DL0_l = 0.0;
			double DL0_r = 0.0;
			if (l_node != nullptr)
			{
				h_l = l_node->WIDTH[u];
				beta_l = l_node->BETA[u][g];
				DL0_l = l_node->DL[u][0][g];
			}

			if (r_node != nullptr)
			{
				h_r = r_node->WIDTH[u];
				beta_r = r_node->BETA[u][g];
				DL0_r = r_node->DL[u][0][g];
			}
			double L_l = (DL0_l / h_l + DL0_c / h_c) / (beta_l + beta_c);
			double L_r = (DL0_c / h_c + DL0_r / h_r) / (beta_c + beta_r);

			DL[u][1][g] = D * (L_r - L_l) / 2.0;
			DL[u][2][g] = D * (L_r + L_l - 2.0 * DL0_c / D) / 2.0;
		}
	}
}

void Node::makeOneDimensionalFlux() {
	int dim = SOLVER->nDIM;
	int group = SOLVER->nGROUP;
	for (int u = 0; u < dim; u++) {
		for (int g = 0; g < group; g++) {
			double flux_l = getSurfaceFlux(u, Left_side, g);
			double flux_r = getSurfaceFlux(u, Right_side, g);
			C[u][0][g] = FLUX[g];
			C[u][1][g] = (flux_r - flux_l) / 2.0;
			C[u][2][g] = (flux_r + flux_l) / 2.0 - FLUX[g];
			SRC1[g] = DL[u][1][g];
			SRC2[g] = DL[u][2][g];
		}
		add_product(SRC1, M1[u], C[u][1], group);
		add_product(SRC2, M2[u], C[u][2], group);
		GaussianElimination(M3[u], C[u][3], SRC1, group);
		GaussianElimination(M4[u], C[u][4], SRC2, group);
	}
}

void Node::updateAverageFlux() {
	int dim = SOLVER->nDIM;
	int group = SOLVER->nGROUP;
	double keff = SOLVER->K_EFF;
	for (int i = 0; i < group; i++) {
		for (int j = 0; j < group; j++)
			MM[i][j] = A[i][j];
		SRC[i] = 0.0;
	}

	for (int u = 0; u < dim; u++)
		for (int g = 0; g < group; g++)
			MM[g][g] += 12.0 * Q[u][0][g] / WIDTH[u];

	for (int u = 0; u < dim; u++) {
		for (int g = 0; g < group; g++) {
			double j_in_l = INCOM_CURRENT[u][Left_side][g];
			double j_in_r = INCOM_CURRENT[u][Right_side][g];
			double Q4 = 1.0 - Q[u][2][g] - Q[u][3][g];
			SRC[g] += (2.0 * Q[0][u][g] * C[u][4][g] + Q4 * (j_in_l + j_in_r)) / WIDTH[u];
		}
	}
	GaussianElimination(MM, FLUX, SRC, group);
}

void Node::updateOutgoingCurrent() {
	int dim = SOLVER->nDIM;
	int group = SOLVER->nGROUP;
	for (int u = 0; u < dim; u++) {
		for (int g = 0; g < group; g++) {
			double j_in_l = INCOM_CURRENT[u][Left_side][g];
			double j_in_r = INCOM_CURRENT[u][Right_side][g];
			OUT_CURRENT[u][Left_side][g] = Q[u][0][g] * (6 * FLUX[g] - C[u][4][g]) + Q[u][1][g] * C[u][3][g] - Q[u][2][g] * j_in_r + Q[u][3][g] * j_in_l;
			OUT_CURRENT[u][Right_side][g] = Q[u][0][g] * (6 * FLUX[g] - C[u][4][g]) - Q[u][1][g] * C[u][3][g] - Q[u][2][g] * j_in_l + Q[u][3][g] * j_in_r;
		}
	}
}

void Node::add_product(double* SRC, double* M, double* C, int group) {
	for (int i = 0; i < group; ++i) {
		M[i] = SRC[i];
		for (int j = 0; j < group; ++j) {
			M[i] += A[i][j] * C[j];
		}
	}
}

void Node::GaussianElimination(double** M_in, double* C, double* SRC, int group) {
	// LU decomposition without pivoting
	double** L = new double* [group];
	double** U = new double* [group];
	for (int i = 0; i < group; ++i) {
		L[i] = new double[group] {};
		U[i] = new double[group] {};
	}

	// Copy M_in into U for decomposition
	for (int i = 0; i < group; ++i) {
		for (int j = 0; j < group; ++j) {
			U[i][j] = M_in[i][j];
		}
	}

	for (int i = 0; i < group; ++i) {
		L[i][i] = 1.0;
		for (int j = i + 1; j < group; ++j) {
			double factor = U[j][i] / U[i][i];
			L[j][i] = factor;
			for (int k = i; k < group; ++k) {
				U[j][k] -= factor * U[i][k];
			}
		}
	}

	// Solve L*y = SRC using forward substitution
	double* y = new double[group];
	for (int i = 0; i < group; ++i) {
		y[i] = SRC[i];
		for (int j = 0; j < i; ++j) {
			y[i] -= L[i][j] * y[j];
		}
	}

	// Solve U*x = y using back substitution
	for (int i = group - 1; i >= 0; --i) {
		C[i] = y[i];
		for (int j = i + 1; j < group; ++j) {
			C[i] -= U[i][j] * C[j];
		}
		C[i] /= U[i][i];
	}

	// 메모리 해제
	for (int i = 0; i < group; ++i) {
		delete[] L[i];
		delete[] U[i];
	}
	delete[] L;
	delete[] U;
	delete[] y;
}

