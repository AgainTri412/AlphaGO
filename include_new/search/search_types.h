#pragma once

#include <cstdint>
#include <limits>
#include <vector>

#include "core/board.h"

namespace gomoku {

using EvalScore = int;
static constexpr EvalScore kInfinity  = std::numeric_limits<EvalScore>::max() / 4;
static constexpr EvalScore kMateScore = kInfinity - 1000; // used for immediate wins
static constexpr EvalScore kDrawScore = 0;

struct SearchLimits {
    int           maxDepth = 32;
    std::uint64_t maxNodes = 0; // 0 = unlimited
    std::uint64_t timeLimitMs = 1000;
    std::uint64_t panicExtraTimeMs = 300;
    bool          enableNullMove = true;
    bool          enablePanicMode = true;
};

struct SearchResult {
    Move       bestMove;
    EvalScore  bestScore = 0; // always from the perspective of rootSideToMove at search start
    int        depthReached = 0;
    bool       isMate = false;
    bool       isTimeout = false;
    bool       isForcedWin = false;

    std::vector<Move> principalVariation;

    std::uint64_t nodes = 0;
    std::uint64_t qnodes = 0;
    std::uint64_t hashHits = 0;
};

// RAII helper to ensure every makeMove is undone even on exceptions/returns.
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

