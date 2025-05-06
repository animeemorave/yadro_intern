#include <cstdio>
#include <fstream>
#include "TapeImplementation.hpp"
#include "doctest.h"

std::string mode_write = "write", mode_read = "read";

TEST_SUITE("TapeImplementation Tests") {
    TEST_CASE("Write and read value") {
        std::string input_file = "test_tape.txt";
        std::string config_file = "test_config.txt";
        std::ofstream config_out(config_file);
        config_out
            << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
        config_out.close();

        TapeImplementation tape(input_file, config_file, mode_write);
        int write_value = 123;
        tape.write(write_value);
        tape.to_start();

        int read_value;
        tape.read(read_value);
        CHECK(read_value == write_value);
    }

    TEST_CASE("Shift right moves to next position") {
        std::string input_file = "test_tape.txt";
        std::string config_file = "test_config.txt";
        std::ofstream config_out(config_file);
        config_out
            << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
        config_out.close();

        TapeImplementation tape(input_file, config_file, mode_write);
        tape.write(100);
        tape.shift_r();
        tape.write(200);
        tape.to_start();

        int first, second;
        tape.read(first);
        tape.shift_r();
        tape.read(second);

        CHECK(first == 100);
        CHECK(second == 200);
    }

    TEST_CASE("Shift left returns to previous position") {
        std::string input_file = "test_tape.txt";
        std::string config_file = "test_config.txt";
        std::ofstream config_out(config_file);
        config_out
            << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
        config_out.close();

        TapeImplementation tape(input_file, config_file, mode_write);
        tape.write(100);
        tape.shift_r();
        tape.shift_l();
        int value;
        tape.read(value);
        CHECK(value == 100);
    }

    TEST_CASE("to_start resets position") {
        std::string input_file = "test_tape.txt";
        std::string config_file = "test_config.txt";
        std::ofstream config_out(config_file);
        config_out
            << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
        config_out.close();
        TapeImplementation tape(input_file, config_file, mode_write);
        tape.write(100);
        tape.shift_r();
        tape.to_start();
        CHECK(tape.is_eof() == false);
    }
    TEST_CASE("is_eof detects end of tape") {
        std::string input_file = "test_tape.txt";
        std::string config_file = "test_config.txt";
        std::ofstream config_out(config_file);
        config_out
            << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
        config_out.close();
        TapeImplementation tape(input_file, config_file, mode_write);
        tape.write(100);
        tape.shift_r();
        CHECK(tape.is_eof() == true);
    }
    TEST_CASE("Insert value maintains tape integrity") {
        std::string input_file = "test_tape.txt";
        std::string config_file = "test_config.txt";
        std::ofstream config_out(config_file);
        config_out
            << "read_delay=0\nwrite_delay=0\nshift_delay=0\nto_start_delay=0";
        config_out.close();
        TapeImplementation tape(input_file, config_file, mode_write);
        tape.write(100);
        tape.shift_r();
        tape.write(200);
        tape.to_start();
        tape.insert_value(50);
        int first, second, third;
        tape.read(first);
        tape.shift_r();
        tape.read(second);
        tape.shift_r();
        tape.read(third);
        CHECK(first == 50);
        CHECK(second == 100);
        CHECK(third == 200);
    }

    TEST_CASE("Invalid mode throws exception") {
        std::string input_file = "test.txt";
        std::string config_file = "config.txt";
        { std::ofstream(config_file) << "read_delay=0"; }
        std::string mode = "invalid_mode";
        CHECK_THROWS_AS(
            TapeImplementation(input_file, config_file, mode),
            std::invalid_argument
        );
    }

    TEST_CASE("Non-existent input file in read mode throws") {
        std::string input_file = "nonexistent.txt";
        std::string config_file = "config.txt";
        { std::ofstream(config_file) << "read_delay=0"; }
        CHECK_THROWS_AS(
            TapeImplementation(input_file, config_file, mode_read),
            std::invalid_argument
        );
    }
}