#include "FB_DSSE.h"
#include <iostream>
#include <chrono>
#pragma warning(disable : 4996)
using namespace std;
using namespace chrono;


int main() {
	// bitlen : maximum number of files supported by the system.
	int updatetimes = 10;

	string w = "hello";
	for (int i = 0; i <= 6; i++) {
		int bitLen = pow(10, i + 1);
		cout << "bitLen: " << bitLen << endl;
		cout << "Insert Time:" << endl;
		FB_DSSE my(bitLen);
		BIGNUM* tmp = BN_new();
		BN_set_word(tmp, 0b10);

		system_clock::time_point start = system_clock::now();
		for (int j = 0; j < updatetimes; j++) {
			my.Update(w, tmp);
		}
		system_clock::time_point end = system_clock::now();
		nanoseconds nano = (end - start);

		cout << nano.count() / updatetimes << endl;

		cout << "Delete Time:" << endl;
		BN_set_word(tmp, -0b10);

		start = system_clock::now();
		for (int j = 0; j < updatetimes; j++) {
			my.Update(w, tmp);
		}
		end = system_clock::now();
		nano = (end - start);

		cout << nano.count() / updatetimes << endl;

		cout << "Search Time:" << endl;
		BIGNUM* tmp2 = BN_new();
		start = system_clock::now();
		tmp2 = my.Search(w);
		end = system_clock::now();
		nano = end - start;
		cout << nano.count() << endl;
		cout << endl;
	}
	return 0;
}