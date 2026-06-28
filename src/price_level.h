// price_level.h
#ifndef PRICE_LEVEL_H
#define PRICE_LEVEL_H

#include <cstdint>
#include "order.h"

class PriceLevel {
public:
    PriceLevel() : price_(0), head_(nullptr), tail_(nullptr), total_quantity_(0) {}
    explicit PriceLevel(uint64_t price)
        : price_(price), head_(nullptr), tail_(nullptr), total_quantity_(0) {}

    void add_order(Order* o);
    void remove_order(Order* o);
    bool is_empty() const { return head_ == nullptr; }

    uint64_t price() const { return price_; }
    uint64_t total_quantity() const { return total_quantity_; }
    Order* head() const { return head_; }
    void reduce_total(uint64_t delta) { total_quantity_ -= delta; }
private:
    uint64_t price_;
    Order* head_;
    Order* tail_;
    uint64_t total_quantity_;
};

#endif