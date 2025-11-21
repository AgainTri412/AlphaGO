#ifndef GOMOKU_SEARCH_TYPES_H
#define GOMOKU_SEARCH_TYPES_H

#include <cstdint>
#include <vector>
#include <limits>
#include "core/board.h"

namespace gomoku {

// Scoring type.
using EvalScore = int;

// Constants used across the search implementation.
static constexpr EvalScore kInfinity   = std::numeric_limits<EvalScore>::max() / 4;
static constexpr EvalScore kMateScore  = kInfinity - 1000;  // Score for immediate win.
static constexpr EvalScore kDrawScore  = 0;

// Search limits control how far and how long search is allowed to go.
struct SearchLimits {
    int maxDepth = 32;
    std::uint64_t maxNodes = 0;
    std::uint64_t timeLimitMs = 1000;
    std::uint64_t panicExtraTimeMs = 300;
    bool enableNullMove = true;
    bool enablePanicMode = true;
};

// Detailed result of a single move search.
struct SearchResult {
    Move       bestMove;         // Selected root move.
    EvalScore  bestScore = 0;    // Score from perspective of rootSideToMove (fixed at start of search).
    int        depthReached = 0; // Deepest fully completed iteration.
    bool       isMate = false;   // True if bestScore indicates a forced win/loss.
    bool       isTimeout = false;// True if search stopped due to time.
    bool       isForcedWin = false; // True if ThreatSolver proved a win at root.

    std::vector<Move> principalVariation;

    std::uint64_t nodes = 0;
    std::uint64_t qnodes = 0;
    std::uint64_t hashHits = 0;
};

// RAII helper for make/unmake move safety.
class MoveGuard {
public:
    MoveGuard(Board& board, const Move& move)
        : board_(board), move_(move), valid_(board_.makeMove(move.x, move.y)) {}

    ~MoveGuard() {
        if (valid_) {
            board_.unmakeMove(move_.x, move_.y);
        }
    }

    bool isValid() const { return valid_; }

private:
    Board& board_;
    Move   move_;
    bool   valid_;
};

} // namespace gomoku

#endif // GOMOKU_SEARCH_TYPES_H