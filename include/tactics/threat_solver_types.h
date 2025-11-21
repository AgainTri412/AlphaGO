#pragma once

#include <cstdint>
#include <vector>

#include "core/board.h"

namespace gomoku {

// -----------------------------------------------------------------------------
// Basic enums used by the solver
// -----------------------------------------------------------------------------

/**
 * @brief Directions in which threats can appear.
 */
enum class Direction : uint8_t {
    Horizontal = 0,  ///< Along a row (x changes, y fixed)
    Vertical   = 1,  ///< Along a column (y changes, x fixed)
    DiagNWSE   = 2,  ///< NW–SE diagonal (\ direction)
    DiagNESW   = 3   ///< NE–SW diagonal (/ direction)
};

/**
 * @brief Threat type classification (a,b) where:
 *        a = stones towards five, b = number of ways to complete.
 *
 * The most important for search:
 *   - Five, OpenFour             => immediate / winning threats.
 *   - SimpleFour, OpenThree,
 *     BrokenThree                => forcing threats.
 *
 * Others are non-forcing but useful for evaluation.
 */
enum class ThreatType : uint8_t {
    None = 0,

    // Winning
    Five,          ///< (5,1) – existing five in a row
    OpenFour,      ///< (4,2) – two winning continuations

    // Forcing threats (opponent must respond or lose)
    SimpleFour,    ///< (4,1) – one winning continuation
    OpenThree,     ///< (3,3) – four empties, two defense points
    BrokenThree,   ///< (3,2) – three empties, three defense options

    // Non-forcing threats
    SimpleThree,   ///< (3,1)
    TwoFourWays,   ///< (2,4)
    TwoThreeWays,  ///< (2,3)
    TwoTwoWays,    ///< (2,2)
    TwoOneWay,     ///< (2,1)
    OneFiveWays,   ///< (1,5)
    OneFourWays,   ///< (1,4)
    OneThreeWays,  ///< (1,3)
    OneTwoWays,    ///< (1,2)
    OneOneWay      ///< (1,1)
};

// -----------------------------------------------------------------------------
// Threat primitives exposed to the rest of the engine
// -----------------------------------------------------------------------------

/**
 * @brief Concrete threat instance on the board for a given player.
 *
 * All coordinates are absolute board moves (0 ≤ x,y < 12).
 */
struct ThreatInstance {
    ThreatType type       = ThreatType::None;     ///< Type/strength of threat.
    Player     attacker   = Player::Black;        ///< Player that owns the threat.
    Direction  direction  = Direction::Horizontal;///< Direction of the line.

    // Stones that belong to the attacker and are part of the pattern.
    std::vector<Move> stones;

    // Empty squares that are required to remain empty for the threat to work
    // (includes defense points and auxiliary empties).
    std::vector<Move> requiredEmpty;

    // Squares where the defender can legally defend this threat.
    std::vector<Move> defensePoints;

    // Squares the attacker can later play on to convert this threat into a
    // stronger/winning threat (e.g. open-three → open-four).
    std::vector<Move> finishingMoves;
};

/**
 * @brief A forcing threat sequence for a single attacking player.
 *
 * This is a tactical line that (if valid and not refuted) should end in
 * a winning threat (Five or OpenFour).
 */
struct ThreatSequence {
    /// Player for whom this sequence is winning.
    Player attacker = Player::Black;

    /// Threats in logical order, respecting dependencies.
    std::vector<ThreatInstance> threats;

    /// Concrete moves for the attacker in this sequence (in play order).
    std::vector<Move> attackerMoves;

    /// Concrete moves for the defender in this sequence, under the
    /// “all-defenses” assumption (may contain multiple responses per threat).
    std::vector<Move> defenderMoves;
};

/**
 * @brief Result of computing defensive moves against an opponent’s threat search.
 *
 * If isLost is true, no defensive move can stop all winning sequences.
 * Otherwise, defensiveMoves lists the subset of moves that keep the position
 * alive; the search engine should focus on those moves in this node.
 */
struct DefensiveSet {
    bool isLost = false;              ///< True iff no defense exists; node is lost.
    std::vector<Move> defensiveMoves; ///< Moves that defend against all found sequences.
};

// -----------------------------------------------------------------------------
// Search-time limits / configuration
// -----------------------------------------------------------------------------

/**
 * @brief Limits and optional abort flag for a single threat search call.
 */
struct ThreatSearchLimits {
    /// Maximum number of internal nodes (threat/combo nodes) to explore.
    int maxNodes = 200000;

    /// Maximum logical depth (number of threat layers).
    int maxDepth = 20;

    /// Optional external abort flag (owned by caller). If non-null and set
    /// to true during search, the solver will stop early and return “no info”.
    const bool* abortFlag = nullptr;
};

} // namespace gomoku

