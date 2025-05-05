#include "ledger.hpp"
using namespace ledger_namespace;

time::time(int h, int m) : hour(h), minute(m) {
}

time::time(std::string &string) {
    hour = ((string[0] - '0') * 10 + (string[1] - '0'));
    minute = ((string[3] - '0') * 10 + (string[4] - '0'));
}

ledger_namespace::time &time::operator+=(const time &other) {
    hour = (hour + other.hour + (minute + other.minute) / 60) % 24;
    minute = (minute + other.minute) % 60;
    return *this;
}

[[nodiscard]] std::string time::to_string() const {
    const std::string hour_string =
        hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
    const std::string minute_string =
        minute < 10 ? "0" + std::to_string(minute) : std::to_string(minute);
    return hour_string + ":" + minute_string;
}

ledger::ledger(int sit_count, int hour_price, time start_time, time end_time)
    : sit_count_(sit_count),
      hour_price_(hour_price),
      start_time_(start_time),
      end_time_(end_time),
      busy_table(0),
      tables(sit_count + 1, time(-1, -1)),
      table_timer(sit_count + 1, time(0, 0)),
      table_earn(sit_count + 1, 0) {
}

void ledger::arrived(std::vector<std::string> &commands) {
    const time cur_time(commands[0]);
    std::string client_name = commands[2];
    std::cout << cur_time.to_string() << " 1 " + client_name << "\n";
    if (check_time(cur_time) ||
        check_client_contains(cur_time, client_name, true)) {
        return;
    }
    clients_to_tables[client_name] = 0;
}

void ledger::sit(std::vector<std::string> &commands) {
    const time cur_time(commands[0]);
    std::string client_name = commands[2];
    int number = std::stoi(commands[3]);
    std::cout << cur_time.to_string() << " 2 " + client_name << " " << number
              << "\n";
    check_client_contains(cur_time, client_name, false);
    if (!check_place(cur_time, number)) {
        tables[number] = cur_time;
        if (clients_to_tables[client_name] == 0) {
            busy_table++;
        }
        time wasted_time = cur_time - tables[clients_to_tables[client_name]];
        table_earn[clients_to_tables[client_name]] +=
            hour_price_ * (wasted_time.hour + (wasted_time.minute > 0 ? 1 : 0));
        table_timer[clients_to_tables[client_name]] += wasted_time;
        tables[clients_to_tables[client_name]].hour = -1;
        clients_to_tables[client_name] = number;
    }
}

void ledger::wait(std::vector<std::string> &commands) {
    const time cur_time(commands[0]);
    std::string client_name = commands[2];
    std::cout << cur_time.to_string() << " 3 " + client_name << "\n";
    if (check_free_place(cur_time)) {
        return;
    }
    if (client_in_queue.size() >= sit_count_) {
        client_left(cur_time, client_name);
        return;
    }
    queue.push(client_name);
    client_in_queue.emplace(client_name);
}

void ledger::left(std::vector<std::string> &commands) {
    const time cur_time(commands[0]);
    std::string client_name = commands[2];
    std::cout << cur_time.to_string() << " 4 " << client_name << "\n";
    bool flag = check_client_contains(cur_time, client_name, false);
    if (!flag && clients_to_tables[client_name] != 0) {
        busy_table--;
        time wasted_time = cur_time - tables[clients_to_tables[client_name]];
        table_earn[clients_to_tables[client_name]] +=
            hour_price_ * (wasted_time.hour + (wasted_time.minute > 0 ? 1 : 0));
        table_timer[clients_to_tables[client_name]] += wasted_time;
        tables[clients_to_tables[client_name]].hour = -1;
        client_in_queue.erase(client_name);
        refresh_queue(cur_time, clients_to_tables[client_name]);
    }
    if (!flag) {
        client_in_queue.erase(client_name);
        clients_to_tables.erase(client_name);
    }
}

void ledger::kick_out() {
    std::vector<std::string> clients;
    for (auto x : clients_to_tables) {
        clients.emplace_back(x.first);
    }
    std::sort(clients.begin(), clients.end());
    for (auto client : clients) {
        client_left(end_time_, client);
    }
}

void ledger_namespace::ledger::earnings() {
    for (int i = 1; i <= sit_count_; i++) {
        std::cout << i << " " << table_earn[i] << " "
                  << table_timer[i].to_string() << "\n";
    }
}

void ledger::refresh_queue(time cur_time, int freed_number) {
    if (queue.empty()) {
        return;
    }
    std::string client_name = queue.front();
    queue.pop();
    while (client_in_queue.find(client_name) != client_in_queue.end() &&
           !queue.empty()) {
        client_name = queue.front();
        queue.pop();
    }
    busy_table++;
    clients_to_tables[client_name] = freed_number;
    tables[freed_number] = cur_time;
    std::cout << cur_time.to_string() << " 12 " << client_name << " "
              << freed_number << "\n";
}

bool ledger::check_time(time cur_time) const {
    if ((cur_time.hour < start_time_.hour || cur_time.hour > end_time_.hour) ||
        (cur_time.hour == start_time_.hour &&
         cur_time.minute < start_time_.minute) ||
        (cur_time.hour == end_time_.hour && cur_time.minute > end_time_.minute
        )) {
        std::cout << cur_time.to_string() << " 13 NotOpenYet\n";
        return true;
    }
    return false;
}

bool ledger::client_contains(std::string &client_name) const {
    bool is_contains = true;
    try {
        clients_to_tables.at(client_name);
    } catch (...) {
        is_contains = false;
    }
    return is_contains;
}

bool ledger::check_client_contains(
    time cur_time,
    std::string &client_name,
    bool flag
) const {
    bool is_contains = client_contains(client_name);
    if (!is_contains && !flag) {
        std::cout << cur_time.to_string() << " 13 ClientUnknown\n";
        return true;
    }
    if (is_contains && flag) {
        std::cout << cur_time.to_string() << " 13 YouShallNotPass\n";
        return true;
    }
    return false;
}

bool ledger::check_free_place(time cur_time) const {
    if (sit_count_ - busy_table > 0) {
        std::cout << cur_time.to_string() << " 13 ICanWaitNoLonger!\n";
        return true;
    }
    return false;
}

bool ledger::check_place(time cur_time, int number) const {
    if (tables[number].hour != -1) {
        std::cout << cur_time.to_string() << " 13 PlaceIsBusy\n";
        return true;
    }
    return false;
}

void ledger::client_left(time cur_time, std::string &client_name) {
    bool flag = check_client_contains(cur_time, client_name, false);
    if (!flag && clients_to_tables[client_name] != 0) {
        busy_table--;
        time wasted_time = cur_time - tables[clients_to_tables[client_name]];
        table_earn[clients_to_tables[client_name]] +=
            hour_price_ * (wasted_time.hour + (wasted_time.minute > 0 ? 1 : 0));
        table_timer[clients_to_tables[client_name]] += wasted_time;
        tables[clients_to_tables[client_name]].hour = -1;
    }
    client_in_queue.erase(client_name);
    clients_to_tables.erase(client_name);
    std::cout << cur_time.to_string() << " 11 " << client_name << "\n";
}
