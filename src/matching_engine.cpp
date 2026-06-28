#include "matching_engine.h"
#include <algorithm>

MatchingEngine::MatchingEngine(OrderBook* book) : book_(book) {}

void MatchingEngine::process_order(Order* o) {
    if (o->side == Side::BUY) {
        // Match against asks
        while (o->quantity > 0
               && book_->best_ask() != 0
               && (o->type == OrderType::MARKET || book_->best_ask() <= o->price)) {

            uint64_t best_ask_price = book_->best_ask();
            PriceLevel* lvl = book_->ask_level_at(best_ask_price);

            while (o->quantity > 0) {
                Order* resting = lvl->head();
                if (resting == nullptr) break;

                uint32_t fill_qty = std::min(o->quantity, resting->quantity);

                Trade t;
                t.buy_order_id = o->order_id;
                t.sell_order_id = resting->order_id;
                t.price = resting->price;
                t.quantity = fill_qty;
                t.timestamp = o->timestamp;
                trade_log_.push_back(t);

                o->quantity -= fill_qty;
                resting->quantity -= fill_qty;
                lvl->reduce_total(fill_qty);

                if (resting->quantity == 0) {
                    book_->cancel_order(resting->order_id);
                }
            }
        }
    } else {
        // SELL: match against bids
        while (o->quantity > 0
               && book_->best_bid() != 0
               && (o->type == OrderType::MARKET || book_->best_bid() >= o->price)) {

            uint64_t best_bid_price = book_->best_bid();
            PriceLevel* lvl = book_->bid_level_at(best_bid_price);

            while (o->quantity > 0) {
                Order* resting = lvl->head();
                if (resting == nullptr) break;

                uint32_t fill_qty = std::min(o->quantity, resting->quantity);

                Trade t;
                t.buy_order_id = resting->order_id;
                t.sell_order_id = o->order_id;
                t.price = resting->price;
                t.quantity = fill_qty;
                t.timestamp = o->timestamp;
                trade_log_.push_back(t);

                o->quantity -= fill_qty;
                resting->quantity -= fill_qty;
                lvl->reduce_total(fill_qty);
                if (resting->quantity == 0) {
                    book_->cancel_order(resting->order_id);
                }
            }
        }
    }

    // Rest the remainder on the book if it's a limit order
    if (o->quantity > 0 && o->type == OrderType::LIMIT) {
        book_->add_order(o);
    }
    // MARKET orders with leftover are discarded
}