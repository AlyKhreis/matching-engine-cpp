#ifndef ORDER_H
#define ORDER_H
#include <cstdint>

enum class Side { BUY, SELL };
enum class OrderType { LIMIT, MARKET };

class Order {
    uint64_t order_id;
    Side side;
    OrderType type;
    uint64_t price;       // in ticks
    uint32_t quantity;    // shares remaining
    uint64_t timestamp;   // nanoseconds, for FIFO at same price level

    Order* next = nullptr;
    Order* prev = nullptr;
};



#endif
