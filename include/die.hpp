#ifndef TTC_DIE_HPP
#define TTC_DIE_HPP

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

#define die(msg) \
    std::cerr << "FATAL: " << msg << std::endl; \
    std::this_thread::sleep_for(std::chrono::seconds(5)); \
    std::exit(1);

#endif