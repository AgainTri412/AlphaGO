#include "tactics/threat_solver.h"

#include <utility>

#include "tactics/threat_solver_internal.h"

namespace gomoku {

ThreatSolver::ThreatSolver(const Board& board)
    : impl_(std::make_unique<Impl>(board)) {}

ThreatSolver::ThreatSolver(const ThreatSolver& other)
    : impl_(std::make_unique<Impl>(*other.impl_)) {}

ThreatSolver& ThreatSolver::operator=(const ThreatSolver& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}

ThreatSolver::ThreatSolver(ThreatSolver&& other) noexcept = default;
ThreatSolver& ThreatSolver::operator=(ThreatSolver&& other) noexcept = default;
ThreatSolver::~ThreatSolver() = default;

void ThreatSolver::syncFromBoard(const Board& board) { impl_->syncFromBoard(board); }

void ThreatSolver::notifyMove(const Move& move) { impl_->notifyMove(move); }

void ThreatSolver::notifyUndo(const Move& move) { impl_->notifyUndo(move); }

ThreatAnalysis ThreatSolver::analyzeThreats(const Board& board, Player attacker) {
    return impl_->analyzeThreats(board, attacker);
}

bool ThreatSolver::findWinningThreatSequence(Player attacker,
                                             ThreatSequence& outSequence,
                                             const ThreatSearchLimits& limits) const {
    return impl_->findWinningThreatSequence(attacker, outSequence, limits);
}

DefensiveSet ThreatSolver::computeDefensiveSet(Player defender,
                                               const ThreatSearchLimits& limits) const {
    return impl_->computeDefensiveSet(defender, limits);
}

bool ThreatSolver::hasImmediateWinningThreat(Player attacker) const {
    return impl_->hasImmediateWinningThreat(attacker);
}

void ThreatSolver::collectCurrentForcingThreats(Player attacker,
                                                std::vector<ThreatInstance>& out) const {
    impl_->collectCurrentForcingThreats(attacker, out);
}

ThreatType ThreatSolver::getThreatAt(Player attacker, const Move& move, Direction direction) const {
    return impl_->getThreatAt(attacker, move, direction);
}

void ThreatSolver::getThreatsAt(Player attacker, const Move& move, std::vector<ThreatType>& out) const {
    impl_->getThreatsAt(attacker, move, out);
}

} // namespace gomoku

