#include "tactics/threat_solver_internal.h"

namespace gomoku {

namespace {
Player otherPlayer(Player p) { return (p == Player::Black) ? Player::White : Player::Black; }
}

ThreatSolver::Impl::Impl(const Board& board) : rootBoard(&board) {}

ThreatSolver::Impl::Impl(const Impl& other) : rootBoard(other.rootBoard) {}

ThreatSolver::Impl& ThreatSolver::Impl::operator=(const Impl& other) {
    if (this != &other) {
        rootBoard = other.rootBoard;
    }
    return *this;
}

void ThreatSolver::Impl::syncFromBoard(const Board& board) { rootBoard = &board; }

void ThreatSolver::Impl::notifyMove(const Move& /*move*/) {
    // No cached rotated bitboards: keeping the board reference is sufficient.
}

void ThreatSolver::Impl::notifyUndo(const Move& /*move*/) {
    // No cached rotated bitboards: keeping the board reference is sufficient.
}

ThreatType ThreatSolver::Impl::getThreatAt(Player /*attacker*/,
                                           const Move& /*move*/,
                                           Direction /*direction*/) const {
    // Simplified solver only understands immediate wins; expose none for now.
    return ThreatType::None;
}

void ThreatSolver::Impl::getThreatsAt(Player attacker, const Move& move, std::vector<ThreatType>& out) const {
    out.clear();
    out.push_back(getThreatAt(attacker, move, Direction::Horizontal));
    out.push_back(getThreatAt(attacker, move, Direction::Vertical));
    out.push_back(getThreatAt(attacker, move, Direction::DiagNWSE));
    out.push_back(getThreatAt(attacker, move, Direction::DiagNESW));
}

} // namespace gomoku

