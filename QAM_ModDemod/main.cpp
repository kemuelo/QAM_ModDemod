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
	vector<complex<double>> star;
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

		if (M == 4) {}
	}
};


int main() {  };
