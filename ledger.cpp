#include "ledger.hpp"

namespace ledger {

Time::Time(int h, int m) : hours(h), minutes(m) {
    if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60) {
        throw std::invalid_argument("Invalid time values");
    }
}

Time::Time(const std::string &time_str) {
    if (time_str.size() != 5 || time_str[2] != ':' ||
        sscanf(time_str.c_str(), "%d:%d", &hours, &minutes) != 2) {
        throw std::invalid_argument("Invalid time format");
    }
    if (hours < 0 || hours >= 24 || minutes < 0 || minutes >= 60) {
        throw std::invalid_argument("Invalid time values");
    }
}

std::string Time::to_string() const {
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
    return buffer;
}

bool Time::operator==(const Time &rhs) const {
    return hours == rhs.hours && minutes == rhs.minutes;
}

bool Time::operator<(const Time &rhs) const {
    return hours < rhs.hours || (hours == rhs.hours && minutes < rhs.minutes);
}

Time &Time::operator+=(const Time &rhs) {
    minutes += rhs.minutes;
    hours += rhs.hours + minutes / 60;
    minutes %= 60;
    hours %= 24;
    return *this;
}

Time operator-(const Time &lhs, const Time &rhs) {
    int total_minutes =
        (lhs.hours * 60 + lhs.minutes) - (rhs.hours * 60 + rhs.minutes);
    if (total_minutes < 0) {
        total_minutes += 1440;
    }
    return {total_minutes / 60, total_minutes % 60};
}

Ledger::Ledger(
    int table_count,
    int hourly_rate,
    Time opening_time,
    Time closing_time
)
    : hourly_rate_(hourly_rate),
      opening_time_(opening_time),
      closing_time_(closing_time),
      occupied_tables_count_(0),
      tables(table_count + 1) {
}

void Ledger::handle_arrival(const std::vector<std::string> &args) {
    const Time current(args[0]);
    const std::string &client = args[2];
    if (!is_within_working_hours(current) ||
        validate_client_presence(current, client, true)) {
        return;
    }
    client_table_map[client] = 0;
}

void Ledger::handle_seating(const std::vector<std::string> &args) {
    const Time current(args[0]);
    const std::string &client = args[2];
    const int table_number = std::stoi(args[3]);
    if (validate_client_presence(current, client, false) ||
        is_table_available(current, table_number)) {
        return;
    }
    tables[table_number].current_usage_start = current;
    tables[table_number].occupied = true;
    if (client_table_map[client] == 0) {
        occupied_tables_count_++;
    }
    update_table_usage(current, client_table_map[client]);
    client_table_map[client] = table_number;
}

void Ledger::print_final_report() {
    std::vector<std::string> clients;
    for (const auto &entry : client_table_map) {
        clients.push_back(entry.first);
    }
    std::sort(clients.begin(), clients.end());
    for (const auto &client : clients) {
        remove_client(closing_time_, client);
    }
    std::cout << closing_time_.to_string() << "\n";
    for (size_t i = 1; i < tables.size(); ++i) {
        std::cout << i << " " << tables[i].revenue << " "
                  << tables[i].total_usage_time.to_string() << "\n";
    }
}

void Ledger::handle_waiting(const std::vector<std::string> &args) {
    const Time current(args[0]);
    const std::string &client = args[2];
    if (validate_client_presence(current, client, false)) {
        return;
    }
    const size_t free_tables = tables.size() - 1 - occupied_tables_count_;
    if (free_tables > 0) {
        std::cout << current.to_string() << " 13 ICanWaitNoLonger!\n";
        return;
    }
    if (waiting_clients.size() >= tables.size() - 1) {
        remove_client(current, client);
        return;
    }
    client_queue.push(client);
    waiting_clients.insert(client);
}

void Ledger::handle_departure(const std::vector<std::string> &args) {
    const Time current(args[0]);
    const std::string &client = args[2];
    if (validate_client_presence(current, client, false)) {
        return;
    }
    const int table_number = client_table_map[client];
    if (table_number != 0) {
        occupied_tables_count_--;
        update_table_usage(current, table_number);
        process_queue(current, table_number);
    }
    waiting_clients.erase(client);
    client_table_map.erase(client);
}

void Ledger::process_queue(const Time &current_time, int freed_table) {
    if (client_queue.empty()) {
        return;
    }
    std::string next_client;
    do {
        next_client = client_queue.front();
        client_queue.pop();
    } while (!client_queue.empty() &&
             waiting_clients.find(next_client) == waiting_clients.end());

    if (waiting_clients.find(next_client) == waiting_clients.end()) {
        return;
    }
    occupied_tables_count_++;
    client_table_map[next_client] = freed_table;
    tables[freed_table].current_usage_start = current_time;
    tables[freed_table].occupied = true;
    waiting_clients.erase(next_client);
    std::cout << current_time.to_string() << " 12 " << next_client << " "
              << freed_table << "\n";
}

bool Ledger::is_within_working_hours(const Time &current_time) const {
    if (current_time < opening_time_ ||
        !(current_time < closing_time_ || current_time == closing_time_)) {
        std::cout << current_time.to_string() << " 13 NotOpenYet\n";
        return false;
    }
    return true;
}

bool Ledger::validate_client_presence(
    const Time &current_time,
    const std::string &client,
    bool should_exist
) const {
    const bool exists = client_table_map.find(client) != client_table_map.end();
    if (should_exist && exists) {
        std::cout << current_time.to_string() << " 13 YouShallNotPass\n";
        return true;
    }
    if (!should_exist && !exists) {
        std::cout << current_time.to_string() << " 13 ClientUnknown\n";
        return true;
    }
    return false;
}

bool Ledger::is_table_available(const Time &current_time, int table_number)
    const {
    if (table_number < 1 || (size_t)table_number >= tables.size()) {
        std::cout << current_time.to_string() << " 13 PlaceIsBusy\n";
        return true;
    }
    if (tables[table_number].occupied) {
        std::cout << current_time.to_string() << " 13 PlaceIsBusy\n";
        return true;
    }
    return false;
}

void Ledger::remove_client(
    const Time &current_time,
    const std::string &client
) {
    if (validate_client_presence(current_time, client, false)) {
        return;
    }
    const int table_number = client_table_map[client];
    if (table_number != 0) {
        occupied_tables_count_--;
        update_table_usage(current_time, table_number);
    }
    waiting_clients.erase(client);
    client_table_map.erase(client);
    std::cout << current_time.to_string() << " 11 " << client << "\n";
}

void Ledger::update_table_usage(const Time &current_time, int table_number) {
    if (table_number == 0) {
        return;
    }
    Table &table = tables[table_number];
    const Time duration = current_time - table.current_usage_start;
    table.total_usage_time += duration;
    const int total_minutes = duration.hours * 60 + duration.minutes;
    const int hours = (total_minutes + 59) / 60;
    table.revenue += hours * hourly_rate_;
    table.occupied = false;
    table.current_usage_start = Time(0, 0);
}
}  // namespace ledger