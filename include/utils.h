#pragma once

#include <iomanip>
#include <iostream>

namespace utils {
	inline void ncout(double n, bool nline = false, int spacing = 8) {
		int s = 1;
		int ni = n;
		
		if (ni < 0) ++s;

		do {
			// reduce n;
			ni /= 10;
			++s;
		} while (ni != 0);

		//std::cout << "@" << s << "@";

		std::cout << std::setw(spacing - s) << " ";

		std::cout << n;
		if (nline)
			std::cout << std::endl;
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
