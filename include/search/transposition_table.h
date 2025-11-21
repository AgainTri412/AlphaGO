#ifndef GOMOKU_SEARCH_TRANSPOSITION_TABLE_H
#define GOMOKU_SEARCH_TRANSPOSITION_TABLE_H

#include <cstdint>
#include <vector>
#include "core/board.h"
#include "search/search_types.h"

namespace gomoku {

enum class TTNodeType : std::uint8_t {
    Exact,       // Exact score.
    LowerBound,  // alpha-beta lower bound (score >= value).
    UpperBound   // alpha-beta upper bound (score <= value).
};

struct TTEntry {
    std::uint64_t key = 0;
    EvalScore     value = 0; // from the perspective of rootSideToMove at the start of the search
    EvalScore     eval = 0;  // from the perspective of rootSideToMove at the start of the search
    int           depth = -1;
    TTNodeType    type = TTNodeType::Exact;
    Move          bestMove;
};

class TranspositionTable {
public:
    // Initialize with a default size (e.g., 2^20 entries).
    explicit TranspositionTable(std::size_t size = 1048576);

    void clear();

    // Retrieve an entry. Returns nullptr if key doesn't match or empty.
    TTEntry* probe(std::uint64_t key);

    // Store an entry using replacement policy.
    void store(std::uint64_t key,
               EvalScore     value,
               EvalScore     eval,
               int           depth,
               TTNodeType    type,
               const Move&   bestMove);

    // Normalize mate scores relative to root ply.
    static EvalScore toTTScore(EvalScore score, int plyFromRoot);
    static EvalScore fromTTScore(EvalScore score, int plyFromRoot);

private:
    std::vector<TTEntry> table_;
};

} // namespace gomoku

#endif // GOMOKU_SEARCH_TRANSPOSITION_TABLE_H