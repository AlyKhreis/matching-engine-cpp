#include <iostream>
#include "order.h"
#include "order_book.h"
int main() {
    OrderBook book(100000);

    Order* o1 = new Order{1, Side::BUY,  OrderType::LIMIT, 14990, 100, 1, nullptr, nullptr};
    Order* o2 = new Order{2, Side::BUY,  OrderType::LIMIT, 14985, 200, 2, nullptr, nullptr};
    Order* o3 = new Order{3, Side::SELL, OrderType::LIMIT, 15010, 150, 3, nullptr, nullptr};
    Order* o4 = new Order{4, Side::SELL, OrderType::LIMIT, 15015,  50, 4, nullptr, nullptr};

    book.add_order(o1);
    book.add_order(o2);
    book.add_order(o3);
    book.add_order(o4);

    std::cout << "After adding 4 orders:\n";
    book.print_book();
    std::cout << "Best bid: " << book.best_bid() << "\n";
    std::cout << "Best ask: " << book.best_ask() << "\n\n";

    book.cancel_order(1);

    std::cout << "After cancelling order 1 (top bid):\n";
    book.print_book();
    std::cout << "Best bid: " << book.best_bid() << "\n";
    std::cout << "Best ask: " << book.best_ask() << "\n";

    delete o1; delete o2; delete o3; delete o4;
    return 0;

}
