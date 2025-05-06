#define TAPEIMPL_HPP_
#ifdef TAPEIMPL_HPP_
#include <chrono>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include "TapeInterface.hpp"

struct TapeConfig {
    std::chrono::milliseconds read_delay_;
    std::chrono::milliseconds write_delay_;
    std::chrono::milliseconds shift_delay_;
    std::chrono::milliseconds to_start_delay_;

    TapeConfig(
        int read_delay = 0,
        int write_delay = 0,
        int shift_delay = 0,
        int to_start_delay = 0
    )
        : read_delay_(read_delay),
          write_delay_(write_delay),
          shift_delay_(shift_delay),
          to_start_delay_(to_start_delay) {
    }
};

class TapeImplementation : public TapeInterface {
    size_t index = 0;
    size_t last_index = 0;
    std::fstream tape;
    TapeConfig config;

public:
    TapeImplementation(
        std::string &input_file,
        std::string &config_file,
        std::string &mode
    );
    ~TapeImplementation();
    TapeImplementation(TapeImplementation &tape) = delete;
    TapeImplementation(TapeImplementation &&tape) = delete;
    TapeImplementation &operator=(TapeImplementation &tape) = delete;
    TapeImplementation &operator=(TapeImplementation &&tape) = delete;
    void read(int &value) override;
    void write(int object) override;
    void shift_l() override;
    void shift_r() override;
    void to_start() override;
    void insert_value(int index) override;
    bool is_eof() override;
};
#endif  // TAPEIMPL_HPP_