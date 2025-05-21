#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "ledger.hpp"

bool check_valid_name(const std::string &name) {
    return std::all_of(name.begin(), name.end(), [&](char ch) {
        return ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9'));
    });
}

void check(
    std::vector<std::string> &commands,
    int num_tables,
    ledger::Time &cur_time
) {
    if (commands.size() < 3) {
        throw std::invalid_argument("");
    }
    const ledger::Time event_time(commands[0]);
    if (event_time < cur_time) {
        throw std::invalid_argument("");
    }
    cur_time = event_time;
    const int command = stoi(commands[1]);
    const std::string name = commands[2];
    if ((command < 0 || command > 4) || !check_valid_name(name)) {
        throw std::invalid_argument("");
    }
    switch (command) {
        case 2: {
            const int table_number = stoi(commands[3]);
            if (commands.size() != 4 || table_number < 0 ||
                table_number > num_tables) {
                throw std::invalid_argument("");
            }
        } break;
        default:
            if (commands.size() != 3) {
                throw std::invalid_argument("");
            }
    }
}

int main([[maybe_unused]] int argc, char *argv[]) {
    assert(argc == 2);
    // NOLINTNEXTLINE [cppcoreguidelines-pro-bounds-pointer-arithmetic]
    std::ifstream input(argv[1]);
    input.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if (!input.is_open()) {
        return 1;
    }
    std::string read_buffer;
    int num_tables = 0;
    int price = 0;
    getline(input, read_buffer);
    try {
        num_tables = stoi(read_buffer);
        if (num_tables <= 0) {
            throw std::invalid_argument("");
        }
    } catch (...) {
        std::cout << read_buffer << "\n";
        return 0;
    }
    ledger::Time start_time;
    ledger::Time end_time;
    try {
        getline(input, read_buffer);
        std::stringstream ss(read_buffer);
        std::string open_str;
        std::string close_str;
        ss >> open_str >> close_str;
        start_time = ledger::Time(open_str);
        end_time = ledger::Time(close_str);
        if (start_time.hours > end_time.hours ||
            (start_time.hours == end_time.hours &&
             start_time.minutes > end_time.minutes)) {
            throw std::invalid_argument("");
        }
    } catch (std::invalid_argument &err) {
        std::cout << read_buffer << "\n";
        return 0;
    }
    std::cout << start_time.to_string() << "\n";
    getline(input, read_buffer);
    try {
        price = stoi(read_buffer);
        if (price <= 0) {
            throw std::invalid_argument("");
        }
    } catch (...) {
        std::cout << read_buffer << "\n";
        return 0;
    }
    ledger::Ledger ledger(num_tables, price, start_time, end_time);
    static const std::map<int, std::function<void(std::vector<std::string> &)>>
        function_table = {
            {1, [&](std::vector<std::string> &args
                ) { ledger.handle_arrival(args); }},
            {2, [&](std::vector<std::string> &args
                ) { ledger.handle_seating(args); }},
            {3, [&](std::vector<std::string> &args
                ) { ledger.handle_waiting(args); }},
            {4, [&](std::vector<std::string> &args
                ) { ledger.handle_departure(args); }}
        };
    ledger::Time cur_time;
    while (!input.eof()) {
        getline(input, read_buffer);
        std::cout << read_buffer << "\n";
        std::vector<std::string> commands;
        std::stringstream ss(read_buffer);
        std::string command;
        while (ss >> command) {
            commands.push_back(command);
        }
        try {
            check(commands, num_tables, cur_time);
            const int command_number = stoi(commands[1]);
            function_table.at(command_number)(commands);
        } catch (...) {
            return 0;
        }
    }
    input.close();
    ledger.print_final_report();
    return 0;
}