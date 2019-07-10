#pragma once

#include "defines.h"

inline reagent to_reagent(std::vector<std::string> name, std::vector<int> atom_count) {
	reagent r;

	for (int i = 0; i < name.size(); i++) {
		r.element.push_back(std::make_pair(name[i], atom_count[i]));
	}

	return r;
}

inline void trim_reagents(std::vector<reagent> vr, reagent compound) {
	auto it = vr.begin();
	while (it != vr.end()) {
		bool useful = false;
		for (auto re : it->element) {
			for (auto e : compound.element) {
				if (e.first == re.first)
					useful = true;
			}
		}
		if (!useful) {
			it = vr.erase(it);
		}
		else {
			++it;
		}
	}
}

inline std::vector<int> quantify_reagents(std::vector<reagent> vr, std::vector<std::string> names = {}) {
	std::vector<int> temp;
	if (!names.empty())
		temp.reserve(names.size());

	for (auto n : names) {
		int atom_count = 0;
		for (auto r : vr) {
			for (auto ri : r.element) {
				if (ri.first == n)
					atom_count += ri.second;
			}
		}
		temp.push_back(atom_count);
	}

	return temp;
}

// vr: vector of available reagents(vector<reagent>), compound: desired composition(reagent)
// return: +n when n is missing
// return: -n when n is not enough
// return:  0 when everything is fine
inline int validate_reagents(std::vector<reagent> vr, reagent compound) {
	int index = 0;
	for (auto e : compound.element) {
		bool possible_element = false;
		bool possible_atomcnt = false;
		int ec = 0;
		for (auto ri : vr) {
			for (auto re : ri.element) {
				if (e.first == re.first) {
					possible_element = true;
					ec += re.second;
				}
			}
		}
		if (ec >= e.second) {
			possible_atomcnt = true;
		}
		if (!possible_element || !possible_atomcnt) {
			if (!possible_element) {
				//std::cout << "Reconstruction impossible, missing: " << e.first << std::endl;
				return 1 + index;
			}
			if (!possible_atomcnt && possible_element) {
				//std::cout << "Reconstruction impossible, not enough " << e.first << "; Present " << ec << " out of " << e.second << " atoms" << std::endl;
				return -1 - index;
			}
			//NLINE
			
			//return false;
		}
		++index;
	}

	//std::cout << "Reconstruction possible, all elements present" << std::endl;
	//NLINE
	return 0;
}
