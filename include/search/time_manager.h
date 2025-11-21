#ifndef GOMOKU_SEARCH_TIME_MANAGER_H
#define GOMOKU_SEARCH_TIME_MANAGER_H

#include <chrono>
#include <cstdint>
#include "search/search_types.h"

namespace gomoku {

class TimeManager {
public:
    using Clock = std::chrono::steady_clock;

    // Initialize with limits for a new search.
    void start(const SearchLimits& limits);

    // Check if search should stop.
    // updates internal stop flag.
    // @param nodesVisited: accumulated nodes + qnodes.
    // @param inPanic: if true, uses panicExtraTimeMs.
    bool checkStopCondition(std::uint64_t nodesVisited, bool inPanic = false);

    bool isStopped() const { return stop_; }
    
    std::uint64_t elapsedMs() const;

private:
    Clock::time_point startTime_;
    SearchLimits      limits_;
    bool              stop_ = false;
};

} // namespace gomoku

#endif // GOMOKU_SEARCH_TIME_MANAGER_H