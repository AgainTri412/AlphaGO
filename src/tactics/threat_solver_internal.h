#pragma once

#include <vector>

#include "core/board.h"
#include "tactics/threat_solver.h"

namespace gomoku {

struct ThreatSolver::Impl {
    explicit Impl(const Board& board);
    Impl(const Impl& other);
    Impl& operator=(const Impl& other);
    Impl(Impl&& other) noexcept = default;
    Impl& operator=(Impl&& other) noexcept = default;

    void syncFromBoard(const Board& board);
    void notifyMove(const Move& move);
    void notifyUndo(const Move& move);

    ThreatAnalysis analyzeThreats(const Board& board, Player attacker);
    bool findWinningThreatSequence(Player attacker,
                                   ThreatSequence& outSequence,
                                   const ThreatSearchLimits& limits) const;
    DefensiveSet computeDefensiveSet(Player defender,
                                     const ThreatSearchLimits& limits) const;
    bool hasImmediateWinningThreat(Player player) const;
    void collectCurrentForcingThreats(Player player, std::vector<ThreatInstance>& out) const;
    ThreatType getThreatAt(Player attacker, const Move& move, Direction direction) const;
    void getThreatsAt(Player attacker, const Move& move, std::vector<ThreatType>& out) const;

    const Board* rootBoard = nullptr;
};

} // namespace gomoku

