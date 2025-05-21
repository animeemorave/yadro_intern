#include "doctest.h"
#include "ledger.hpp"
#include <sstream>
#include <vector>

TEST_CASE("Time test") {
    SUBCASE("Valid time parsing") {
        ledger::Time t1("09:00");
        ledger::Time t2("23:59");
        CHECK(t1.hours == 9);
        CHECK(t1.minutes == 0);        
        CHECK(t2.hours == 23);
        CHECK(t2.minutes == 59);
    }

    SUBCASE("Invalid time parsing") {
        CHECK_THROWS(ledger::Time("24:00"));
        CHECK_THROWS(ledger::Time("12:60"));
        CHECK_THROWS(ledger::Time("ab:ec"));
        CHECK_THROWS(ledger::Time("9:14"));
        CHECK_THROWS(ledger::Time("09:4"));
        CHECK_THROWS(ledger::Time("009:4"));
        CHECK_THROWS(ledger::Time("9:004"));
    }

    SUBCASE("Time comparisons") {
        ledger::Time t1("09:30");
        ledger::Time t2("10:00");
        CHECK(t1 == ledger::Time(9,30));
        CHECK(t2 == ledger::Time(10,0));
        CHECK(t1 < t2);
        CHECK_FALSE(t2 < t1);
    }

    SUBCASE("Time arithmetic") {
        ledger::Time t1(1, 30);
        ledger::Time t2(2, 45);
        ledger::Time diff = t2 - t1;
        CHECK(diff.hours == 1);
        CHECK(diff.minutes == 15);
        t1 += ledger::Time(0, 45);
        CHECK(t1.hours == 2);
        CHECK(t1.minutes == 15);
        t1 += ledger::Time(0, 45);
        CHECK(t1.hours == 3);
        CHECK(t1.minutes == 0);
    }
}

