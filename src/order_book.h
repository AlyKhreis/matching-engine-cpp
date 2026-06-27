#ifndef MATCHING_ENGINE_CPP_ORDER_BOOK_H
#define MATCHING_ENGINE_CPP_ORDER_BOOK_H

#include <cstdint>
#include <unordered_map>
#include "order.h"
#include "price_level.h"

class OrderBook {
public:
    explicit OrderBook(uint64_t max_price_ticks);
    ~OrderBook();

    void add_order(Order* o);
    void cancel_order(uint64_t order_id);

    uint64_t best_bid() const;   // returns 0 if no bids
    uint64_t best_ask() const;   // returns 0 if no asks

    void print_book(int depth = 5) const;  // debug helper

    OrderBook(const OrderBook&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;
private:
    uint64_t max_price_ticks_;
    PriceLevel* bid_levels_;     // array, size max_price_ticks_
    PriceLevel* ask_levels_;     // array, size max_price_ticks_

    std::unordered_map<uint64_t, Order*> id_map_;

    uint64_t best_bid_tick_;     // cached
    uint64_t best_ask_tick_;     // cached
};


#endif //MATCHING_ENGINE_CPP_ORDER_BOOK_H
