#ifndef MATCHING_ENGINE_MATCHING_ENGINE_H
#define MATCHING_ENGINE_MATCHING_ENGINE_H
#include "order_book.h"
#include "order.h"
#include "trade.h"
#include <vector>


class MatchingEngine {
public:
    explicit MatchingEngine(OrderBook *book);
    void process_order(Order *o);
    const std::vector<Trade> &get_trade_log() const { return trade_log_; }
private:
    OrderBook *book_;
    std::vector<Trade> trade_log_;

};



#endif //MATCHING_ENGINE_MATCHING_ENGINE_H
