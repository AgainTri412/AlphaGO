#ifndef GOMOKU_SEARCH_HISTORY_HEURISTIC_H
#define GOMOKU_SEARCH_HISTORY_HEURISTIC_H

#include "core/board.h"

namespace gomoku {

// Interface for history heuristic and move ordering statistics.
class IHistoryHeuristic {
public:
    virtual ~IHistoryHeuristic() = default;

   // Return a heuristic score for a move (larger = more promising).
    virtual int getHistoryScore(Player sideToMove, const Move& move) const = 0;

    // Called when a move causes a beta cutoff at some depth.
    virtual void recordBetaCutoff(Player sideToMove, const Move& move, int depth) = 0;

    // Called for PV moves at each depth to slightly boost them.
    virtual void recordPVMove(Player sideToMove, const Move& move, int depth) = 0;

    // Utility: allow the search engine to clear history between games.
    virtual void clear() = 0;
};

} // namespace gomoku

#endif // GOMOKU_SEARCH_HISTORY_HEURISTIC_H