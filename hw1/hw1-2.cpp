#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

#define LEN 1000000 // the length of bit
#define EBN0 11

#define M 3
#define N 7
#define K 4

// int G[K][N] = {
//     {1, 0, 0, 0, 1, 0, 1},
//     {0, 1, 0, 0, 1, 1, 1},
//     {0, 0, 1, 0, 1, 1, 0},
//     {0, 0, 0, 1, 0, 1, 1}
// };

// int H[M][N] = {
//     {1, 1, 1, 0, 1, 0, 0},
//     {0, 1, 1, 1, 0, 1, 0},
//     {1, 1, 0, 1, 0, 0, 1}
// };

int G[K][N] = {
    {1, 0, 0, 0, 1, 1, 1},
    {0, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 0, 1, 0, 1},
    {0, 0, 0, 1, 0, 1, 1}
};

int H[M][N] = {
    {1, 1, 1, 0, 1, 0, 0},
    {1, 1, 0, 1, 0, 1, 0},
    {1, 0, 1, 1, 0, 0, 1}
};

// int G[K][N] = {
//     {1, 1, 0, 1, 0, 0, 0},
//     {0, 1, 1, 0, 1, 0, 0},
//     {1, 1, 1, 0, 0, 1, 0},
//     {1, 0, 1, 0, 0, 0, 1}
// };

// int H[M][N] = {
//     {1, 0, 0, 1, 0, 1, 1},
//     {0, 1, 0, 1, 1, 1, 0},
//     {0, 0, 1, 0, 1, 1, 1}
// };


int random_message[LEN][K] = {0};
int coding_matrix[LEN][N] = {0};
double after_AWGN[LEN][N] = {0.0};
int after_demod[LEN][N] = {0};
int decoding_matrix[LEN][K] = {0};

int H_T[N][M];

void init()
{
	for (int i = 0; i < LEN; i++) {
		for (int j = 0; j < N; j++) {
			coding_matrix[i][j] = 0;
			after_AWGN[i][j] = 0.0;
			after_demod[i][j] = 0;
		}
	}
}

void generate_H_T()
{
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			H_T[j][i] = H[i][j];
		}
	}
}

void modulation(int *random) 
{
	*random = (*random == 1) ? -1 : 1;
}

int demod(double code) 
{
	return (code > 0) ? 0 : 1;
}

void print(int ebn0, double ber) 
{
	cout << ebn0 << " " << ber << endl;
	fstream fout("Result2.txt", ios::out | ios::app);
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
	
	double Eb = (double) N/ (double) 4;
	double snr = pow(10, 0.1*ebn0);  
	double sigma = sqrt((Eb / snr) / 2); 
	return bpsk + sigma * v1 * s;
}

void generate_rm()
{
	for (int i = 0; i < LEN; i++) {
		for (int j = 0; j < K; j++) {
			random_message[i][j] = rand() % 2;
		}
	}
}

int bitmul(int x, int y)
{
	return (x == 1 && y == 1) ? 1 : 0;
}

int bitxor(int x, int y)
{
	return (x == y) ? 0 : 1;
}

int bitinvert(int x)
{
	return (x == 0) ? 1 : 0;
}

void channel_encoder()
{
    for (int i = 0; i < LEN; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < K; k++) {
                coding_matrix[i][j] = bitxor(coding_matrix[i][j], bitmul(random_message[i][k], G[k][j]));
            }
        }
    }
}

void channel_decoder()
{
	int s[M] = {0}; // syndrome, if syndrome = 0, legal codeword
    for (int i = 0; i < LEN; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < N; k++) {
                s[j] = bitxor(s[j], bitmul(after_demod[i][k], H_T[k][j])); // s = rH^T
            }
        }
		int flag = 0;
		for (int x = 0; x < M; x++)
			flag += s[x];

		if (flag > 0) { // syndrome != zero vector => illegal (transmission errors exist) => correct
			int errorbit = -1;
			for (int y = 0; y < N; y++) {
				int temp = 0;
				for (int x = 0; x < M; x++) {
					if (s[x] == H_T[y][x])
						temp++;
				}
				if (temp == M) {
					errorbit = y;
					break;
				}
			}
			if (errorbit != -1) {
				after_demod[i][errorbit] = bitinvert(after_demod[i][errorbit]);
			}
		}
		for (int j = 0; j < K; j++)
			decoding_matrix[i][j] = after_demod[i][j];

		for (int j = 0; j < M; j++)  
			s[j] = 0;
    }	
}

void calc_ber(int enb0)
{
	int err = 0;
	for (int i = 0; i < LEN; i++) {
		for (int j = 0; j < K; j++) {
			if (decoding_matrix[i][j] != random_message[i][j])
				err++;
		}
	}
	double ber = err / (double)LEN;
	print(enb0, ber); 
}

int main() {
	fstream fclear("Result2.txt", ios::out);
	srand(time(NULL)); 
	generate_H_T();
	for (int x = 0; x < EBN0; x++) { 
		init();
		generate_rm();
		channel_encoder();
		for (int i = 0; i < LEN; i++) {
			for (int j = 0; j < N; j++) {
				modulation(&coding_matrix[i][j]);
				after_AWGN[i][j] = awgn(coding_matrix[i][j], x);
				after_demod[i][j] = demod(after_AWGN[i][j]);
			}
		}
		channel_decoder();
		calc_ber(x);
	}
	return 0;
}
