#pragma once

#include <chrono>
#include <cstdint>

#include "search/search_types.h"

namespace gomoku {

class TimeManager {
public:
    using Clock = std::chrono::steady_clock;

    void start(const SearchLimits& limits);
    bool checkStopCondition(std::uint64_t nodesVisited, bool inPanic = false);

    bool        isStopped() const { return stop_; }
    std::uint64_t elapsedMs() const;

private:
    Clock::time_point startTime_{};
    SearchLimits      limits_{};
    bool              stop_ = false;
};

} // namespace gomoku

