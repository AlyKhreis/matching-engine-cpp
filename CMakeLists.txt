cmake_minimum_required(VERSION 3.20)
project(matching_engine)


set(CMAKE_CXX_STANDARD 17)

include_directories(src)


add_executable(matching_engine
        main.cpp
        src/order_book.cpp
        src/price_level.cpp
        src/matching_engine.cpp
        src/matching_engine.h
)