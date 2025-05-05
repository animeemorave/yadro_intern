#define LEDGER_HPP_
#ifdef LEDGER_HPP_
#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ledger_namespace {
struct time {
    int hour;
    int minute;

    time(int h, int m);
    time(std::string &string);
    [[nodiscard]] std::string to_string() const;
    time &operator+=(const time &other);

    friend time operator-(const time &lhs, const time &rhs) {
        time answer(0, 0);
        answer.hour = lhs.hour - rhs.hour;
        if (lhs.minute - rhs.minute < 0) {
            answer.hour -= 1;
        }
        answer.minute = ((60 - rhs.minute) + lhs.minute) % 60;
        return answer;
    }
};

class ledger {
public:
    ledger(int sit_count, int hour_price, time start_time, time end_time);
    void arrived(std::vector<std::string> &commands);
    void sit(std::vector<std::string> &commands);
    void wait(std::vector<std::string> &commands);
    void left(std::vector<std::string> &commands);
    void kick_out();
    void earnings();

private:
    void refresh_queue(time time, int freed_number);
    bool check_time(time time) const;
    bool client_contains(std::string &client_name) const;
    bool check_client_contains(time time, std::string &client_name, bool flag)
        const;
    bool check_free_place(time time) const;
    bool check_place(time time, int number) const;
    void client_left(time time, std::string &client_name);

    const int hour_price_;
    const int sit_count_;
    const time start_time_;
    const time end_time_;
    int busy_table;
    std::unordered_map<std::string, int> clients_to_tables;
    std::unordered_set<std::string> client_in_queue;
    std::queue<std::string> queue;
    std::vector<time> tables;
    std::vector<time> table_timer;
    std::vector<int> table_earn;
};
}  // namespace ledger_namespace
#endif  // LEDGER_HPP_