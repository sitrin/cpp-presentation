// compile:
// g++ -std=c++17 -O3 -Wall -pedantic -o lcg lcg.cpp

#include <iostream>
#include  <iomanip>

inline uint16_t high_transform(uint32_t n) {
    return (n >> 16) & 0xffff;
}


int main() {
    //cout << "0x" << setfill('0') << setw(2) << right << hex << 10;

    const uint32_t A = 0x343FD;
    const uint32_t C = 0x269EC3;
    const uint32_t M = 0x80000000; // 2^32
    
    uint32_t state = 0;
    
    for (int i=0; i<10; ++i) {
        state = (A * state + C) % M;
        std::cout << high_transform(state) << "\n";
    }


}