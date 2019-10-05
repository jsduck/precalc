#include "defines.h"

#include "input.h"
#include "modeldb.h"
#include "elementdb.h"
#include "utils.h"

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;


//#define DEBUG 1

int method1()
{
	const ElementDB edb;
	ElementDB::init();

	StateDB sdb;
	ModelDB mdb(edb);

	std::cout.precision(4); 
	std::cout.setf(ios::fixed);

	const Input in(INPUT);
	const auto els = in().els;
	const auto amounts = in().amounts;
	const auto rdb = in().rdb;

	const auto coefficients = sdb.populate(els);


	std::stringbuf nbuf;
	std::ostream log(&nbuf);
	std::ofstream g("out.txt");


	std::cout << std::endl;

	/*
	 * First parse of mdb, generates solutions based on possible element states
	 * 1: ensures charge parity, AX + BY = 0, where X and Y are states and A and B are quantities of elements
	 * 2: ensures at least one element is present, A + B >= 1 where A and B are quantities of elements
	 * returns a 3D array containing:
	 * > 3rd layer contains all models solved
	 * > 2nd layer contains all solutions to the current model
	 * > 1st layer contains all variables from the current solution
	 */
	for (const auto c : coefficients) {
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

		// var type is int as we are looking for quantities of elements (mols i think)
		//mdb.insert(lhs, rhs, sense, 'I', { 0, 10 });
	};

	std::stringbuf test;
	std::ostream hello(&test);
	//mdb.solve(rdb, log);

	//g << log.rdbuf();

	// TODO: maybe make solve clear data as well? ask mike
	// empty model data so mdb can be reused later
	//mdb.clear();

	//clear redundant info from cmd
	system("CLS");

	// TODO : take a loot and split this later
	/*
	const char var = 'A';
	std::vector<double> solution = { 1, 1, 1, 1 };
	std::vector<std::vector<std::pair<string, double>>> lhs, rhs;
	std::vector<char> sense;
			
	// result from previous solve as the new rhs
	for (int i = 0; i < solution.size(); ++i) {
		sense.push_back('=');
			
		rhs.push_back({ {"", solution[i]} });
	}

	const int nv = rdb().size();

	std::vector<std::string> vn;
	for (int i = 0; i < nv; ++i) {
		vn.push_back(std::string(1, var + i));
	}

	// create lhs from available reagents
	// TODO: needs combinations or something for multiple solutions but will probably just swithc to google-ot anyway
	for (int i = 0; i < els.size(); ++i) {
		std::vector<std::pair<string, double>> lhs_row;
		for (int j = 0; j < nv; ++j) {
			double val = 0;
			for (auto &k : rdb(j)()) {
				if (k().n == els[i]) {
					val = k().q;
					// TODO: think about this break
					break;
				}

			}

			lhs_row.push_back({ vn[j], val });
		}
		lhs.push_back(lhs_row);
	}

	// type is S/C as we want continous values
	// range of values is between 0 and 10 inclusive, ask mike about this
	mdb.insert(lhs, rhs, sense, 'C', { 0.00, 10 }, Reagent(els, solution));
	*/
	// combinations of reagents
	std::vector<double> inset;
	for (int i = 0; i < rdb().size(); ++i) {
		// TODO: do pretrimming here to save speed
		inset.push_back(i);
	}
	auto pset = utils::power_set(inset);

	const char var = 'A';
	std::vector<std::string> vn;


	for (int i = 0; i < inset.size(); ++i) {
		vn.push_back(std::string(1, var + i));
	}

	for (auto &ps : pset) {
		std::vector<std::vector<std::pair<string, double>>> lhs, rhs;
		std::vector<char> sense;

		// nr of cols
		
		for (int i = 0; i < els.size(); ++i) {
			sense.push_back('=');

			std::vector<std::pair<std::string, double>> lhs_row;
			for (int j = 0; j < ps.size(); ++j) {
				double val = 0;
				for (auto &k : rdb(static_cast<int>(ps[j]))()) {
					if (k().n == els[i]) {
						val = k().q;
						break;
					}

				}
				
				lhs_row.push_back({ vn[j], val });
			}

			lhs.push_back(lhs_row);
			rhs.push_back({ {"", amounts[i]} });
		}

		// per column
		std::vector<std::string> col_labels;
		for (int j = 0; j < ps.size(); ++j) {
			Reagent r = rdb(static_cast<int>(ps[j]));

			int non_zero = 0;
			for (int i = 0; i < els.size(); ++i) {
				if (lhs[i][j].second == 0)
					non_zero++;
			}

			// remove all zeero colums
			if (non_zero == els.size()) {
				for (int i = 0; i < els.size(); ++i) {
					try {
						if (lhs[i].size() > j)
							lhs[i].erase(lhs[i].begin() + j);
						else {
							lhs[i].erase(lhs[i].begin() + lhs[i].size() - 1);
						}
					} catch (...) {
						
					}
					
				}

				continue;
			}

			col_labels.push_back(r.str());
		}

		mdb.insert(lhs, rhs, sense, 'C', { 0.0005, 10.0 }, col_labels, Reagent(els, amounts));
	}

	//mdb.property.repeat = true;
	mdb.property.use_edb = true;

	mdb.solve(rdb, log);

	std::cout << "OUTPUT >> out.txt" << std::endl;

	g << log.rdbuf();

	std::cout << std::endl;
	std::cout << "DONE" << std::endl;

	return 0;
}

int method2() {
	const ElementDB edb;
	ElementDB::init();

	StateDB sdb;

	std::cout.precision(4);
	std::cout.setf(ios::fixed);

	const Input in(INPUT);
	const auto els = in().els;
	const auto amounts = in().amounts;
	const auto rdb = in().rdb;

	const auto coefficients = sdb.populate(els);


	std::cout.precision(3);
	std::cout.setf(ios::fixed);

	const int nv = rdb().size();

	const char var = 'A';
	std::vector<std::string> vn;
	for (int i = 0; i < nv; ++i) {
		vn.push_back(std::string(1, var + i));
	}

	//std::vector<double> solution = { 1, 1, 1, 1 };
	std::vector<std::vector<std::pair<string, double>>> lhs, rhs;

	// create lhs from available reagents
	// TODO: needs combinations or something for multiple solutions but will probably just swithc to google-ot anyway
	for (int i = 0; i < els.size(); ++i) {
		std::vector<std::pair<string, double>> lhs_row;
		for (int j = 0; j < nv; ++j) {
			double val = 0;
			for (auto &k : rdb(j)()) {
				if (k().n == els[i]) {
					val = k().q;
					// TODO: think about this break
					break;
				}

			}

			lhs_row.push_back({ vn[j], val });
		}
		lhs.push_back(lhs_row);
		rhs.push_back({ {"RHS", amounts[i]} });
	}

	//std::cout << "Input: ";
	//for (auto via : amounts) {
	//	std::cout << via << " ";
	//}
	//std::cout << endl;

	MatrixXd A(lhs.size(), lhs[0].size());
	VectorXd B(lhs.size());

	for (int i = 0; i < els.size(); ++i) {
		for (int j = 0; j < nv; ++j) {
			A(i, j) = lhs[i][j].second;
		}

		B(i) = amounts[i];
	}

	//for (int i = 0; i < nv; ++i) {
	//	A(els.size(), i) = 1;
	//	B(els.size()) = 1;
	//}

	// A << 2, 2, 0, 0,
	// 	0, 0, 2, 2,
	// 	0, 1, 0, 3;// ,
	// 	 //1, 1, 1, 1;
	// B << 1, 1, 1;// , 1;

	FullPivLU<MatrixXd> lu;

	//cout << "A is: " << endl << A << endl;
	//cout << "B is: " << endl << B << endl;

	lu.compute(A);
	auto K = lu.kernel();
	auto S = lu.solve(B);

	cout << "Rank: " << lu.rank() << endl;
	cout << "Solution:\n" << S << endl;
	cout << "Kernel is:\n" << K << endl;

	// sol of points is given by P = S + t * K where t is a real number
	std::vector<double> inset;
	for (double i = 0; i < K.cols(); ++i) {
		inset.push_back(i);
	}

	

	// each kernel colum = column in array
	// each line in array = solution
	// issue is size as it becomes num_sols^n = big
	// maybe do a map? no as we'd need to also add a hashing function, but it wouldnt be too costy? mhm

	// Method 1 - Array
	vector<vector<double>> coeffs;
	double factor = 0.05;


	auto pset = utils::power_set(inset);
	for (auto &ps : pset) {

	}

	system("CLS");

	auto P = S + K;
	int sols = 0;
	int lastt = -100;
	//for (auto kc = 0; kc < K.cols(); ++kc) {
		for (auto t = 0.00; t <= 1.00; t += 0.005) {
			for (auto v = 0.00; v <= 1.00; v += 0.005) {
				auto p = S + t * K.col(0) + v * K.col(1);

				bool ffs = false;
				for (int i = 0; i < lhs[0].size(); ++i) {
					if (p(i) < 0)
						ffs = true;
				}
				if (ffs == true)
					continue;

				double sum = 0;
				//cout << "P: ";
				for (int j = 0; j < lhs[0].size(); ++j) {
					cout << p(j) << " ";
					sum += rdb()[j].mass() * p(j);
				}
				//cout << "M(g/mol): " << sum;
				cout << endl;
				sols++;
				lastt = t;
			}
		}
	//}
	//cout << "Optimal solutions: " << sols << endl;
	//cout << "Last pivot: " << lastt << endl;

	return 0;
}


int main() {
	method2();
	//method1();

	return 0;
}