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
