#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "core/board.h"
#include "search/search_types.h"

namespace gomoku {

enum class TTNodeType : std::uint8_t { Exact, LowerBound, UpperBound };

struct TTEntry {
    std::uint64_t key = 0;
    EvalScore     value = 0; // root-relative score for stored node
    EvalScore     eval = 0;  // static eval at storage time (root-relative)
    int           depth = -1; // remaining depth from the node where this entry was stored
    TTNodeType    type = TTNodeType::Exact;
    Move          bestMove{};
};

// Fixed-size replacement transposition table. Not thread-safe.
class TranspositionTable {
public:
    explicit TranspositionTable(std::size_t size = 1u << 20); // size = number of entries

    void clear();

    // Returns a pointer to the slot for the given key (may contain unrelated data).
    TTEntry* probe(std::uint64_t key);

    // Stores/overwrites the slot for key using depth-preferred replacement.
    void store(std::uint64_t key,
               EvalScore     value,
               EvalScore     eval,
               int           depth,
               TTNodeType    type,
               const Move&   bestMove);

    // Encodes mate scores with ply distance to remain comparable across depths.
    static EvalScore toTTScore(EvalScore score, int plyFromRoot);
    static EvalScore fromTTScore(EvalScore score, int plyFromRoot);

private:
    std::vector<TTEntry> table_;
};

} // namespace gomoku

