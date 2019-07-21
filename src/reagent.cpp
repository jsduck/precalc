#include "reagent.h"

Reagent::Reagent() = default;

Reagent::Reagent(std::vector<std::string> name, std::vector<int> atom_count) {
	for (auto i = 0; i < name.size(); i++) {
		_self.emplace_back(name[i], atom_count[i]);
	}
}

const std::vector<Element>& Reagent::operator()() const {
	return _self;
}

const Element& Reagent::operator()(int idx) const {
	return _self[idx];
}

void Reagent::insert(const Element& element) {
	_self.push_back(element);
}

void Reagent::remove(int idx) {
	_self.erase(_self.begin() + idx);
}
