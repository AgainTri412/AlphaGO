#pragma once

#include <cstdint>
#include <vector>

namespace gomoku {

// Represents the two possible players.
enum class Player { Black = 0, White = 1 };

// Coordinate-based move (0 <= x,y < 12).
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
// side-effectful make/unmake plus read-only queries.
class Board {
public:
    Board();

    // Basic cell queries -----------------------------------------------------
    bool isOccupied(int x, int y) const;
    int  getCellState(int x, int y) const; // 0 = empty, 1 = black, 2 = white

    // Move management --------------------------------------------------------
    Player sideToMove() const { return side_to_move_; }
    bool   makeMove(int x, int y);   // toggles side, updates hash; false if illegal
    bool   unmakeMove(int x, int y); // toggles side back; assumes previous makeMove

    // Move generation helpers ------------------------------------------------
    std::vector<Move> getLegalMoves() const;
    std::vector<Move> getCandidateMoves() const; // proximity-limited move set

    // Game state utilities ---------------------------------------------------
    bool checkWin(Player player) const;
    int  countStones(Player player) const;

    // Zobrist hashing --------------------------------------------------------
    uint64_t getHashKey() const { return hashKey_; }

    // Legacy setup utilities (use sparingly; keep hash consistent) -----------
    Player getSideToMove() const { return side_to_move_; }
    void   setSideToMove(Player p);
    bool   placeStone(int x, int y, Player player);
    bool   removeStone(int x, int y, Player player);

private:
    static inline int index(int x, int y) { return y * 12 + x; }
    static inline int chunkOf(int idx) { return idx >> 6; }
    static inline int offsetOf(int idx) { return idx & 63; }

    uint64_t bb_[2][3];
    Player   side_to_move_;

    // Zobrist tables (lazily initialized) -----------------------------------
    static bool     zobristInitialized_;
    static uint64_t zobristTable_[12][12][2];
    static uint64_t zobristSide_;

    uint64_t hashKey_;

    static void initZobrist();
};

} // namespace gomoku

