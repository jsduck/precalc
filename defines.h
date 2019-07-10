#pragma once
// std
#include <string>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
// boost
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
// eigen
#include <Eigen/Dense>


#define NLINE std::cout << std::endl;

struct coefficient
{
	coefficient() {
		name = "nul";
		value = 0.0;
	}
	coefficient(std::string n, double c) {
		name = n;
		value = c;
	}

	std::string name;
	double value;
};

struct reagent
{
	std::vector<std::pair<std::string, int>> element;
};

std::multimap<std::string, int> oxidation_state;
std::set<std::string> elements;

inline void init_states() {
	//oxidation_state.emplace("Li", +1);
	//oxidation_state.emplace("Al", +1);
	//oxidation_state.emplace("Al", +2);
	//oxidation_state.emplace("Al", +3);
	//oxidation_state.emplace("O", -2);
	//oxidation_state.emplace("O", -1);
	//oxidation_state.emplace("O", +1);
	//oxidation_state.emplace("O", +2);

	std::fstream fin("data/oxidation_states.txt");

	std::string line; int state = 0;
	boost::char_separator<char> sep(" ");
	while(std::getline(fin, line)) {
		boost::tokenizer<boost::char_separator<char>> tokens(line, sep);

		auto it = tokens.begin();
		std::string element = *it; ++it;
		elements.emplace(element);

		while (it != tokens.end()) {
			std::istringstream(*it) >> state;
			oxidation_state.emplace(element, state);

			++it;
		}
	}
}

inline int validate_oxidation_state_sign(std::vector<int> oxidation_state) {
	int ap = 0, an = 0;
	for (auto os : oxidation_state) {
		if (os >= 0)
			ap++;
		if (os <= 0)
			an++;
	}
	if (ap == oxidation_state.size())
		return 1;
	if (an == oxidation_state.size())
		return -1;

	return 0;
}

inline void print_oxidation_states() {
	std::cout << "---------------- PRINTING OXIDATION STATES ----------------" << std::endl;
	for(auto ei : elements) {
		std::cout << ei;
		std::cout << std::setw(4 - ei.size()) << " ";
		std::cout << ">> ";
		auto range = oxidation_state.equal_range(ei);
		for (auto it = range.first; it != range.second; ++it) {
			if (it->second >= 0) std::cout << "+";
			std::cout << it->second << " ";
		}
		NLINE
	}
	std::cout << "--------------------------- END --------------------------" << std::endl;
	NLINE
}