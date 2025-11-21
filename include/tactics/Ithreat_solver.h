#ifndef GOMOKU_TACTICS_THREAT_SOLVER_H
#define GOMOKU_TACTICS_THREAT_SOLVER_H

#include <vector>
#include "core/board.h"

// Abstract threat solver API.

namespace gomoku {

struct ThreatAnalysis {
    // If true, 'attacker' has a forcing winning sequence assuming optimal defense.
    bool attackerHasForcedWin = false;

    // First move of a winning threat sequence for the attacker.
    Move firstWinningMove;

    // Optional: entire sequence of moves in the winning line.
    std::vector<Move> winningLine;

    // The set of defender moves that *jointly* defend against all threat sequences.
    std::vector<Move> defensiveMoves;
};

// Abstract threat solver API.
class IThreatSolver {
public:
    virtual ~IThreatSolver() = default;

    // Analyze threats for 'attacker' on the given board.
    // Must NOT modify 'board'.
    virtual ThreatAnalysis analyzeThreats(const Board& board, Player attacker) = 0;

    // Incremental state updates.
    virtual void notifyMove(const Move& move) = 0;
    virtual void notifyUndo(const Move& move) = 0;
};
};

} // namespace gomoku

#endif // GOMOKU_TACTICS_THREAT_SOLVER_H