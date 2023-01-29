// compile:
// g++ -std=c++17 -O3 -Wall -pedantic -o lcg lcg.cpp -march=native
//
// Note: -march=native turns out to be AVX2 on EC2 m5 instance
// Benchmarks:
//    100'000'000:   1.04 sec (normal),  0.36 sec (vectorized)
//  1'000'000'000:  10.4  sec (normal),  3.6  sec (vectorized)
// 10'000'000'000: 104    sec (normal), 36    sec (vectorized)

#include <array>
#include <chrono>
#include <immintrin.h>
#include <iostream>
#include <iomanip>

// output only the high 16 bits to avoid "eliminate the
// short period problem in the low-order bits" (Wikipedia)
inline uint16_t high_transform(uint32_t n) {
    return (n >> 16) & 0xffff;
}

// this is the essential operation of an LCG
// no modulus is neccessary because overflow acts
// as the implied modulus for VSC++
// (nice explanation at https://stackoverflow.com/a/14788283)
inline void update_state(uint32_t & state, uint32_t a, uint32_t c) {//, uint32_t m) {
    state = (a * state) + c ;//% m;
}

// virtually the same as update status for non-vectorized args (as above)
inline void update_state(__m256i & state_v, __m256i a_v, __m256i c_v) {
        state_v = _mm256_mullo_epi32(state_v, a_v);
        state_v = _mm256_add_epi32(state_v, c_v);
}

// print out a fancy hex dump of an array (or any container)
template<typename T>
void print_state(const T& state) {
    for(auto s : state) {
         std::cout << high_transform(s) << " ";
    }
    std::cout << "]";
}

int main() {

    // VS C++ LCG params (Wikipedia)
    const uint32_t A = 0x343FD;
    const uint32_t C = 0x269EC3;

    const size_t NUM_UPDATES = 100'000'000'00;

    // 256 bit seed
    const std::array<uint32_t, 8> SEED {{ 'C','O','L','U','M','B','I','A' }};
    
    // initialize state to SEED
    auto state = SEED; 
    
    auto start = std::chrono::system_clock::now();
    for(size_t i=0; i<SEED.size(); ++i) {
        for(size_t j = 0; j < NUM_UPDATES; ++j) {
            update_state(state[i], A, C);//, M);
        }
    }
    std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
    
    // output non-vectorized results
    std::cout << std::fixed << std::setprecision(9) << std::left;
    std::cout << "Perform " << (int)NUM_UPDATES << " loops for each (of 8) bytes of seed     (" << diff.count() << " sec): [ ";
    print_state(state);
    std::cout << "\n";
    
    // Now do vectorized
    const auto SEED_v = _mm256_stream_load_si256((__m256i const *)(SEED.data()));
    const auto A_v = _mm256_stream_load_si256((__m256i const *)(std::array<uint32_t, 8>{{A,A,A,A,A,A,A,A}}.data()));
    const auto C_v = _mm256_stream_load_si256((__m256i const *)(std::array<uint32_t, 8>{{C,C,C,C,C,C,C,C}}.data()));

    // as above for non-vectorized, initialize state to SEED
    auto state_v = SEED_v;
    
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i < NUM_UPDATES; ++i) {
        update_state(state_v, A_v, C_v);
    }
    diff = std::chrono::system_clock::now() - start;
    
    std::array<uint32_t,8> state_v_as_array;
    _mm256_storeu_si256((__m256i*) state_v_as_array.data(), state_v);
    
    // output result of vectorized approach
    std::cout << "Perform " << (int)NUM_UPDATES << " loops for all bytes of seed in parallel (" << diff.count() << " sec): [ ";
    print_state(state_v_as_array);
    std::cout << "\n";
}