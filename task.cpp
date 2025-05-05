#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include "ledger.hpp"

int main(int argc, char *argv[]) {
    std::ifstream input(argv[1]);
    std::string read_buffer;
    int sit_count = 0, price = 0;
    input >> sit_count;
    input >> read_buffer;
    std::cout << read_buffer << "\n";
    const ledger_namespace::time start_time(read_buffer);
    input >> read_buffer;
    const ledger_namespace::time end_time(read_buffer);
    input >> price;
    getline(input, read_buffer);
    ledger_namespace::ledger ledger(sit_count, price, start_time, end_time);
    static const std::map<int, std::function<void(std::vector<std::string> &)>>
        function_table = {
            {1, [&](std::vector<std::string> &args) { ledger.arrived(args); }},
            {2, [&](std::vector<std::string> &args) { ledger.sit(args); }},
            {3, [&](std::vector<std::string> &args) { ledger.wait(args); }},
            {4, [&](std::vector<std::string> &args) { ledger.left(args); }}
        };
    while (getline(input, read_buffer)) {
        std::vector<std::string> commands;
        std::stringstream ss(read_buffer);
        std::string command;

        while (ss >> command) {
            commands.push_back(command);
        }
        int command_number = stoi(commands[1]);
        function_table.at(command_number)(commands);
    }
    input.close();
    ledger.kick_out();
    std::cout << end_time.to_string() << std::endl;
    ledger.earnings();
    return 0;
}