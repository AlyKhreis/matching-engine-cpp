#include <iostream>
#include "order.h"
#include "order_book.h"
#include "matching_engine.h"
#include <chrono>
#include <random>
#include <algorithm>
#include <vector>




int main() {
     OrderBook book(100000);
    MatchingEngine engine(&book);

    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int> side_dist(0, 1);
    std::uniform_int_distribution<uint64_t> price_dist(14900, 15100);
    std::uniform_int_distribution<uint32_t> qty_dist(1, 100);

    const int N = 1'000'000;
    std::vector<Order*> orders;
    orders.reserve(N);

    // Generate all orders upfront (separate from the hot loop)
    for (int i = 0; i < N; i++) {
        Side s = (side_dist(rng) == 0) ? Side::BUY : Side::SELL;
        uint64_t price = price_dist(rng);
        uint32_t qty = qty_dist(rng);
        Order* o = new Order{
            static_cast<uint64_t>(i + 1),
            s,
            OrderType::LIMIT,
            price,
            qty,
            static_cast<uint64_t>(i),
            nullptr, nullptr
        };
        orders.push_back(o);
    }

    // Latency tracking
    std::vector<uint64_t> latencies_ns;
    latencies_ns.reserve(N);

    auto t_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        auto t0 = std::chrono::high_resolution_clock::now();
        engine.process_order(orders[i]);
        auto t1 = std::chrono::high_resolution_clock::now();
        latencies_ns.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count()
        );
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();

    double ops_per_sec = N / (total_us / 1'000'000.0);

    // Compute percentiles
    std::sort(latencies_ns.begin(), latencies_ns.end());
    auto p50 = latencies_ns[N * 50 / 100];
    auto p99 = latencies_ns[N * 99 / 100];
    auto p999 = latencies_ns[N * 999 / 1000];

    std::cout << "Processed " << N << " orders in " << total_us << " us\n";
    std::cout << "Throughput: " << ops_per_sec << " ops/sec\n";
    std::cout << "Latency p50:  " << p50 << " ns\n";
    std::cout << "Latency p99:  " << p99 << " ns\n";
    std::cout << "Latency p999: " << p999 << " ns\n";
    std::cout << "Trades emitted: " << engine.get_trade_log().size() << "\n";

    for (auto* o : orders) delete o;


    return 0;
}