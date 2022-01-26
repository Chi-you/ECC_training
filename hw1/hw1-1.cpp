#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

#define LEN 4000000 // the length of bit
#define EBN0 11

int random_message[LEN] = {0};
int after_mod[LEN] = {0};
double after_AWGN[LEN] = {0};
int after_demod[LEN] = {0};

int modulation(int random) 
{
	return (random == 1) ? -1 : 1;
}

int demod(double code) 
{
	return (code > 0) ? 0 : 1;
}

void print(int ebn0, double ber) 
{
	cout << ebn0 << " " << ber << endl;
	fstream fout("Result1.txt", ios::out | ios::app);
	if (!fout.is_open()) {
		cout << "error" << endl;
		exit(1);
	}
	fout << ebn0 << " " << ber << endl;
	fout.close();
}

double awgn(double bpsk, int ebn0)
{
	double s = 0.0, v1, v2, x;
	do {
		v1 = 2.0 * (rand() / ((double)RAND_MAX)) - 1.0;
		v2 = 2.0 * (rand() / ((double)RAND_MAX)) - 1.0;
		s = v1 * v1 + v2 * v2;
	} while (s >= 1.0);
	s = sqrt(-2.0 * log(s) / s);
	
	double Eb = 1;
	double snr = pow(10, 0.1*ebn0);  // snr = e_d/N_0
	double sigma = sqrt((Eb / snr) / 2); 
	return bpsk + sigma * v1 * s;
}

void generate_rm()
{
    srand(time(NULL)); 
	for (int i = 0; i < LEN; i++) {
		random_message[i] = rand() % 2;
	}
}

void calc_ber(int enb0)
{
	int err = 0;
	for (int i = 0; i < LEN; i++) {
        if (after_demod[i] != random_message[i])
            err++;
	}
	double ber = err / (double)LEN;
	print(enb0, ber); 
}

int main() {
	fstream fclear("Result1.txt", ios::out);
	srand(time(NULL)); 
	for (int x = 0; x < EBN0; x++) { 
		generate_rm();
		for (int i = 0; i < LEN; i++) {	
            after_mod[i] = modulation(random_message[i]);
            after_AWGN[i] = awgn(after_mod[i], x);
            after_demod[i] = demod(after_AWGN[i]);
		}
		calc_ber(x);
	}
	return 0;
}
