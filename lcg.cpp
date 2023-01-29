// compile:
// g++ -std=c++17 -O3 -Wall -pedantic -o lcg lcg.cpp

#include <iostream>

inline uint16_t high_transform(uint32_t n) {
    return (n >> 16) & 0xffff;
}

inline void update_state(uint32_t & state, uint32_t a, uint32_t c, uint32_t m) {
    state = (a * state + c) % m;
}

int main() {
    //cout << "0x" << setfill('0') << setw(2) << right << hex << 10;

    const uint32_t A = 0x343FD;
    const uint32_t C = 0x269EC3;
    const uint32_t M = 0x80000000; // 2^32
    
    const uint32_t NUM_UPDATES = 10;

    
    const uint32_t SEED = 0;
    uint32_t state = SEED;
    
    // seed starts at 0 and goes to 4
    for(uint32_t i = 0; i < NUM_UPDATES; ++i) {
        update_state(state, A, C, M);
    }
    
    std::cout << "Using seed: " << SEED << "; state = " << state << " after " << NUM_UPDATES << " updates.\n";
    

}