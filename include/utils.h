#pragma once

#include <iomanip>
#include <iostream>

namespace utils {
	inline void ncout(double n, std::ostream& out = std::cout, int spacing = 8, bool nline = false) {
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

		out << n;
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
}
