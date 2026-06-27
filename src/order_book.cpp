// order_book.cpp
#include "order_book.h"
#include <iostream>

OrderBook::OrderBook(uint64_t max_price_ticks)
    : max_price_ticks_(max_price_ticks),
      bid_levels_(new PriceLevel[max_price_ticks]),
      ask_levels_(new PriceLevel[max_price_ticks]),
      best_bid_tick_(0),
      best_ask_tick_(0)
{}

OrderBook::~OrderBook() {
    delete[] bid_levels_;
    delete[] ask_levels_;
}

void OrderBook::add_order(Order* o) {
    if (o->price >= max_price_ticks_) return;

    if (o->side == Side::BUY) {
        bid_levels_[o->price].add_order(o);
        if (o->price > best_bid_tick_) best_bid_tick_ = o->price;
    } else {
        ask_levels_[o->price].add_order(o);
        if (best_ask_tick_ == 0 || o->price < best_ask_tick_) best_ask_tick_ = o->price;
    }

    id_map_[o->order_id] = o;
}

void OrderBook::cancel_order(uint64_t order_id) {
    auto it = id_map_.find(order_id);
    if (it == id_map_.end()) return;
    Order* o = it->second;

    PriceLevel* side_array = (o->side == Side::BUY) ? bid_levels_ : ask_levels_;
    side_array[o->price].remove_order(o);
    id_map_.erase(order_id);

    if (o->side == Side::BUY && o->price == best_bid_tick_) {
        if (bid_levels_[o->price].is_empty()) {
            uint64_t p = best_bid_tick_;
            while (p > 0 && bid_levels_[p].is_empty()) {
                p--;
            }
            best_bid_tick_ = bid_levels_[p].is_empty() ? 0 : p;
        }
    }

    if (o->side == Side::SELL && o->price == best_ask_tick_) {
        if (ask_levels_[o->price].is_empty()) {
            uint64_t p = best_ask_tick_;
            while (p < max_price_ticks_ && ask_levels_[p].is_empty()) {
                p++;
            }
            best_ask_tick_ = (p >= max_price_ticks_) ? 0 : p;
        }
    }
}

uint64_t OrderBook::best_bid() const { return best_bid_tick_; }
uint64_t OrderBook::best_ask() const { return best_ask_tick_; }

void OrderBook::print_book(int depth) const {
    std::cout << "===== ORDER BOOK =====\n";
    std::cout << "ASKS:\n";
    int count = 0;
    for (uint64_t p = best_ask_tick_; p < max_price_ticks_ && count < depth; p++) {
        if (!ask_levels_[p].is_empty()) {
            std::cout << "  " << p << "  x  " << ask_levels_[p].total_quantity() << "\n";
            count++;
        }
    }
    std::cout << "BIDS:\n";
    count = 0;
    for (uint64_t p = best_bid_tick_; p > 0 && count < depth; p--) {
        if (!bid_levels_[p].is_empty()) {
            std::cout << "  " << p << "  x  " << bid_levels_[p].total_quantity() << "\n";
            count++;
        }
    }
    std::cout << "======================\n";
}
