#pragma once

#include "core/board.h"
#include "search/search_types.h"

namespace gomoku {

// Interface for static evaluation. Implementations should not modify the board.
// Scores must be returned from the perspective of maxPlayer (positive = good).
class IEvaluator {
public:
    virtual ~IEvaluator() = default;

    virtual EvalScore evaluate(const Board& board, Player maxPlayer) = 0;
};

} // namespace gomoku

