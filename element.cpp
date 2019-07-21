#include "element.h"

Element::Element() = default;

Element::Element(const ElementData& d) {
	_self.n = d.n;
	_self.q = d.q;
	_self.s = d.s;
}

Element::Element(std::string s, int i, int e) {
	_self.n = s;
	_self.q = i;
	_self.s = e;
}

const ElementData& Element::operator()() const {
	return _self;
}
