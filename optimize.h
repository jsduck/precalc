#pragma once

#include "defines.h"

#include <gurobi_c++.h>
#include <unordered_map>
#include "reagent.h"

using namespace std;

inline int compare_stoichs(std::vector<int> s1, std::vector<int> s2) {
	int l = 0, g = 0, e = 0;
	for (int i = 0; i < s1.size(); i++) {
		if (s1[i] < s2[i])
			l++;
		else if (s1[i] > s2[i])
			g++;
		else if (s1[i] == s2[i])
			e++;
	}
	if (l == s1.size())
		return -1;
	if (g == s1.size())
		return 1;
	if (e == s1.size())
		return 0;

	return INT32_MAX;
}

// return list of all stoichiometries if possible otherwise 0
inline std::vector<std::vector<int>> optimize(GRBEnv env, std::vector<std::string> var_name, std::vector<int> oxidation_state, char type = 'I', std::pair<int, int> range = {0, 20}) {
	try {
		std::vector<std::vector<int>> temp;
		
		GRBModel model = GRBModel(env);

		model.set(GRB_IntParam_OutputFlag, 0);

		GRBLinExpr expr = 0;
		std::vector<GRBVar> vars;
		std::vector<double> coeffs;
		
		for (int i = 0; i < var_name.size(); i++) {
			vars.push_back(model.addVar(range.first, range.second, 0, type, var_name[i]));
			coeffs.push_back(static_cast<double>(oxidation_state[i]));
		}
		expr.addTerms(&coeffs[0], &vars[0], vars.size());

		model.addConstr(expr, GRB_EQUAL, 0);

		GRBLinExpr sum = 0;
		for (auto v  : vars) {
			sum += v;
		}

		model.addConstr(sum, GRB_GREATER_EQUAL, 1);

		model.set(GRB_IntParam_InfUnbdInfo, 1);
		model.set(GRB_IntParam_PoolSearchMode, 2);
		model.set(GRB_IntParam_PoolSolutions, 100);

		model.optimize();


		int status = model.get(GRB_IntAttr_Status);
		if (status == GRB_UNBOUNDED) {
			//cout << "The model cannot be solved "
			//	<< "because it is unbounded" << endl;
		}
		else if (status == GRB_OPTIMAL) {
			//cout << "The optimal objective is " <<
			//	model.get(GRB_DoubleAttr_ObjVal) << endl;
			std::vector<int> iv;
			for (auto var : vars) {
				//std::cout << var.get(GRB_StringAttr_VarName) << " "
				//	<< var.get(GRB_DoubleAttr_X) << endl;

				int attr = var.get(GRB_DoubleAttr_X);
				iv.push_back(attr);
				
				//temp.push_back({ var.get(GRB_StringAttr_VarName), var.get(GRB_DoubleAttr_X) });
			}

			temp.push_back(iv);

			//std::cout << std::endl;
			auto solcount = model.get(GRB_IntAttr_SolCount);
			if (solcount > 1)
			for (int i = 1; i < solcount; ++i) {
				std::vector<int> niv;
				model.set(GRB_IntParam_SolutionNumber, i);
				auto nv = model.getVars();
				double objn = 0;

				double sum = 0.0;
				for (int j = 0; j < vars.size(); ++j) {
					sum += nv[j].get(GRB_DoubleAttr_Xn);
					int nattr = nv[j].get(GRB_DoubleAttr_Xn);
					niv.push_back(nattr);
				}
				for (int j = 0; j < vars.size(); ++j) {
					//std::cout << nv[j].get(GRB_StringAttr_VarName) << " "
					//	<< nv[j].get(GRB_DoubleAttr_Xn) << " ";
					objn += (nv[j].get(GRB_DoubleAttr_Obj) * nv[j].get(GRB_DoubleAttr_Xn));
				}
				//std::cout << std::endl;
				for (int j = 0; j < vars.size(); ++j) {
					//std::cout << nv[j].get(GRB_StringAttr_VarName) << " "
					//	<< nv[j].get(GRB_DoubleAttr_Xn) / sum << " ";
				}
				//std::cout << std::endl;
				//std::cout << "Solution " << i << " has objective: " << objn;

				bool search = false;
				for (int q = 0; q < temp.size(); q++) {
					if (compare_stoichs(temp[q], niv) == 0)
						search = true;
				}

				if (!search) {
					temp.emplace_back(niv);
				}

				//temp.push_back(niv);
				//std::cout << temp.size() << " ";
			}

			model.set(GRB_IntParam_OutputFlag, 1);
			model.terminate();

			//std::cout << "Optimal solution space: " << solcount << std::endl;
			//std::cout << "Returned soltuion space: " << temp.size() << std::endl;

		}
		else {
			//std::cout << "The model is infeasible; computing IIS" << endl << endl;

			//model.computeIIS();
			//cout << "The following constraints cannot be satisfied" << endl;
			//auto c = model.getConstrs();
			//for (int i = 0; i < model.get(GRB_IntAttr_NumConstrs); ++i)
			//{
			//	if (c[i].get(GRB_IntAttr_IISConstr) == 1)
			//	{
			//		cout << c[i].get(GRB_StringAttr_ConstrName) << endl;
			//	}
			//}
		}

		return temp;
	}
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...)
	{
		cout << "Exception during optimization" << endl;
	}

	return {};
}

inline void optimize(std::vector<reagent> lhs, std::vector<std::string> var_name, std::vector<std::vector<int>> oxidation_state, char type = 'I', std::pair<int, int> range = { 0, 20 }) {
	GRBEnv env;
	auto rq = quantify_reagents(lhs, var_name);
	for (auto os : oxidation_state) {
		const int valid = validate_oxidation_state_sign(os);
		if (valid == 0) {
			std::cout << "Optimising state: ";
			for (auto e : os) {
				std::cout << e << " ";
			}
			//std::cout << " >> " << std::endl;
			NLINE
			auto op = optimize(env, var_name, os, type, range);
			if (!op.empty()) {
				int si = 1;
				for (auto vi : op) {
					std::cout << "> Solution " << si;
					if (si >= 10 && si < 100) std::cout << setw(2) << " ";
					else if (si >= 100) std::cout << setw(1) << " ";
					else if (si < 10) std::cout << setw(3) << " ";
					int spacing_i = 0;
					for (auto vei : vi) {
						std::cout << vei << " ";
						if (vei < 10)
							spacing_i += 2;
						else if (vei >= 10 && vei < 100)
							spacing_i += 3;
					}

					auto rhs = to_reagent(var_name, vi);
					auto res = validate_reagents(lhs, rhs);
					std::cout << setw(10-spacing_i) << " ";
					if (res == 0) {
						std::cout << "validated";
					} else if (res < 0) {
						std::cout << "not validated, not enough " << var_name[std::abs(res+1)];
						std::cout << setw(3 - var_name[std::abs(res + 1)].size()) << " ";
						std::cout << rq[std::abs(res + 1)] << "/" << vi[std::abs(res + 1)] << " atoms needed";
					} else if (res > 0) {
						std::cout << "not validated, missing " << var_name[std::abs(res-1)];
					}
					NLINE
					++si;
				}
			}
		} else {
			std::cout << "Discarding ";
			if (valid == 1) {
				std::cout << "all positive state: ";
				for (auto e : os) {
					std::cout << e << " ";
				}
				std::cout << std::endl;
			}
			if (valid == -1) {
				std::cout << "all negative state: ";
				for (auto e : os) {
					std::cout << e << " ";
				}
				std::cout << std::endl;
			}
		}
	}
}