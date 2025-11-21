#pragma once

#include <vector>

#include "core/board.h"

namespace gomoku {

struct ThreatAnalysis {
    bool attackerHasForcedWin = false;
    Move firstWinningMove{};
    std::vector<Move> winningLine;   // attacker-first sequence when forced win exists
    std::vector<Move> defensiveMoves; // safe defenses when no forced win; empty if lost
};

// Interface for threat search. Does not own the board; not thread-safe.
class IThreatSolver {
public:
    virtual ~IThreatSolver() = default;

    // Analyzes threats for attacker on the given board (board should match internal state).
    virtual ThreatAnalysis analyzeThreats(const Board& board, Player attacker) = 0;
    // Incremental update hooks to keep internal rotated bitboards in sync with Board.
    virtual void notifyMove(const Move& move) = 0;
    virtual void notifyUndo(const Move& move) = 0;
};

} // namespace gomoku

