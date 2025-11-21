#ifndef GOMOKU_SEARCH_ENGINE_H
#define GOMOKU_SEARCH_ENGINE_H

#include "core/board.h"
#include "search/search_types.h"
#include "search/evaluator.h"
#include "search/transposition_table.h"
#include "search/time_manager.h"
#include "search/history_heuristic.h"
#include "tactics/threat_solver.h"

namespace gomoku {

// The SearchEngine owns the search state for a single game.
// It orchestrates PVS, Time Management, and Tactical solvers.
class SearchEngine {
public:
    // SearchEngine constructor (Dependency Injection).
    // Dependencies (evaluator, threatSolver, history) must outlive SearchEngine.
    SearchEngine(Board&             board,
                 IEvaluator&        evaluator,
                 IThreatSolver* threatSolver,
                 IHistoryHeuristic* historyHeuristic);

    // Compute the best move within limits.
    SearchResult searchBestMove(const SearchLimits& limits);

    const SearchResult& getLastSearchResult() const { return lastResult_; }

    void clearTranspositionTable() { tt_.clear(); }

private:
    // --- Core Search ---
    EvalScore search(int depth, EvalScore alpha, EvalScore beta, int ply, bool allowNull, bool inPV);
    EvalScore quiescence(EvalScore alpha, EvalScore beta, int ply);
    void iterativeDeepening();

    // --- Helpers ---
    bool canDoNullMove(const ThreatAnalysis& threatInfo, int depth, int ply) const;
    EvalScore nullMoveSearch(EvalScore alpha, EvalScore beta, int depth, int ply);
    void extractPrincipalVariation(std::vector<Move>& outPV, int maxDepth);

    // --- Dependencies ---
    Board&             board_;
    IEvaluator&        evaluator_;
    IThreatSolver* threatSolver_;
    IHistoryHeuristic* history_;

    // --- Internal Components ---
    TranspositionTable tt_;
    TimeManager        timeManager_;

    // --- Search State ---
    Player             rootSide_;
    SearchResult       lastResult_;
    
    // Statistics counters (cleared per search)
    std::uint64_t      nodes_ = 0;
    std::uint64_t      qnodes_ = 0;
    std::uint64_t      hashHits_ = 0;
};

} // namespace gomoku

#endif // GOMOKU_SEARCH_ENGINE_H