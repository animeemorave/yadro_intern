#include "TapeImplementation.hpp"
#include <cmath>
#include <sstream>

TapeImplementation::TapeImplementation(
    std::string &input_file,
    std::string &config_file,
    std::string &mode
) {
    std::ifstream file;
    if (mode != "write" && mode != "read") {
        throw std::invalid_argument("Unknown mode");
    }
    if (mode == "write") {
        tape.open(input_file, std::ios::in | std::ios::out | std::ios::trunc);
    } else {
        tape.open(input_file, std::ios::in | std::ios::out);
    }
    if (!tape) {
        throw std::invalid_argument("can`t open config_file");
    }
    file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    file.open(config_file);
    if (!file) {
        throw std::invalid_argument("can`t open file");
    }
    TapeConfig config;
    std::string read_buffer;
    while (!file.eof()) {
        getline(file, read_buffer);
        std::stringstream ss(read_buffer);
        std::string key;
        if (std::getline(ss, key, '=')) {
            int value;
            ss >> value;
            if (key == "read_delay") {
                config.read_delay_ = std::chrono::milliseconds(value);
            } else if (key == "write_delay") {
                config.write_delay_ = std::chrono::milliseconds(value);
            } else if (key == "shift_delay") {
                config.shift_delay_ = std::chrono::milliseconds(value);
            } else if (key == "to_start_delay") {
                config.to_start_delay_ = std::chrono::milliseconds(value);
            }
        }
    }
    file.close();
}

TapeImplementation::~TapeImplementation() {
    if (tape.is_open()) {
        tape.close();
    }
}

void TapeImplementation::read(int &value) {
    tape.seekg(index);
    tape >> value;
    std::this_thread::sleep_for(std::chrono::milliseconds(config.read_delay_));
}

void TapeImplementation::write(int value) {
    tape.clear();
    tape.seekg(index);
    tape << value << ' ';
    last_index = index;
    std::this_thread::sleep_for(std::chrono::milliseconds(config.write_delay_));
}

void TapeImplementation::shift_l() {
    index = last_index;
    tape.seekg(index);
    std::this_thread::sleep_for(std::chrono::milliseconds(config.shift_delay_));
}

void TapeImplementation::shift_r() {
    int value;
    tape.seekg(index);
    tape >> value;
    last_index = index;
    index += std::to_string(value).length() + 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(config.shift_delay_));
}

void TapeImplementation::to_start() {
    index = 0;
    last_index = 0;
    tape.seekg(index);
    std::this_thread::sleep_for(config.to_start_delay_);
}

void TapeImplementation::insert_value(int value) {
    std::string cur_string;
    tape.seekg(index);
    getline(tape, cur_string);
    write(value);
    tape << cur_string;
}

bool TapeImplementation::is_eof() {
    int value;
    tape.seekg(index);
    return !(tape >> value);
}
