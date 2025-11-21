#ifndef GOMOKU_SEARCH_EVALUATOR_H
#define GOMOKU_SEARCH_EVALUATOR_H

#include "core/board.h"
#include "search/search_types.h"

namespace gomoku {

// Abstract interface for evaluation strategies.
class IEvaluator {
public:
    virtual ~IEvaluator() = default;

    // The evaluation must be:
    //   * Fast (called at leaf nodes and sometimes in quiescence).
    //   * Side-aware: score is always from the perspective of "maximizing" side.
    //   * maxPlayer: The player for whom positive scores are favorable.
    virtual EvalScore evaluate(const Board& board, Player maxPlayer) = 0;
};

} // namespace gomoku

#endif // GOMOKU_SEARCH_EVALUATOR_H