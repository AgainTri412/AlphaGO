#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "core/board.h"
#include "tactics/ithreat_solver.h"

namespace gomoku {

enum class Direction : std::uint8_t { Horizontal, Vertical, DiagNWSE, DiagNESW };

enum class ThreatType : std::uint8_t {
    None = 0,
    Five,
    OpenFour,
    SimpleFour,
    OpenThree,
    BrokenThree,
    SimpleThree,
    TwoFourWays,
    TwoThreeWays,
    TwoTwoWays,
    TwoOneWay,
    OneFiveWays,
    OneFourWays,
    OneThreeWays,
    OneTwoWays,
    OneOneWay
};

struct ThreatInstance {
    ThreatType type = ThreatType::None;
    Player     attacker = Player::Black;
    Direction  direction = Direction::Horizontal;
    std::vector<Move> stones;
    std::vector<Move> requiredEmpty;
    std::vector<Move> defensePoints;
    std::vector<Move> finishingMoves;
};

struct ThreatSequence {
    Player attacker = Player::Black;
    std::vector<ThreatInstance> threats;
    std::vector<Move> attackerMoves;
    std::vector<Move> defenderMoves;
};

struct DefensiveSet {
    bool              isLost = false;
    std::vector<Move> defensiveMoves;
};

struct ThreatSearchLimits {
    int         maxNodes = 200000;
    int         maxDepth = 20;
    const bool* abortFlag = nullptr; // non-owning
};

// Concrete threat solver using PIMPL to hide heavy implementation details.
class ThreatSolver : public IThreatSolver {
public:
    explicit ThreatSolver(const Board& board);

    void syncFromBoard(const Board& board);
    void notifyMove(const Move& move) override;
    void notifyUndo(const Move& move) override;

    ThreatAnalysis analyzeThreats(const Board& board, Player attacker) override;

    bool findWinningThreatSequence(Player attacker,
                                   ThreatSequence& outSequence,
                                   const ThreatSearchLimits& limits = {}) const;

    DefensiveSet computeDefensiveSet(Player defender,
                                     const ThreatSearchLimits& limits = {}) const;

    bool hasImmediateWinningThreat(Player attacker) const;
    void collectCurrentForcingThreats(Player attacker, std::vector<ThreatInstance>& out) const;
    ThreatType getThreatAt(Player attacker, const Move& move, Direction direction) const;
    void getThreatsAt(Player attacker, const Move& move, std::vector<ThreatType>& out) const;

    ThreatSolver(const ThreatSolver& other);
    ThreatSolver& operator=(const ThreatSolver& other);
    ThreatSolver(ThreatSolver&& other) noexcept;
    ThreatSolver& operator=(ThreatSolver&& other) noexcept;
    ~ThreatSolver();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace gomoku

