#include "defines.h"

#include "input.h"
#include "reduce.h"
#include "optimize.h"
#include "reagent.h"

#include <gurobi_c++.h>

using namespace std;
using namespace Eigen;

#define DEBUG 1

int main()
{
	init_states();
	//print_oxidation_states();
	//init_charges();

	std::cout.precision(4); 
	std::cout.setf(ios::fixed);

	auto in = input();
	auto data = in.first;
	auto lhs = in.second;
	//std::vector<std::string> tn = {
	//	"Li", "Al", "O"
	//};
	//std::vector<int> tc = {
	//	1, 2, 4
	//};
	//auto rhs = to_reagent(tn, tc);
	//validate_reagents(lhs, rhs);

	int coeff, eq;

	eq = 2; coeff = data.size();
	
	MatrixXd A(eq, coeff);
	VectorXd B(eq);

	for (int i = 0; i < coeff; i++) {
		A(0, i) = 1;
		A(1, i) = data[i].value;
	}
	B << 1, 0;

	// TODO: FIX CHARGE SELECTION TO WORK WITH MULTIPLE CHARGES
	VectorXd S = lu_reduce(A, B);

#ifdef PERCENTAGE
	std::cout.precision(6);
	std::cout << "Proportions of elements:" << std::endl;
	for (int i = 0; i < data.size(); i++) {
		std::cout << data[i].name << ":" << setw(9 - data[i].name.size()) << S(i) * 100 << "%" << std::endl;
		hin.second[i] *= S(i);
	}
#endif

	std::vector<std::string> names;
	std::vector<std::vector<int>> samples;
	for (auto i : data) {
		names.push_back(i.name);
	}

	const auto coefficients = populate_coefficients(names);

	NLINE

	// the way with vector of vector of coefficients
	optimize(lhs, names, coefficients, 'I', {0, 10});

	return 0;
}