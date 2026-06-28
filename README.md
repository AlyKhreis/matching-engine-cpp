# Limit Order Book & Matching Engine (C++)

A single-symbol limit order book and matching engine in C++17, built around HFT-style data structures: a flat price-level array for O(1) price lookups, intrusive doubly-linked lists per level for O(1) order insertion and cancellation, and a hash map from order ID to order pointer for O(1) lookup on cancel. Benchmarked at ~9.6M ops/sec with sub-microsecond p99 latency on synthetic order flow.

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                       MATCHING ENGINE                            │
│              process_order(), trade_log                          │
└─────────────────────────────────────────────────────────────────┘
                                │
                       process_order
                                ▼
                       ┌──────────────────┐
                       │    OrderBook     │
                       │                  │
                       │  bid_levels[]    │
                       │  ask_levels[]    │
                       │  id_map          │
                       │  best_bid_tick   │
                       │  best_ask_tick   │
                       └──────────────────┘
                                │
                       owns array of
                                ▼
                       ┌──────────────────┐
                       │   PriceLevel     │
                       │                  │
                       │  head ──────────┼──► Order ⇄ Order ⇄ Order
                       │  tail            │
                       │  total_quantity  │
                       └──────────────────┘

   MatchingEngine emits → Trade {buy_id, sell_id, price, qty}
```

Incoming orders flow through `MatchingEngine::process_order`. The engine checks whether the order crosses the opposite side of the book and, if so, walks the FIFO of orders at the best opposite price, fills as much as possible, and emits a `Trade` for each fill. Any non-crossing limit order is appended to its price level on the book. Market orders with leftover quantity after sweeping liquidity are discarded.

The book is decoupled from the engine. `OrderBook` exposes `add_order`, `cancel_order`, `best_bid`, `best_ask`, and direct access to individual price levels — but never matches orders itself. This separation makes it easy to plug different matching strategies (pro-rata, iceberg, etc.) without touching the book.

## Data Structures

The performance story lives in three choices, all deliberate:

| Concern | Naive choice | Used here | Reason |
|---|---|---|---|
| Find level at price | `std::map<price, level>` (red-black tree, O(log n)) | Flat array `PriceLevel levels[MAX_TICKS]` indexed by tick (O(1)) | Real exchanges use direct addressing; tree lookups are too slow in the hot path |
| Orders at a price | `std::queue` or `std::deque` (heap allocation per push) | Intrusive doubly-linked list with `next/prev` pointers inside `Order` | Removing an arbitrary order is O(1) given just its pointer; no extra allocation per insert |
| Cancel by order ID | Linear scan of the side | `std::unordered_map<uint64_t, Order*>` | O(1) lookup, then O(1) unlink from the intrusive list |

Prices are stored as integer **ticks** (e.g., $150.25 → 15025), never doubles. Floating-point comparison is slow and lossy; exchanges don't roll dice on price math.

## Build

Requires CMake 3.20+ and a C++17 compiler.

\`\`\`bash
git clone https://github.com/AlyKhreis/matching-engine-cpp.git
cd matching-engine-cpp
mkdir cmake-build-release && cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./matching_engine
\`\`\`

## Usage

`main.cpp` ships with a benchmark harness that:

1. Generates 1,000,000 synthetic orders (random side, random price near a mid, random quantity)
2. Feeds them through `MatchingEngine::process_order` one by one
3. Records per-order latency in nanoseconds
4. Reports throughput and p50/p99/p999 latency

\`\`\`
Processed 1000000 orders in 104293 us
Throughput: 9.58837e+06 ops/sec
Latency p50:  42 ns
Latency p99:  292 ns
Latency p999: 1417 ns
Trades emitted: 775162
\`\`\`

To run as a library instead of the benchmark, replace `main.cpp` with your own driver. The `OrderBook` and `MatchingEngine` classes are usable in any context.

## Performance

Benchmarked on an Apple M-series Mac, Release build (`-O2`), single-threaded, 1M random orders within a tight price band so most orders cross.

| Metric | Value |
|---|---|
| Throughput | **~9.6M ops/sec** |
| p50 latency | **42 ns** |
| p99 latency | **292 ns** |
| p999 latency | **1.4 µs** |
| Trades emitted | ~775K |

For perspective:
- p50 of 42ns is roughly 1.5x the latency of an L2 cache miss
- p99 under 300ns means even the 1% slowest orders complete in less time than a function call through a vtable on a typical x86 CPU
- Throughput is in the same range as production single-threaded matching cores

The benchmark intentionally captures per-order latency, which adds 2x `chrono::high_resolution_clock` calls per op — real throughput without instrumentation is higher.

## What's Modeled

- **Price-time priority** FIFO matching across all price levels
- **Limit orders** (rest on the book if non-crossing, match if crossing)
- **Market orders** (sweep liquidity at any price; leftover is discarded)
- **Partial fills** (incoming consumes part of a resting order, remainder stays at price)
- **O(1) order cancellation** by ID
- **Top-of-book maintenance** via cached best-bid/best-ask ticks with walk-back when a level empties
- **Trade emission** with buy/sell order IDs, execution price, and quantity

## What's Not Modeled

- **Multi-symbol** — single instrument only; multi-symbol would wrap each `OrderBook` in an outer map keyed by symbol
- **Stop/iceberg/IOC/FOK order types** — only standard limit and market
- **Pro-rata or other matching algorithms** — strict price-time priority only
- **Self-trade prevention** — buyer and seller IDs aren't compared
- **Order modification** — only add and cancel; modifications would be implemented as cancel + new
- **Persistence** — book is in-memory only, lost on shutdown
- **Network layer** — engine is a library, not a server; FIX/ITCH integration would sit in a separate process feeding events in

