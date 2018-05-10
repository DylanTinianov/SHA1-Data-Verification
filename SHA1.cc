#include "SHA1.h"


int stream_length(std::ifstream & stream) {
    stream.seekg (0, stream.end);
    int length = stream.tellg();
    stream.seekg (0, stream.beg);
    return length;
}

uint32_t rotate_left(uint32_t n, int d) {
    d = 32 - d; // rotate right 32 - d times
    return (n >> d)|(n << (NUM_BITS - d));
}

void pad_input(char* & buffer, int num_padding) {
    // TODO: pad the input
}

uint32_t buffer_to_32block(char* & buffer, int read_start) {
    uint32_t chunk1 = 0xFFFFFFFF & buffer[read_start];
    uint32_t chunk2 = (0xFFFFFFFF & buffer[read_start + 1]) << 8;
    uint32_t chunk3 = (0xFFFFFFFF & buffer[read_start + 2]) << 16;
    uint32_t chunk4 = (0xFFFFFFFF & buffer[read_start + 3]) << 24;
    return chunk1 | chunk2 | chunk3 | chunk4;
}

uint32_t f1(uint32_t & B, uint32_t & C, uint32_t & D) {
    return (B & C) | ((~B) & D);
}

uint32_t f2(uint32_t & B, uint32_t & C, uint32_t & D) {
    return B ^ C ^ D;
}

uint32_t f3(uint32_t & B, uint32_t & C, uint32_t & D) {
    return (B & C) | (B & D) | (C & D);
}

uint32_t f4(uint32_t & B, uint32_t & C, uint32_t & D) {
    return B ^ C ^ D;
}


void round_function(uint160_t & H, Stage t, int round_num, uint32_t* MsgW, uint32_t & A, uint32_t & B, uint32_t & C, uint32_t & D, uint32_t & E) {
    uint32_t A2;
    uint32_t B2;
    uint32_t C2;
    uint32_t D2;
    uint32_t E2;

    // Apply mapping
    if (t == t1) {
        A2 = E + f1(B, C, D) + rotate_left(A, 5) + MsgW[round_num] + STAGE_1_CONSTANT; // addition modulo 2^32 over unsigned 32bit ints
    } else if (t == t2) {
        A2 = E + f2(B, C, D) + rotate_left(A, 5) + MsgW[round_num] + STAGE_2_CONSTANT;
    } else if (t == t3) {
        A2 = E + f3(B, C, D) + rotate_left(A, 5) + MsgW[round_num] + STAGE_3_CONSTANT;
    } else {
        A2 = E + f4(B, C, D) + rotate_left(A, 5) + MsgW[round_num] + STAGE_4_CONSTANT;
    }
    B2 = A;
    C2 = rotate_left(B, 30);
    D2 = C;
    E2 = D;

    A = A2;
    B = B2;
    C = C2;
    D = D2;
    E = E2;
}


void compression_function(char* & buffer, uint160_t & H) {
    // Generate message schedule MsgW
    uint32_t MsgW[80];
    for (int i = 0, buff_count = 0; i < 16; i++, buff_count += 4) {
        MsgW[i] = buffer_to_32block(buffer, buff_count);
    }
    for (int i = 16; i < 80; i++) {
        MsgW[i] = MsgW[i-16] ^ MsgW[i-14] ^ MsgW[i-8] ^ MsgW[i-3]; // TODO: roate_left this?! 
    }

    Stage t = t1;
    uint32_t A = H.bits[0];
    uint32_t B = H.bits[1];
    uint32_t C = H.bits[2];
    uint32_t D = H.bits[3];
    uint32_t E = H.bits[4];

    for (int i = 0; i < NUM_COMPRESSION_ROUNDS; i++) {
        // Check if stage needs to change
        if (i == 20) {
            t = t2;
        } else if (i == 40) {
            t = t3;
        } else if (i == 60) {
            t = t4;
        }
        round_function(H, t, i, MsgW, A, B, C, D, E); // Update A-E
    }

    // Update h0, h1, h2, h3, h4 = A, B, C, D, E + Hi-1 [add modulo 2^32]
    H.bits[0] += A;
    H.bits[1] += B;
    H.bits[2] += C;
    H.bits[3] += D;
    H.bits[4] += E;
}


SHA1::SHA1(std::ifstream & data) : data(data) {
    // Initialize h0, h1, h2, h3, h4
    H.bits[0] = 0x67452301;
    H.bits[1] = 0xEFCDAB89;
    H.bits[2] = 0x98BADCFE;
    H.bits[3] = 0x10325476;
    H.bits[4] = 0xC3D2E1F0;
}

SHA1::~SHA1() {}


void SHA1::compute_hash() {
    int read_counter = stream_length(data); // how many characters are left to read in the message
    std::cout << "Message Length: " << read_counter << std::endl;

    char* buffer = new char [CHARS_512BITS];

    while (true) { 
        // Take next 512 bits of data, pad if needed
        if (read_counter >= CHARS_512BITS) {
            data.read(buffer, CHARS_512BITS); // read in 64 characters (512 bits)
            for (int i = 0; i < CHARS_512BITS; i++) {
                std::cout << buffer[i];
            }
            std::cout << std::endl;
            read_counter = read_counter - CHARS_512BITS;
        } else if (read_counter > 0) {
            data.read(buffer, read_counter); // read in the rest of the stream
            pad_input(buffer, read_counter);
            for (int i = 0; i < read_counter; i++) {
                std::cout << buffer[i];
            }
            read_counter = 0;
        } else {
            // Done!
            break;
        }
        
        // run compression function
        compression_function(buffer, H);
    }
    delete[] buffer;
}

void print_hex_uint160_t(uint160_t val) {
    std::cout << std::hex << val.bits[0] << val.bits[1] << val.bits[2] << val.bits[3] << val.bits[4];
}
