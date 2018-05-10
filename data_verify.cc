#include "SHA1.h"


int main(int argc, char* argv[]) {
    // Open data
    std::ifstream data ("test2.txt");

    if (data) {
        SHA1 * data_verify = new SHA1(data);
        data_verify->compute_hash();
        data_verify->print_hash_value();
        delete data_verify;
    } else {
        std::cout << "File not found" << std::endl;
    }
    std::cout << std::endl;

    

    /*
    char * buff = new char [4];
    buff[0] = 'a';
    buff[1] = 'b';
    buff[2] = 'c';
    buff[3] = 'd';
    std::cout << std::hex << "0x" << buffer_to_32block(buff, 0) << std::endl;
    delete[] buff;
    */
}
