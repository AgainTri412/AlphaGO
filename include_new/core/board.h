#pragma once

#include <cstdint>
#include <vector>

namespace gomoku {

// Board constants for compile-time sizing and bounds checks.
static constexpr int kBoardSize = 12;
static constexpr int kBoardCells = kBoardSize * kBoardSize;

// Represents the two possible players.
enum class Player { Black = 0, White = 1 };

// Coordinate-based move (0 <= x,y < kBoardSize).
struct Move {
    int x = 0;
    int y = 0;

    bool operator==(const Move& other) const { return x == other.x && y == other.y; }
    bool operator<(const Move& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }
};

// 12x12 board representation using bitboards and incremental Zobrist hashing.
// Public API is intentionally minimal: Board owns all state and provides
// side-effectful make/unmake plus read-only queries. Not thread-safe.
class Board {
public:
    Board();

    // Basic cell queries -----------------------------------------------------
    // Returns true if the cell is occupied by either color; bounds must be valid.
    bool isOccupied(int x, int y) const;
    // 0 = empty, 1 = black, 2 = white; bounds must be valid.
    int  getCellState(int x, int y) const;

    // Move management --------------------------------------------------------
    Player sideToMove() const { return side_to_move_; }
    // Places a stone for sideToMove() at (x,y), toggles the side, updates hash.
    // Returns false if the move is illegal (out of bounds or occupied).
    bool   makeMove(int x, int y);
    // Removes the stone placed by makeMove at (x,y), toggles the side back,
    // restores hash. Assumes (x,y) was the last move made by the opposite side.
    bool   unmakeMove(int x, int y);

    // Move generation helpers ------------------------------------------------
    // Returns all legal moves (typically all empty cells).
    std::vector<Move> getLegalMoves() const;
    // Returns proximity-limited candidates for move ordering.
    std::vector<Move> getCandidateMoves() const;

    // Game state utilities ---------------------------------------------------
    // Checks whether the specified player has a five-in-a-row.
    bool checkWin(Player player) const;
    int  countStones(Player player) const;

    // Zobrist hashing --------------------------------------------------------
    uint64_t getHashKey() const { return hashKey_; }

    // Legacy setup utilities (use sparingly; keep hash consistent) -----------
    Player getSideToMove() const { return side_to_move_; }
    void   setSideToMove(Player p);
    // Direct stone placement/removal for setup; caller must avoid conflicts and
    // keep hashing consistent with side_to_move_.
    bool   placeStone(int x, int y, Player player);
    bool   removeStone(int x, int y, Player player);

private:
    static inline int index(int x, int y) { return y * kBoardSize + x; }
    static inline int chunkOf(int idx) { return idx >> 6; }
    static inline int offsetOf(int idx) { return idx & 63; }

    uint64_t bb_[2][3];
    Player   side_to_move_;

    // Zobrist tables (lazily initialized) -----------------------------------
    static bool     zobristInitialized_;
    static uint64_t zobristTable_[kBoardSize][kBoardSize][2];
    static uint64_t zobristSide_;

    uint64_t hashKey_;

    static void initZobrist();
};

} // namespace gomoku

