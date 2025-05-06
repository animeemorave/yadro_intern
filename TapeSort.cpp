#include "TapeSort.hpp"

TapeSort::TapeSort(
    TapeInterface &input_tape,
    TapeInterface &output_tape,
    int max_element
)
    : input_tape(input_tape),
      output_tape(output_tape),
      element_count(max_element) {
}

void TapeSort::sort() {
    std::string nullable_config = "null_config.txt";
    std::string filename,mode = "write";
    int num;
    int parts = 0;
    while (!input_tape.is_eof()) {
        input_tape.read(num);
        input_tape.shift_r();
        part.push_back(num);
        if (part.size() == element_count) {
            std::sort(part.begin(), part.end());
            filename =
                "tmp/tmp_" + std::to_string(parts) + ".txt";

            TapeInterface *help_tape =
                new TapeImplementation(filename, nullable_config, mode);
            for (int i : part) {
                help_tape->write(i);
                help_tape->shift_r();
            }
            help_tape->to_start();
            tapes.push_back(help_tape);
            parts++;
            part.clear();
        }
    }
    if (!part.empty() && part.size() < element_count) {
        std::sort(part.begin(), part.end());
        filename = "tmp/tmp_" + std::to_string(parts) + ".txt";
        TapeInterface *help_tape =
            new TapeImplementation(filename, nullable_config, mode);
        for (int i : part) {
            help_tape->write(i);
            help_tape->shift_r();
        }
        help_tape->to_start();
        tapes.push_back(help_tape);
        parts++;
        part.clear();
    }
    merge();
}

void TapeSort::merge() {
    int index = 0, element1, element2;
    while (!tapes.empty()) {
        output_tape.to_start();
        while (!output_tape.is_eof() && !tapes[index]->is_eof()) {
            tapes[index]->read(element1);
            output_tape.read(element2);
            if (element1 <= element2) {
                output_tape.insert_value(element1);
                tapes[index]->shift_r();
            } else {
                output_tape.shift_r();
            }
        }
        while (!tapes[index]->is_eof()) {
            tapes[index]->read(element1);
            tapes[index]->shift_r();
            output_tape.write(element1);
            output_tape.shift_r();
        }
        tapes.erase(tapes.begin());
    }
}