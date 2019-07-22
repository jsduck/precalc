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
                     std::pair<double, double> range, Reagent re) {
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
	// misc stuff
	md.reagent = re;
	md.label = re.str();
	//now create expressions based on oxidation state
	_models.push_back(new Model(_self, md));
}

void ModelDB::erase(int idx) {
	_models.erase(_models.begin() + idx);
}

std::vector<std::vector<std::vector<double>>> ModelDB::solve(ReagentDB rdb, std::ostream& out, bool validate)
{
	std::vector<std::vector<std::vector<double>>> tmodels;
	tmodels.reserve(_models.size());

	for (auto &m : _models) {
		// TODO: do somethign with this
		if (!m->validate_state_sign_i())
			continue;

		std::vector<std::string> var_name;
		for (auto& vm : m->_data.vars)
			var_name.push_back(vm.first);
		// only works if var_name == element_name
		auto rq = rdb.quanitfy(var_name);

		auto sv = m->solve();

		if (sv.empty())
			continue;

		out << "Optimising state: ";
		out << m->_data.label;
		out << std::endl;

		for (auto l = 0; l < m->_data.lhs.size(); ++l) {
			for (auto r : m->_data.lhs[l]) {
				//std::cout << r.second << " ";
				utils::ncout(r.second, out);
			}
			//std::cout << " | ";
			out << std::setw(4) << " ";
			out << m->_data.sense[l];
			//std::cout << std::setw(4) << " ";

			for (auto r : m->_data.rhs[l]) {
				//std::cout << r.second << " ";
				utils::ncout(r.second, out);
			}

			out << std::endl;
		}
		//std::cout << " >> " << std::endl;
		out << std::endl;

		std::vector<std::vector<double>> tsols;
		out << "Num Solutions: " << sv.size() << std::endl;
		int si = 1;
		tsols.reserve(sv.size());
		for (auto vi : sv) {
			out << "> Solution " << si;
			out << std::setw(5 - utils::ndigits(si)) << " ";
			int spacing_i = 0;
			std::vector<double> tvals;
			for (auto vei : vi) {
				//std::cout << vei << " ";
				if (vei == 0.0)
					vei = 0;
				utils::ncout(vei, out, 4);
				//std::cout << " ";
				if (vei < 10)
					spacing_i += 2;
				else if (vei >= 10 && vei < 100)
					spacing_i += 3;

				tvals.push_back(vei);
			}

			if (validate) {
				auto rhs = Reagent(var_name, vi);
				auto res = rdb.validate(rhs);//validate_reagents(lhs, rhs);
				out << std::setw(10 - spacing_i) << " ";
				if (res == 0) {
					out << "validated";
				}
				else if (res < 0) {
					out << "not validated, not enough " << var_name[std::abs(res + 1)];
					out << std::setw(3 - var_name[std::abs(res + 1)].size()) << " ";
					out << rq[std::abs(res + 1)] << "/" << vi[std::abs(res + 1)] << " atoms needed";
				}
				else if (res > 0) {
					out << "not validated, missing " << var_name[std::abs(res - 1)];
				}
			}

			// TODO: redo this, move mass into reagent itself at creation
			if (property.use_edb) {
				if (!m->_data.label.empty()) {
					double tm = 0;
					out << std::endl;
					out.precision(4);
					out << std::fixed;
					out << "> Mass (g/mol): ";
					for (int i = 0; i < vi.size(); ++i) {
						double mass = rdb()[i].mass() * vi[i];
						utils::ncout(mass, out, 4);
						tm += mass;
					}
				
					out << std::setw(2) << " ";
					out << "=";
					out << std::setw(2) << " ";

					utils::ncout(tm, out, 4);
				}

				
			}

			out << std::endl;
			++si;
			tsols.push_back(tvals);
		}

		out << std::endl;
		out << std::endl;
		tmodels.push_back(tsols);
	}


	return tmodels;
}
