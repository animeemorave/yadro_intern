#ifndef LEDGER_HPP
#define LEDGER_HPP

#include <algorithm>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ledger {

struct Time {
    int hours;
    int minutes;

    Time(int h = 0, int m = 0);
    explicit Time(const std::string &time_str);
    std::string to_string() const;
    bool operator<(const Time &rhs) const;
    bool operator==(const Time &rhs) const;
    Time &operator+=(const Time &rhs);
    friend Time operator-(const Time &lhs, const Time &rhs);
};

struct Table {
    bool occupied = false;
    Time current_usage_start;
    Time total_usage_time;
    int revenue = 0;
};

class Ledger {
public:
    Ledger(
        int table_count,
        int hourly_rate,
        Time opening_time,
        Time closing_time
    );
    void handle_arrival(const std::vector<std::string> &args);
    void handle_seating(const std::vector<std::string> &args);
    void handle_waiting(const std::vector<std::string> &args);
    void handle_departure(const std::vector<std::string> &args);
    void print_final_report();

private:
    void process_queue(const Time &current_time, int freed_table);
    bool is_within_working_hours(const Time &current_time) const;
    bool validate_client_presence(
        const Time &current_time,
        const std::string &client,
        bool should_exist
    ) const;
    bool is_table_available(const Time &current_time, int table_number) const;
    void remove_client(const Time &current_time, const std::string &client);
    void update_table_usage(const Time &current_time, int table_number);

    const int hourly_rate_;
    const Time opening_time_;
    const Time closing_time_;
    int occupied_tables_count_;
    std::unordered_map<std::string, int> client_table_map;
    std::unordered_set<std::string> waiting_clients;
    std::queue<std::string> client_queue;
    std::vector<Table> tables;
};

}  // namespace ledger

#endif  // LEDGER_HPP