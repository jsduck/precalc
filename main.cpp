#include "defines.h"

#include "input.h"
#include "reduce.h"
#include "optimize.h"
#include "modeldb.h"

#include <gurobi_c++.h>

using namespace std;
using namespace Eigen;

//#define DEBUG 1

int main()
{
	StateDB sdb;
	ModelDB mdb;
	//print_oxidation_states();
	//init_charges();

	std::cout.precision(4); 
	std::cout.setf(ios::fixed);

	const Input in(INPUT);
	const auto els = in().els;
	const auto rdb = in().rdb;
	//std::vector<std::string> tn = {
	//	"Li", "Al", "O"
	//};
	//std::vector<int> tc = {
	//	1, 2, 4
	//};
	//auto rhs = to_reagent(tn, tc);
	//validate_reagents(lhs, rhs);

	//int coeff, eq;

	//eq = 2; coeff = data.size();
	
	//MatrixXd A(eq, coeff);
	//VectorXd B(eq);

	//for (int i = 0; i < coeff; i++) {
	//	A(0, i) = 1;
	//	A(1, i) = data[i].value;
	//}
	//B << 1, 0;

	// TODO: FIX CHARGE SELECTION TO WORK WITH MULTIPLE CHARGES
	//VectorXd S = lu_reduce(A, B);

	//std::vector<std::vector<int>> samples;
	const auto coefficients = sdb.populate(els);

	NLINE

	// the way with vector of vector of coefficients
	///optimize(rdb, els, coefficients, 'I', {0, 10});


	for (const auto c : coefficients) {
		// 1: vector<vector<pair<string, double>>> lhs
		// 2: vector<vector<pair<string, double>>> rhs
		// 3: vector<char> sense
		// 4: type
		// 5: range
		for (int i = 0; i < c.size(); ++i) {
			
		}

		std::vector<std::vector<std::pair<std::string, double>>> lhs, rhs;
		std::vector<char> sense = { '=', '>' };

		std::vector<std::pair<std::string, double>> temp1, temp0;
		for (int i = 0; i < c.size(); ++i) {
			temp1.push_back({ els[i], c[i] });
			temp0.push_back({ els[i], 1 });
		}
		lhs.push_back(temp1);
		lhs.push_back(temp0);
		
		rhs.push_back({ {"", 0} });
		rhs.push_back({ {"", 1} });

		mdb.insert(lhs, rhs, sense, 'I', { 0, 10 });
	};

	mdb.solve(rdb);

	return 0;
}