#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <complex>
#include <fstream>

using namespace std;

class QAM_Mod {
private:
	int M;
	int num_bits;
	double norm_fact;
	vector <complex<double>> star;
	void gen_star() {
		int k = sqrt(M);
		for (int i = 0; i < M; i++) {
			int i_val = i / k;
			int q_val = i % k;

			double re_val = 2 * i_val - k + 1;
			double im_val = 2 * q_val - k + 1;

			star.push_back(complex<double>(re_val / norm_fact, im_val / norm_fact));
		}
	}
public:
	QAM_Mod(int M_num) : M(M_num) {
		num_bits = log2(M);

		if (M == 4) { norm_fact = sqrt(2); }
		else if (M == 16) { norm_fact = sqrt(10); }
		else if (M == 64) { norm_fact = sqrt(42); }
		else norm_fact = 1;

		gen_star();
	}
	vector <complex<double>> modulate(const vector <int>& bits) {
		vector <complex<double>> symb;
		for (int i = 0; i < bits.size(); i += num_bits) {
			int symb_indx = 0;
			for (int j = 0; i < num_bits; j++) {
				if (i + j < bits.size()) { symb_indx = (symb_indx << 1) | bits[i + j]; }
			}
			symb.push_back(star[symb_indx]);
		}
		return symb;
	}
	vector <complex<double>> get_star() {
		return star;
	}
};

class AWGN_channel {
public:
	vector <complex<double>> noise(const vector <complex<double>>& symb, double variance) {
		vector <complex<double>> noise_symb;
		random_device rd;
		mt19937 gen(rd());
		normal_distribution<double> dist(0.0, sqrt(variance / 2.0));
		for (const auto& sym : symb) { noise_symb.push_back(sym + complex<double>(dist(gen), dist(gen))); }
		return noise_symb;
	}
};


int main() { 
	const int num_bit = 10000;
	vector <int> M_val = { 4,16,64 };
	vector <double> variance;

	vector <int> tx_bit(num_bit);
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(0, 1);
	for (int i = 0; i < num_bit; ++i) { tx_bit[i] = dist(gen); }

};
