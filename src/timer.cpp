#ifndef TIMER
#define TIMER
#include <chrono>
#include <iostream>

class Timer {
    using clock = std::chrono::high_resolution_clock;
    clock::time_point start_time;
public:
    Timer() : start_time(clock::now()) {}
    int now() {
        auto end_time = clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }
    ~Timer() {
        auto end_time = clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Time Taken: " << duration.count() << " ms" << std::endl;
    }
};
#endif
