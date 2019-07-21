#pragma once

#include <string>

struct ElementData
{
	std::string n;
	int q, s;
};

class Element
{
	ElementData _self;
public:

	Element();
	Element(const ElementData& d);
	Element(std::string s, int i, int e = 0);

	const ElementData& operator()() const;
};
