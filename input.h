#pragma once

#include "defines.h"

#define INPUT "input.txt"

inline std::vector<std::string> get_input(std::string filename) {
	/*
	 * Amount of Elements	# 2
	 * Known Elements		# Na Cl
	 * Sampling Density		# 10
	 * Chemical Reageants	#
	 */

	std::vector<std::string> data;
	std::ifstream file(filename);

	std::string line;
	while (std::getline(file, line)) {
		data.push_back(line);
	}

	return data;
}

inline std::pair<std::vector<coefficient>, std::vector<reagent>> handle_input(std::vector<std::string> data) {
	std::vector<coefficient> elements;
	std::vector<reagent> reagents;

	boost::char_separator<char> sep(" ");
	boost::char_separator<char> sep2(":");
	boost::tokenizer<boost::char_separator<char>> tokens0(data[1], sep);

	// Retrieve elements
	for (const auto& t : tokens0) {
		elements.push_back({ t, {} });
	}

	boost::tokenizer<boost::char_separator<char>> tokens1(data[2], sep);
	for (const auto& t : tokens1) {
		boost::tokenizer<boost::char_separator<char>> mem(t, sep2);
		reagent reagent;
		for (const auto& m : mem) {
			int c = 0;
			auto r = m.find("#");
			if (r == std::string::npos) {
				reagent.element.push_back(std::make_pair(m, 1));
			}
			else {
				std::string n = m.substr(0, r);

				std::stringstream ss;
				ss << m.substr(r + 1);
				ss >> c;

				reagent.element.push_back(std::make_pair(n, c));
			}
		}
		reagents.push_back(reagent);
	}



	return std::make_pair(elements, reagents);
}

inline std::vector<std::vector<int>> populate_coefficients(std::vector<std::string> elements) {
	std::vector<int> charge_count;
	for (auto n : elements) {
		charge_count.push_back(oxidation_state.count(n));
	}

	int mc = 0;
	for (auto c : charge_count) {
		if (c > mc)
			mc = c;
	}

	int piv = 0;
	std::vector<int> charges;
	for (int i = 0; i < charge_count.size(); i++)
		charges.push_back(oxidation_state.find(elements[i])->second);

	std::vector<std::vector<int>> last;
	last.push_back(charges);
	for (int i = charge_count.size() - 1; i >= 0; i--) {
		int cc = charge_count[i];

		std::vector<int> range;

		auto r = oxidation_state.equal_range(elements[i]);
		for (auto it = r.first; it != r.second; ++it) {
			range.push_back(it->second);
		}

		std::vector<std::vector<int>> current = last;
		for (int j = 0; j < range.size(); j++) {
			for (int k = 0; k < last.size(); k++) {
				std::vector<int> nv = current[k];

				nv[i] = range[j];

				bool s = false;
				for (int o = 0; o < last.size(); o++) {
					int sc = 0;
					for (int m = 0; m < current[o].size(); m++) {
						if (nv[m] == current[o][m])
							sc++;
					}

					if (sc == current[o].size()) {
						s = true;
					}
				}

				if (!s) {
					current.push_back(nv);
				}
			}
		}

		last = current;
		//std::cout << std::endl;
	}

	return last;
}

inline std::pair<std::vector<coefficient>, std::vector<reagent>> input() {
	return handle_input(get_input(INPUT));
}