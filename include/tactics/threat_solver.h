// threat_solver.h
//
// Threat sequence search and threat board support for 12×12 Gomoku.
//
// This module provides:
//   - Fast lookup of local threats (five, open four, simple four, open three,
//     broken three, and non-forcing threats) for each player.
//   - A solver that searches for *forcing* winning threat sequences
//     (Allis / Czajka style).
//   - Computation of defensive move sets that refute an opponent’s threat
//     sequences.
//
// Designed to sit next to the Board module (board.h) and be driven by the
// search engine. Implementation lives in threat_solver.cpp.

#ifndef GOMOKU_THREAT_SOLVER_H
#define GOMOKU_THREAT_SOLVER_H

#include <cstdint>
#include <memory>
#include <vector>

#include "core/board.h"             // Path updated to match structure
#include "tactics/Ithreat_solver.h" // [NEW] Include the interface
#include "tactics/threat_solver_types.h"

namespace gomoku {

// -----------------------------------------------------------------------------
// ThreatSolver – main OOP interface
// -----------------------------------------------------------------------------

/**
 * @brief Threat search and threat-board helper for a given root position.
 *
 * Typical usage pattern:
 *
 *   ThreatSolver solver(rootBoard);
 *
 *   // At each node in the main search:
 *   DefensiveSet ds = solver.computeDefensiveSet(rootBoard.sideToMove(), limits);
 *   if (ds.isLost) {
 *       // Node is tactically lost.
 *   } else if (!ds.defensiveMoves.empty()) {
 *       // Only consider ds.defensiveMoves in the search tree.
 *   }
 *
 * You can keep one ThreatSolver per root position and keep it in sync via
 * syncFromBoard() or the incremental onRootMove*() notifications.
 */
class ThreatSolver : public IThreatSolver {
public:
    // -------------------------------------------------------------------------
    // Construction & synchronisation
    // -------------------------------------------------------------------------

    /**
     * @brief Construct a solver from an initial board position.
     *
     * Builds all internal data structures (rotated bitboards, threat board, etc.)
     * from the given Board.
     *
     * @param board Current root position. Reference stored; Board must outlive this ThreatSolver.
     */
    explicit ThreatSolver(const Board& board);

    /**
     * @brief Rebuild internal state from a Board snapshot.
     *
     * Use this when the root position changes in a non-incremental way
     * (e.g. after receiving the opponent’s move from the server).
     *
     * @param board New root board state.
     */
    void syncFromBoard(const Board& board);

    /**
     * @brief Notify the solver that a move has been made.
     *
     * This incrementally updates internal caches around the last move.
     * Intended to be called directly after Board::makeMove().
     *
     * @param move Move that was just played.
     */
    void notifyMove(const Move& move) override;

    /**
     * @brief Notify the solver that a move has been undone.
     *
     * This incrementally updates internal caches around the undone move.
     * Intended to be called directly after Board::unmakeMove().
     *
     * @param move Move that was just undone.
     */
    void notifyUndo(const Move& move) override;

    
    // --- [NEW] Interface Implementation ---
    
    // This acts as an adapter. It calls the detailed methods below 
    // and populates the generic ThreatAnalysis struct.
    ThreatAnalysis analyzeThreats(const Board& board, Player attacker) override {
        ThreatAnalysis result;
        
        // 1. Check for Forced Win
        ThreatSequence winningSeq;
        // Use default limits or configure via member variables if needed
        if (findWinningThreatSequence(attacker, winningSeq)) {
            result.attackerHasForcedWin = true;
            if (!winningSeq.attackerMoves.empty()) {
                result.firstWinningMove = winningSeq.attackerMoves.front();
            }
            result.winningLine = winningSeq.attackerMoves;
        }

        // 2. Compute Defenses (if opponent is attacking, or general safety)
        // Note: In SearchEngine, we usually call analyzeThreats(board, sideToMove)
        // to see if *we* win, and analyzeThreats(board, opponent) to see if *they* win.
        
        // If we found a win for 'attacker', the 'defensiveMoves' field implies 
        // moves that the *defender* could play to stop it.
        // However, your computeDefensiveSet API asks for "defender" perspective.
        Player defender = (attacker == Player::Black) ? Player::White : Player::Black;
        
        // We only strictly need defensive set if the opponent (attacker) has a threat.
        // But for completeness, we can query it.
        DefensiveSet ds = computeDefensiveSet(defender);
        
        if (ds.isLost) {
            // Even with optimal play, defender loses.
            result.defensiveMoves.clear(); 
        } else {
            result.defensiveMoves = ds.defensiveMoves;
        }

        return result;
    }

    // -------------------------------------------------------------------------
    // Main threat sequence queries
    // -------------------------------------------------------------------------

    /**
     * @brief Search for a *forcing* winning threat sequence for a given attacker.
     *
     * Looks for sequences of threats (simple fours, open threes, broken threes,
     * etc.) that end in an OpenFour or Five and force the defender to respond
     * at each step (under Allis’s “all-defenses” assumption).
     *
     * @param attacker    Player for whom we are searching a winning sequence.
     * @param outSequence On success, filled with one found winning sequence.
     * @param limits      Search limits and optional abort flag.
     *
     * @return true if a winning sequence was found within the limits;
     *         false if no sequence was found or the search was aborted.
     *
     * Note: A false result does *not* prove that no winning sequence exists.
     *       It only means none was found under the current limits.
     */
    bool findWinningThreatSequence(Player attacker,
                                   ThreatSequence& outSequence,
                                   const ThreatSearchLimits& limits = {}) const;

    /**
     * @brief Compute the set of moves that defend against all opponent wins.
     *
     * Conceptually:
     *   1. Treat the opponent of @p defender as the attacker and search for
     *      winning threat sequences.
     *   2. If none are found, the position is tactically safe and
     *      DefensiveSet::defensiveMoves is empty.
     *   3. If at least one winning sequence is found, derive the set of moves
     *      that refute *all* such sequences (possibly empty).
     *
     * @param defender Player whose perspective we are defending.
     * @param limits   Search limits and optional abort flag.
     *
     * @return DefensiveSet describing whether the position is lost and, if not,
     *         which moves are valid defenses.
     */
    DefensiveSet computeDefensiveSet(Player defender,
                                     const ThreatSearchLimits& limits = {}) const;

    // -------------------------------------------------------------------------
    // Lightweight tactical queries (no full sequence search)
    // -------------------------------------------------------------------------

    /**
     * @brief Check if a player currently has any *immediate* winning threat.
     *
     * An “immediate winning threat” is an existing Five or OpenFour on the
     * board for @p attacker.
     *
     * @param attacker Player to test.
     * @return true if player has a Five or OpenFour already on the board.
     */
    bool hasImmediateWinningThreat(Player attacker) const;

    /**
     * @brief Collect all *forcing* threats currently available to a player.
     *
     * Forcing threats are:
     * - SimpleFour
     * - OpenThree
     * - BrokenThree
     *
     * @param attacker Player for whom to collect threats.
     * @param out      Vector that will be appended with all current forcing threats.
     */
    void collectCurrentForcingThreats(Player attacker,
                                      std::vector<ThreatInstance>& out) const;

    /**
     * @brief Get the threat type available to @p attacker at @p move in one direction.
     *
     * Uses the solver’s internal threat board. If @p move is occupied or
     * no threat exists in that direction, ThreatType::None is returned.
     *
     * @param attacker  Player to consider as attacker.
     * @param move      Empty intersection to test.
     * @param direction Direction in which to inspect threats.
     *
     * @return Threat type in that direction for this player and move.
     */
    ThreatType getThreatAt(Player attacker,
                           const Move& move,
                           Direction direction) const;

    /**
     * @brief Get all four directional threat types at @p move for @p attacker.
     *
     * Convenience helper for evaluation.
     *
     * @param attacker Player to consider as attacker.
     * @param move     Empty intersection to test.
     * @param out      Vector of size 4 (one per Direction) to fill.
     * Existing contents will be overwritten.
     */
    void getThreatsAt(Player attacker,
                      const Move& move,
                      std::vector<ThreatType>& out) const;

    // -------------------------------------------------------------------------
    // Rule of three: allow copying/moving as normal
    // -------------------------------------------------------------------------

    ThreatSolver(const ThreatSolver& other);
    ThreatSolver& operator=(const ThreatSolver& other);

    ThreatSolver(ThreatSolver&& other) noexcept;
    ThreatSolver& operator=(ThreatSolver&& other) noexcept;

    ~ThreatSolver();

private:
    // Internal helper structures (defined in threat_solver_internal_state.h).
    struct RotatedBitboards;
    struct ThreatBoard;
    struct PatternTable;
    struct SearchContext;

    void onRootMoveMade(const Move& m);
    void onRootMoveUndone(const Move& m);
    void rebuildRotatedBitboards(const Board& board);
    void rebuildThreatBoard();
    bool runWinningThreatSearch(Player attacker,
                                ThreatSequence& outSeq,
                                const ThreatSearchLimits& limits) const;
    DefensiveSet runDefensiveSetSearch(Player defender,
                                       const ThreatSearchLimits& limits) const;

    const Board* rootBoard_ = nullptr;
    RotatedBitboards rotated_{};
    ThreatBoard threats_{};
};

} // namespace gomoku

#include "tactics/threat_solver_internal_state.h"

#endif // GOMOKU_THREAT_SOLVER_H
