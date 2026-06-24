#include "price_level.h"


// price_level.cpp
#include "price_level.h"

void PriceLevel::add_order(Order* o) {
    o->next = nullptr;
    o->prev = tail_;

    if (tail_ == nullptr) {
        // list was empty
        head_ = o;
    } else {
        tail_->next = o;
    }
    tail_ = o;

    total_quantity_ += o->quantity;
}

void PriceLevel::remove_order(Order* o) {
    // patch neighbors
    if (o->prev != nullptr) {
        o->prev->next = o->next;
    } else {
        // o was the head
        head_ = o->next;
    }

    if (o->next != nullptr) {
        o->next->prev = o->prev;
    } else {
        // o was the tail
        tail_ = o->prev;
    }

    // clear the removed order's links (defensive)
    o->prev = nullptr;
    o->next = nullptr;

    total_quantity_ -= o->quantity;
}