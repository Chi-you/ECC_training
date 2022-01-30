#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>

using namespace std;

#define LEN 4000000 // the length of bit
#define EBN0 12


typedef struct convdata{
    int state[6] = {0, 0, 0, 0, 0, 0};
    int input;
    int output[2];
} convdata;

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

int bitmul(int x, int y)
{
	return (x == 1 && y == 1) ? 1 : 0;
}

int bitxor(int x, int y)
{
	return (x == y) ? 0 : 1;
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
	double snr = pow(10, 0.1*ebn0);  
	double sigma = sqrt((Eb / snr) / 2); 
	return bpsk + sigma * v1 * s;
}

int generate_info_bit(int x)
{
    if (x == 0) {
        return 1;
    }
    if (x >= 1 && x <= 5) {
        return 0;
    }
    return bitxor(generate_info_bit(x-5), generate_info_bit(x-6));
}

vector<convdata> encoder(vector<int> input,  int G1[], int G2[]) 
{
    vector<convdata> data;
	data.resize(input.size());
	int size = 0;

	for (int i = 0; i < input.size(); i++) {
        if(i > 0) {
            for(int j = 0; j < 6; j++){
                data[i].state[j] = data[i-1].state[j];
            }
        }
        // cout<<"Before shift"<<endl;
        // for(int j = 0; j < 6; j++){
        //     cout<<data[i].state[j]<<" ";
        // }
        // cout<<endl; 
		// Storing Output: G[G1, G2]
        data[i].output[1] = bitmul(input[i], G1[0]);
        data[i].output[0] = bitmul(input[i], G2[0]);
        for (int j = 1; j < 7; j++) {
            data[i].output[1] = bitxor(data[i].output[1], bitmul(G1[j], data[i].state[j-1]));
            data[i].output[0] = bitxor(data[i].output[0], bitmul(G2[j], data[i].state[j-1]));
        }
        for (int j = 5; j > 0; j--) { // shift right
            data[i].state[j] = data[i].state[j-1];
        }
		data[i].input = input[i];
		data[i].state[0] = input[i];
		size++;
        // cout<<"after shift"<<endl;
        for(int j = 0; j < 6; j++){
            cout<<data[i].state[j]<<" ";
        }
        cout<<endl;        
	}
	data.resize(size);
    return data;
}

void decoder()
{

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
    int G_1[7] = {1, 0, 1, 1, 0, 1, 1};
    int G_2[7] = {1, 1, 1, 1, 0, 0, 1};
	fstream fclear("Result1.txt", ios::out);
    vector<int> info_bit;
    for (int i = 0; i < 12; i++) {
        info_bit.push_back(generate_info_bit(i));
    }
    for (int i = 0; i < info_bit.size(); i++) {
        cout << info_bit[i] << " ";
    }    
    cout<<"\n";
    vector<convdata> encoded = encoder(info_bit, G_1, G_2);
    for (int i = 0; i < info_bit.size(); i++) {
        cout << encoded[i].output[1] << encoded[i].output[0] << " ";
    }

	// for (int x = 0; x < EBN0; x++) { 
	// 	generate_info_bit(LEN);
	// 	for (int i = 0; i < LEN; i++) {	
    //         after_mod[i] = modulation(random_message[i]);
    //         after_AWGN[i] = awgn(after_mod[i], x);
    //         after_demod[i] = demod(after_AWGN[i]);
	// 	}
	// 	calc_ber(x);
	// }
	// return 0;
}
