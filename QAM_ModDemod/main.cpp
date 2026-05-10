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
			for (int j = 0; j < num_bits; j++) {
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

class QAM_Demod {
private:
	int M;
	int num_bits;
	vector <complex<double>> star;

public:
	QAM_Demod(int M_num, const vector <complex<double>>& refer_star) : M(M_num), star(refer_star) {
		num_bits = log2(M);
	}
	vector <int> demod(const vector <complex<double>>& noise_symb) {
		vector <int> demod_bits;

		for (const auto& sym : noise_symb) {
			double min_dist = 1e9;
			int ideal_index = 0;

			for (int i = 0; i < M; ++i) {
				double dist = norm(sym - star[i]);
				if (dist < min_dist) {
					min_dist = dist;
					ideal_index = i;
				}
			}
			for (int j = num_bits - 1; j >= 0; --j) {
				demod_bits.push_back((ideal_index >> j) & 1);
			}
		}
		return demod_bits;
	}
};

int main() { 
	const int num_bit = 100000;
	vector <double> ber_qpsk, ber_qam16, ber_qam64;
	vector <double> variance;

	vector <int> tx_bit(num_bit);
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(0, 1);
	for (int i = 0; i < num_bit; ++i) { tx_bit[i] = dist(gen); }

	AWGN_channel channel;

	for (double v = 0.01; v <= 1.0; v += 0.05) {
		variance.push_back(v);

		for (int M : {4, 16, 64}) {
			QAM_Mod modulator(M);
			QAM_Demod demodulator(M, modulator.get_star());

			vector <complex<double>> tx_symb = modulator.modulate(tx_bit);
			vector <complex<double>> rx_symb = channel.noise(tx_symb, v);
			vector <int> rx_bit = demodulator.demod(rx_symb);

			int error_cnt = 0;
			int compare_len = min(tx_bit.size(), rx_bit.size());
			for (int i = 0; i < compare_len; ++i) {
				if (tx_bit[i] != rx_bit[i]) { error_cnt++; }
			}
			double ber = static_cast <double>(error_cnt) / compare_len;
			if (M == 4) {ber_qpsk.push_back(ber); }
			else if (M == 16) {ber_qam16.push_back(ber); }
			else if (M == 64) {ber_qam64.push_back(ber); }
		}
	}
};
