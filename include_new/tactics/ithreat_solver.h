#pragma once

#include <vector>

#include "core/board.h"

namespace gomoku {

struct ThreatAnalysis {
    bool attackerHasForcedWin = false;
    Move firstWinningMove{};
    std::vector<Move> winningLine;
    std::vector<Move> defensiveMoves;
};

class IThreatSolver {
public:
    virtual ~IThreatSolver() = default;

    virtual ThreatAnalysis analyzeThreats(const Board& board, Player attacker) = 0;
    virtual void notifyMove(const Move& move) = 0;
    virtual void notifyUndo(const Move& move) = 0;
};

} // namespace gomoku

