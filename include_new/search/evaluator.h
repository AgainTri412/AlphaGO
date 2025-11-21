#pragma once

#include "core/board.h"
#include "search/search_types.h"

namespace gomoku {

// Interface for static evaluation. Implementations should not modify the board.
// Scores must be returned from the perspective of maxPlayer (positive = good for maxPlayer).
// Implementations may keep internal caches; they must remain valid for the lifetime of
// the IEvaluator instance and are not required to be thread-safe.
class IEvaluator {
public:
    virtual ~IEvaluator() = default;

    virtual EvalScore evaluate(const Board& board, Player maxPlayer) = 0;
};

} // namespace gomoku

