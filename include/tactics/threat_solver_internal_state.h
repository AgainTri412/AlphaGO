#pragma once

#include <cstdint>
#include <mutex>
#include <vector>

#include "core/board.h"
#include "tactics/threat_solver_types.h"

namespace gomoku {

struct ThreatSolver::RotatedBitboards {
    uint64_t bb[2][4][3] = {};

    void setStone(Player p, int x, int y);
    void clearStone(Player p, int x, int y);
    uint32_t extractLine(Player p, Direction dir, int lineId, int& outLen) const;
};

struct ThreatSolver::ThreatBoard {
    ThreatType cells[2][GOMOKU_BOARD_SIZE][GOMOKU_BOARD_SIZE][4] = {};

    void clear();
    void rebuild(const RotatedBitboards& rbb);
    void incrementalUpdate(const RotatedBitboards& rbb, int x, int y);
};

struct ThreatSolver::PatternTable {
    std::vector<uint32_t> patterns;
    std::vector<int> candidatePatternIds;
    std::vector<int> bestPatternAtCell;

    void initializeOnce();
    static const PatternTable& instance();
};

struct ThreatSolver::SearchContext {
    Board boardCopy;
    Player attacker;
    ThreatSearchLimits limits;
    int nodes = 0;

    SearchContext(const Board& root, Player a, const ThreatSearchLimits& lim);
};

} // namespace gomoku

