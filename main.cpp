#include <iostream>
#include "order.h"
#include "order_book.h"
#include "matching_engine.h"

int main() {
    OrderBook book(100000);
    MatchingEngine engine(&book);

    // Resting orders
    Order* o1 = new Order{1, Side::BUY,  OrderType::LIMIT, 14990, 100, 1, nullptr, nullptr};
    Order* o2 = new Order{2, Side::BUY,  OrderType::LIMIT, 14985, 200, 2, nullptr, nullptr};
    Order* o3 = new Order{3, Side::SELL, OrderType::LIMIT, 15010, 150, 3, nullptr, nullptr};
    Order* o4 = new Order{4, Side::SELL, OrderType::LIMIT, 15015,  50, 4, nullptr, nullptr};

    engine.process_order(o1);
    engine.process_order(o2);
    engine.process_order(o3);
    engine.process_order(o4);

    std::cout << "Book after resting orders:\n";
    book.print_book();

    // Aggressive BUY that crosses both ask levels (limit at 15015 for 200 shares)
    Order* o5 = new Order{5, Side::BUY, OrderType::LIMIT, 15015, 200, 5, nullptr, nullptr};
    engine.process_order(o5);

    std::cout << "\nBook after BUY 200 @ 15015:\n";
    book.print_book();

    std::cout << "\nTrades:\n";
    for (const auto& t : engine.get_trade_log()) {
        std::cout << "  buy=" << t.buy_order_id << " sell=" << t.sell_order_id
                  << " price=" << t.price << " qty=" << t.quantity << "\n";
    }

    delete o1; delete o2; delete o3; delete o4; delete o5;
    return 0;
}