#ifndef MATCHING_ENGINE_CPP_TRADE_H
#define MATCHING_ENGINE_CPP_TRADE_H
#include <cstdint>

struct Trade {
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    uint64_t price; //execution price in ticks
    uint32_t quantity; //shares traded
    uint64_t timestamp; //when it happened
};

#endif
