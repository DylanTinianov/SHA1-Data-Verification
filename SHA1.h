#ifndef SHA1_H
#define SHA1_H

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>

#define NUM_BITS 32
#define CHARS_512BITS 64
#define NUM_COMPRESSION_ROUNDS 80

#define STAGE_1_CONSTANT 0x5A827999
#define STAGE_2_CONSTANT 0x6ED9EBA1
#define STAGE_3_CONSTANT 0x8F1BBCDC
#define STAGE_4_CONSTANT 0xCA62C1D6


struct uint160_t {
    uint32_t bits[5];
};

enum Stage {t1, t2, t3, t4};


class SHA1 {
    public:
        SHA1(std::ifstream & data);
        virtual ~SHA1();
        void print_hex_uint160_t(uint160_t val);
        void print_hash_value();
        void compute_hash(); // Compute the hash

    private:
        std::ifstream & data;
        uint160_t H;
        Stage t;
        uint160_t hash_value;
};


#endif
