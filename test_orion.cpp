#include "orion/Orion.h"
#include <chrono>
using namespace std;
using namespace chrono;

int main(int, char**) {

    bool usehdd = false;
    int updatetimes = 10;
    for (int i = 0; i <= 6; i++) {
        int bitLen = pow(10, i + 1);
        cout << "bitLen: " << bitLen << endl;
        cout << "Insert Time:" << endl;
        Orion orion(usehdd, bitLen);

        string w1 = "hello";
        int idx = rand() % bitLen;

        system_clock::time_point start = system_clock::now();
        for (int j = 0; j < updatetimes; j++) {
            orion.insert(w1, idx + j);
        }
        system_clock::time_point end = system_clock::now();
        nanoseconds nano = end - start;
        cout << nano.count() / updatetimes << endl;

        cout << "Delete Time:" << endl;
        start = system_clock::now();
        for (int j = 0; j < updatetimes; j++) {
            orion.remove(w1, idx + j);
        }
        end = system_clock::now();
        nano = end - start;
        cout << nano.count() / updatetimes << endl;
        
        cout << "Search Time:" << endl;
        start = system_clock::now();
        vector<int> res = orion.search(w1);
        end = system_clock::now();
        nano = end - start;
        cout << nano.count() << endl;
        cout << endl;

    }
    return 0;
}

