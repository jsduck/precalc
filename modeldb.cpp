#include "modeldb.h"
#include "utils.h"

ModelDB::ModelDB() = default;

int ModelDB::validate_state_sign_i(std::vector<int> state) {
	int ap = 0, an = 0;
	for (auto i : state) {
		if (i >= 0) ++ap;
		if (i <= 0) ++an;
	}

	if (ap == state.size())
		return 1;
	if (an == state.size())
		return -1;
	
	return 0;
}

void ModelDB::insert(Model* m) {
	_models.push_back(m);
}

void ModelDB::insert(std::vector<std::vector<std::pair<std::string, double>>> lhs,
                     std::vector<std::vector<std::pair<std::string, double>>> rhs, std::vector<char> sense, char type,
                     std::pair<double, double> range) {
	//params setup
	ModelData md;
	// options phase
	md.options.type = type;
	md.options.var_range = range;
	// default_params
	md.params.push_back({"GRB_IntParam_OutputFlag", "0"});
	md.params.push_back({"GRB_IntParam_InfUnbdInfo", "1"});
	md.params.push_back({"GRB_IntParam_PoolSearchMode", "2"});
	md.params.push_back({"GRB_IntParam_PoolSolutions", "100"});
	//vars first from elements
	//md.terms = terms;
	md.lhs = lhs;
	md.rhs = rhs;
	md.sense = sense;
	//now create expressions based on oxidation state
	_models.push_back(new Model(_self, md));
}

void ModelDB::erase(int idx) {
	_models.erase(_models.begin() + idx);
}

int ModelDB::solve(ReagentDB rdb)
{
	for (auto &m : _models) {
		if (!m->validate_state_sign_i())
			continue;

		std::vector<std::string> var_name;
		for (auto& vm : m->_data.vars)
			var_name.push_back(vm.first);
		auto rq = rdb.quanitfy(var_name);

		auto sv = m->solve();

		if (sv.empty())
			continue;

		std::cout << "Optimising state: ";
		std::cout << std::endl;
		for (auto l = 0; l < m->_data.lhs.size(); ++l) {
			for (auto r : m->_data.lhs[l]) {
				//std::cout << r.second << " ";
				utils::ncout(r.second);
			}
			//std::cout << " | ";
			std::cout << std::setw(4) << " ";
			std::cout << m->_data.sense[l];
			//std::cout << std::setw(4) << " ";

			for (auto r : m->_data.rhs[l]) {
				//std::cout << r.second << " ";
				utils::ncout(r.second);
			}

			std::cout << std::endl;
		}
		//std::cout << " >> " << std::endl;
		NLINE

		std::cout << "Num Solutions: " << sv.size() << std::endl;
		int si = 1;
		for (auto vi : sv) {
			std::cout << "> Solution " << si;
			std::cout << std::setw(4 - utils::ndigits(si)) << " ";
			int spacing_i = 0;
			for (auto vei : vi) {
				std::cout << vei << " ";
				if (vei < 10)
					spacing_i += 2;
				else if (vei >= 10 && vei < 100)
					spacing_i += 3;
			}

			auto rhs = Reagent(var_name, vi);
			auto res = rdb.validate(rhs);//validate_reagents(lhs, rhs);
			std::cout << std::setw(10 - spacing_i) << " ";
			if (res == 0) {
				std::cout << "validated";
			}
			else if (res < 0) {
				std::cout << "not validated, not enough " << var_name[std::abs(res + 1)];
				std::cout << std::setw(3 - var_name[std::abs(res + 1)].size()) << " ";
				std::cout << rq[std::abs(res + 1)] << "/" << vi[std::abs(res + 1)] << " atoms needed";
			}
			else if (res > 0) {
				std::cout << "not validated, missing " << var_name[std::abs(res - 1)];
			}
			NLINE
				++si;
		}

		NLINE
		NLINE
	}


	return 0;
}
