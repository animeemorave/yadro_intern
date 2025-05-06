#define TapeSort_HPP_
#ifdef TapeSort_HPP_
#include <algorithm>
#include <regex>
#include <vector>
#include "TapeImplementation.hpp"

class TapeSort {
public:
    TapeSort(
        TapeInterface &input_tape,
        TapeInterface &output_tape,
        int max_element
    );
    void sort();

private:
    TapeInterface &input_tape;
    TapeInterface &output_tape;
    int element_count;
    std::vector<int> part;
    std::vector<TapeInterface *> tapes;
    void merge();
};

#endif  // TapeSort_HPP_