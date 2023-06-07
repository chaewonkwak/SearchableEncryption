#include "SSEClientHandler.h"

#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;
#pragma warning(disable : 4996)
int main() {
    int updatetimes = 10;

    string w1 = "hello";  
    for (int i = 0; i <= 9; i++) {
        int bitLen = GGM_SIZE; // pow(10, i + 1);
        cout << "bitLen: " << bitLen << endl;
        cout << "Insert Time:" << endl;
        int idx = rand() % bitLen;
        SSEClientHandler client;

        system_clock::time_point start = system_clock::now();
        for (int j = 0; j < updatetimes; j++) {
            client.update(INS, w1, idx + j);
        }
        system_clock::time_point end = system_clock::now();
        nanoseconds nano = end - start;

        cout << nano.count() / updatetimes << endl;

        cout << "Delete Time:" << endl;
        start = system_clock::now();
        for (int j = 0; j < updatetimes; j++) {
            client.update(DEL,w1, idx + j);
        }
        end = system_clock::now();
        nano = end - start;

        cout << nano.count() / updatetimes << endl;
        //    cout << duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << endl;
        //    cout << duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << endl;

        cout << "Search Time:" << endl;
        start = system_clock::now();
        vector<int> results = client.search(w1);
        end = system_clock::now();
        nano = end - start;
        cout << nano.count() << endl;
        cout << endl;

    }
    return 0;
}