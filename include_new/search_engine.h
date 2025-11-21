#pragma once

#include "core/board.h"
#include "search/search_types.h"
#include "search/evaluator.h"
#include "search/transposition_table.h"
#include "search/time_manager.h"
#include "search/history_heuristic.h"
#include "tactics/ithreat_solver.h"

namespace gomoku {

class SearchEngine {
public:
    SearchEngine(Board& board,
                 IEvaluator& evaluator,
                 IThreatSolver* threatSolver,
                 IHistoryHeuristic* historyHeuristic);

    SearchResult searchBestMove(const SearchLimits& limits);
    const SearchResult& getLastSearchResult() const { return lastResult_; }

    void clearTranspositionTable() { tt_.clear(); }

private:
    EvalScore search(int depth, EvalScore alpha, EvalScore beta, int ply, bool allowNull, bool inPV);
    EvalScore quiescence(EvalScore alpha, EvalScore beta, int ply);
    void      iterativeDeepening();

    bool      canDoNullMove(const ThreatAnalysis& threatInfo, int depth, int ply) const;
    EvalScore nullMoveSearch(EvalScore alpha, EvalScore beta, int depth, int ply);
    void      extractPrincipalVariation(std::vector<Move>& outPV, int maxDepth);

    Board&             board_;
    IEvaluator&        evaluator_;
    IThreatSolver*     threatSolver_;
    IHistoryHeuristic* history_;

    TranspositionTable tt_;
    TimeManager        timeManager_;

    Player       rootSide_ = Player::Black;
    SearchResult lastResult_{};

    std::uint64_t nodes_ = 0;
    std::uint64_t qnodes_ = 0;
    std::uint64_t hashHits_ = 0;
};

} // namespace gomoku

