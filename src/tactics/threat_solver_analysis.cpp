#include "tactics/threat_solver_internal.h"

#include <algorithm>

namespace gomoku {

namespace {

Player otherPlayer(Player p) { return (p == Player::Black) ? Player::White : Player::Black; }

bool isImmediateWinningMove(Board& board, const Move& move, Player attacker) {
    if (board.isOccupied(move.x, move.y)) {
        return false;
    }
    if (!board.makeMove(move.x, move.y)) {
        return false;
    }
    bool win = board.checkWin(attacker);
    board.unmakeMove(move.x, move.y);
    return win;
}

std::vector<Move> collectLegalMoves(const Board& board) {
    auto legal = board.getLegalMoves();
    std::sort(legal.begin(), legal.end(), [](const Move& a, const Move& b) {
        return (a.y == b.y) ? (a.x < b.x) : (a.y < b.y);
    });
    return legal;
}

}

ThreatAnalysis ThreatSolver::Impl::analyzeThreats(const Board& board, Player attacker) {
    syncFromBoard(board);

    ThreatAnalysis result;
    ThreatSequence sequence;
    if (findWinningThreatSequence(attacker, sequence, {})) {
        result.attackerHasForcedWin = true;
        if (!sequence.attackerMoves.empty()) {
            result.firstWinningMove = sequence.attackerMoves.front();
            result.winningLine = sequence.attackerMoves;
        }
        return result;
    }

    DefensiveSet defensive = computeDefensiveSet(otherPlayer(attacker), {});
    result.attackerHasForcedWin = defensive.isLost;
    result.defensiveMoves = defensive.defensiveMoves;
    return result;
}

bool ThreatSolver::Impl::findWinningThreatSequence(Player attacker,
                                                   ThreatSequence& outSequence,
                                                   const ThreatSearchLimits& limits) const {
    if (!rootBoard) return false;

    Board boardCopy = *rootBoard;
    if (boardCopy.checkWin(attacker)) {
        outSequence = ThreatSequence{attacker};
        return true;
    }

    auto legal = collectLegalMoves(boardCopy);
    int nodes = 0;
    for (const auto& move : legal) {
        if (limits.abortFlag && *limits.abortFlag) {
            return false;
        }
        if (++nodes > limits.maxNodes) {
            return false;
        }
        if (isImmediateWinningMove(boardCopy, move, attacker)) {
            ThreatInstance threat{};
            threat.type = ThreatType::Five;
            threat.attacker = attacker;
            threat.finishingMoves.push_back(move);

            ThreatSequence seq{};
            seq.attacker = attacker;
            seq.threats.push_back(threat);
            seq.attackerMoves.push_back(move);

            outSequence = std::move(seq);
            return true;
        }
    }

    return false;
}

DefensiveSet ThreatSolver::Impl::computeDefensiveSet(Player defender,
                                                     const ThreatSearchLimits& limits) const {
    DefensiveSet result;
    if (!rootBoard) return result;

    Player attacker = otherPlayer(defender);
    Board boardCopy = *rootBoard;
    auto legal = collectLegalMoves(boardCopy);

    int nodes = 0;
    for (const auto& move : legal) {
        if (limits.abortFlag && *limits.abortFlag) {
            return result;
        }
        if (++nodes > limits.maxNodes) {
            return result;
        }
        if (isImmediateWinningMove(boardCopy, move, attacker)) {
            result.defensiveMoves.push_back(move);
        }
    }

    if (result.defensiveMoves.empty()) {
        result.isLost = false;
        return result;
    }

    if (result.defensiveMoves.size() > 1) {
        result.isLost = true;
        result.defensiveMoves.clear();
        return result;
    }

    result.isLost = false;
    return result;
}

bool ThreatSolver::Impl::hasImmediateWinningThreat(Player player) const {
    if (!rootBoard) return false;
    Board boardCopy = *rootBoard;

    if (boardCopy.checkWin(player)) {
        return true;
    }

    auto legal = collectLegalMoves(boardCopy);
    for (const auto& move : legal) {
        if (isImmediateWinningMove(boardCopy, move, player)) {
            return true;
        }
    }
    return false;
}

void ThreatSolver::Impl::collectCurrentForcingThreats(Player player,
                                                      std::vector<ThreatInstance>& out) const {
    out.clear();
    if (!rootBoard) return;

    Board boardCopy = *rootBoard;
    auto legal = collectLegalMoves(boardCopy);
    for (const auto& move : legal) {
        if (isImmediateWinningMove(boardCopy, move, player)) {
            ThreatInstance threat{};
            threat.type = ThreatType::Five;
            threat.attacker = player;
            threat.finishingMoves.push_back(move);
            out.push_back(std::move(threat));
        }
    }
}

} // namespace gomoku

