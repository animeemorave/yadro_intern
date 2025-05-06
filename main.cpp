#include <cassert>
#include <iostream>
#include "TapeSort.hpp"

const int MAX_ELEMENT = 5;

int main(int argc, char *argv[]) {
    assert(argc == 3);
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    std::string nullable_config = "null_config.txt";
    std::string read_mode = "read", write_mode = "write";
    try {
        TapeImplementation inputTape(inputFile, nullable_config, read_mode);
        TapeImplementation outputTape(outputFile, nullable_config, write_mode);
        TapeSort sorter(inputTape, outputTape, MAX_ELEMENT);
        sorter.sort();
    } catch (std::exception &err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}