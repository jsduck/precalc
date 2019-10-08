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

	inline int fact(int n)
	{
		int result = 1;
		while (n > 1) {
			result *= n--;
		}
		return result;
	}

	template <typename T>
	void permutation(std::vector<T> v)
	{
		std::sort(v.begin(), v.end());
		do {
			std::copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
			std::cout << std::endl;
		} while (std::next_permutation(v.begin(), v.end()));
	}

	template <typename T>
	std::vector<std::vector<T>> combination_k(const std::vector<T>& v, std::size_t count)
	{
		std::vector<std::vector<T>> cnk;
		assert(count <= v.size());
		std::vector<bool> bitset(v.size() - count, 0);
		bitset.resize(v.size(), 1);

		do {
			std::vector<T> vs;
			for (std::size_t i = 0; i != v.size(); ++i) {
				if (bitset[i]) {
					vs.push_back(v[i]);
				}
			}
			cnk.push_back(vs);
		} while (std::next_permutation(bitset.begin(), bitset.end()));

		return cnk;
	}

	inline bool increase(std::vector<bool>& bs)
	{
		for (std::size_t i = 0; i != bs.size(); ++i) {
			bs[i] = !bs[i];
			if (bs[i] == true) {
				return true;
			}
		}
		return false; // overflow
	}

	template <typename T>
	void PowerSet(const std::vector<T>& v)
	{
		std::vector<bool> bitset(v.size());

		do {
			for (std::size_t i = 0; i != v.size(); ++i) {
				if (bitset[i]) {
					std::cout << v[i] << " ";
				}
			}
			std::cout << std::endl;
		} while (increase(bitset));
	}
}
