#pragma once

#include <iomanip>
#include <iostream>

namespace utils {
	inline void ncout(double n, std::ostream& out = std::cout, int spacing = 8, std::string extra = "", bool nline = false) {
		int s = 1;
		int ni = n;
		
		if (ni < 0) ++s;

		do {
			// reduce n;
			ni /= 10;
			++s;
		} while (ni != 0);

		//std::cout << "@" << s << "@";

		out << std::setw(spacing - s) << " ";

		out << n << " " << extra;
		if (nline)
			out << std::endl;
	}

	inline int ndigits(int n) {
		int s = 0;
		do {
			++s;
			n /= 10;
		} while (n != 0);

		return s;
	}

	inline std::vector<std::vector<double>> power_set(std::vector<double> in) {
		std::vector<std::vector<double>> nset;
		const int pow_set_size = pow(2, in.size());
		for (int counter = 0; counter < pow_set_size; ++counter) {
			std::vector<double> vset;
			for (int j = 0; j < in.size(); j++) {
				if (counter & (1 << j))
					vset.push_back(in[j]);
			}
			nset.push_back(vset);
		}

		return nset;
	}
}
