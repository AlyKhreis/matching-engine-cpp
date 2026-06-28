#include <iostream>
#include "order.h"
#include "order_book.h"
#include "matching_engine.h"




void print_trades(const MatchingEngine& engine) {
    std::cout << "Trades:\n";
    for (const auto& t : engine.get_trade_log()) {
        std::cout << "  buy=" << t.buy_order_id << " sell=" << t.sell_order_id
                  << " price=" << t.price << " qty=" << t.quantity << "\n";
    }
}

int main() {
    // ---------- TEST 1: Crossing limit BUY fills two ask levels ----------
    {
        std::cout << "\n========== TEST 1: Crossing limit BUY ==========\n";
        OrderBook book(100000);
        MatchingEngine engine(&book);

        Order* o1 = new Order{1, Side::BUY,  OrderType::LIMIT, 14990, 100, 1, nullptr, nullptr};
        Order* o2 = new Order{2, Side::BUY,  OrderType::LIMIT, 14985, 200, 2, nullptr, nullptr};
        Order* o3 = new Order{3, Side::SELL, OrderType::LIMIT, 15010, 150, 3, nullptr, nullptr};
        Order* o4 = new Order{4, Side::SELL, OrderType::LIMIT, 15015,  50, 4, nullptr, nullptr};
        engine.process_order(o1);
        engine.process_order(o2);
        engine.process_order(o3);
        engine.process_order(o4);

        Order* o5 = new Order{5, Side::BUY, OrderType::LIMIT, 15015, 200, 5, nullptr, nullptr};
        engine.process_order(o5);

        std::cout << "Book after BUY 200 @ 15015:\n";
        book.print_book();
        print_trades(engine);

        delete o1; delete o2; delete o3; delete o4; delete o5;
    }

    // ---------- TEST 2: Partial fill ----------
    {
        std::cout << "\n========== TEST 2: Partial fill ==========\n";
        OrderBook book(100000);
        MatchingEngine engine(&book);

        Order* r1 = new Order{10, Side::BUY,  OrderType::LIMIT, 15000, 100, 10, nullptr, nullptr};
        engine.process_order(r1);

        Order* s1 = new Order{11, Side::SELL, OrderType::LIMIT, 15000, 60, 11, nullptr, nullptr};
        engine.process_order(s1);

        std::cout << "Book after partial fill (resting 100 - 60 = 40):\n";
        book.print_book();
        print_trades(engine);
        std::cout << "Best bid: " << book.best_bid() << "\n";

        delete r1; delete s1;
    }

    // ---------- TEST 3: Market order sweeps liquidity ----------
    {
        std::cout << "\n========== TEST 3: Market SELL sweeps bids ==========\n";
        OrderBook book(100000);
        MatchingEngine engine(&book);

        Order* b1 = new Order{20, Side::BUY, OrderType::LIMIT, 15000, 100, 20, nullptr, nullptr};
        Order* b2 = new Order{21, Side::BUY, OrderType::LIMIT, 14995, 200, 21, nullptr, nullptr};
        Order* b3 = new Order{22, Side::BUY, OrderType::LIMIT, 14990, 150, 22, nullptr, nullptr};
        engine.process_order(b1);
        engine.process_order(b2);
        engine.process_order(b3);

        // Market SELL for 250 shares - should consume b1 fully + 150 of b2
        Order* m1 = new Order{23, Side::SELL, OrderType::MARKET, 0, 250, 23, nullptr, nullptr};
        engine.process_order(m1);

        std::cout << "Book after MARKET SELL 250:\n";
        book.print_book();
        print_trades(engine);

        delete b1; delete b2; delete b3; delete m1;
    }

    // ---------- TEST 4: Non-crossing limit just rests ----------
    {
        std::cout << "\n========== TEST 4: Non-crossing limit BUY rests ==========\n";
        OrderBook book(100000);
        MatchingEngine engine(&book);

        Order* a1 = new Order{30, Side::SELL, OrderType::LIMIT, 15010, 100, 30, nullptr, nullptr};
        engine.process_order(a1);

        // Bid at 14000 is well below best ask 15010 - should not cross
        Order* b1 = new Order{31, Side::BUY, OrderType::LIMIT, 14000, 50, 31, nullptr, nullptr};
        engine.process_order(b1);

        std::cout << "Book (BUY at 14000 should rest, no trades):\n";
        book.print_book();
        print_trades(engine);

        delete a1; delete b1;
    }
    return 0;
}