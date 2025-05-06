#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "TapeSort.hpp"
#include "doctest.h"

using namespace std;

void create_null_config() {
    ofstream config_file("null_config.txt");
    config_file
        << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
}

void create_tmp_dir() {
    system("mkdir -p tmp");
}

void clean_up() {
    remove("null_config.txt");
    system("rm -rf tmp");
}

vector<int> read_tape_contents(TapeInterface &tape) {
    vector<int> results;
    tape.to_start();
    while (!tape.is_eof()) {
        int num;
        tape.read(num);
        results.push_back(num);
        tape.shift_r();
    }
    return results;
}

TEST_SUITE("TapeSort Tests") {
    TEST_CASE("Basic sorting") {
        create_null_config();
        create_tmp_dir();
        ofstream input_file("input.txt");
        input_file << "5 3 1 4 2";
        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 5);
        sorter.sort();

        vector<int> result = read_tape_contents(*output_tape);
        vector<int> expected = {1, 2, 3, 4, 5};
        CHECK(result == expected);

        clean_up();
    }

    TEST_CASE("Multiple temporary tapes merge") {
        create_null_config();
        create_tmp_dir();
        ofstream input_file("input.txt");
        input_file << "9 7 5 3 1 8 6 4 2 0";
        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 3);
        sorter.sort();

        vector<int> result = read_tape_contents(*output_tape);
        vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(result == expected);
        clean_up();
    }

    TEST_CASE("Edge case: max_element=1") {
        create_null_config();
        create_tmp_dir();
        ofstream input_file("input.txt");
        input_file << "5 3 1 4 2";
        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 1);
        sorter.sort();

        vector<int> result = read_tape_contents(*output_tape);
        vector<int> expected = {1, 2, 3, 4, 5};
        CHECK(result == expected);

        clean_up();
    }

    TEST_CASE("Empty input tape") {
        create_null_config();
        create_tmp_dir();
        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 5);
        sorter.sort();

        output_tape->to_start();
        CHECK(output_tape->is_eof());

        clean_up();
    }

    TEST_CASE("Already sorted input") {
        create_null_config();
        create_tmp_dir();
        ofstream input_file("input.txt");
        input_file << "1 2 3 4 5";
        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 3);
        sorter.sort();

        vector<int> result = read_tape_contents(*output_tape);
        vector<int> expected = {1, 2, 3, 4, 5};
        CHECK(result == expected);

        clean_up();
    }

    TEST_CASE("Reverse sorted input") {
        create_null_config();
        create_tmp_dir();
        ofstream input_file("input.txt");
        input_file << "5 4 3 2 1";
        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 2);
        sorter.sort();

        vector<int> result = read_tape_contents(*output_tape);
        vector<int> expected = {1, 2, 3, 4, 5};
        CHECK(result == expected);

        clean_up();
    }

    TEST_CASE("All elements same") {
        create_null_config();
        create_tmp_dir();
        ofstream input_file("input.txt");
        input_file << "2 2 2 2 2";

        std::string filename_in = "input.txt", filename_out = "output.txt",
                    config = "null_config.txt", mode_write = "write",
                    mode_read = "read";
        TapeInterface *input_tape =
            new TapeImplementation(filename_in, config, mode_read);
        TapeInterface *output_tape =
            new TapeImplementation(filename_out, config, mode_write);

        TapeSort sorter(*input_tape, *output_tape, 3);
        sorter.sort();

        vector<int> result = read_tape_contents(*output_tape);
        vector<int> expected = {2, 2, 2, 2, 2};
        CHECK(result == expected);

        clean_up();
    }
}