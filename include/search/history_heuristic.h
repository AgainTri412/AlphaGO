#pragma once

#include "core/board.h"

namespace gomoku {

// Interface for history heuristic; tracks move-ordering scores. Implementations
// are non-owning and not thread-safe.
class IHistoryHeuristic {
public:
    virtual ~IHistoryHeuristic() = default;

    virtual int  getHistoryScore(Player sideToMove, const Move& move) const = 0;
    virtual void recordBetaCutoff(Player sideToMove, const Move& move, int depth) = 0;
    virtual void recordPVMove(Player sideToMove, const Move& move, int depth) = 0;
    virtual void clear() = 0;
};

} // namespace gomoku

