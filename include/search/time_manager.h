#pragma once

#include <chrono>
#include <cstdint>

#include "search/search_types.h"

namespace gomoku {

// Simple per-search time manager. Not thread-safe.
class TimeManager {
public:
    using Clock = std::chrono::steady_clock;

    // Starts a new timer using the provided limits; resets internal stop flag.
    void start(const SearchLimits& limits);
    // Returns true when time/node budget suggests aborting search. inPanic allows
    // tighter limits after the main deadline passes.
    bool checkStopCondition(std::uint64_t nodesVisited, bool inPanic = false);

    bool        isStopped() const { return stop_; }
    std::uint64_t elapsedMs() const;

private:
    Clock::time_point startTime_{};
    SearchLimits      limits_{};
    bool              stop_ = false;
};

} // namespace gomoku

