#pragma once

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
    int           depth = -1;
    TTNodeType    type = TTNodeType::Exact;
    Move          bestMove;
};

class TranspositionTable {
public:
    explicit TranspositionTable(std::size_t size = 1 << 20);

    void clear();

    TTEntry* probe(std::uint64_t key);

    void store(std::uint64_t key,
               EvalScore     value,
               EvalScore     eval,
               int           depth,
               TTNodeType    type,
               const Move&   bestMove);

    static EvalScore toTTScore(EvalScore score, int plyFromRoot);
    static EvalScore fromTTScore(EvalScore score, int plyFromRoot);

private:
    std::vector<TTEntry> table_;
};

} // namespace gomoku

