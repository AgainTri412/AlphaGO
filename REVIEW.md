# Header Design Review for Gomoku Engine

## include/core/board.h
**Role:** Central game-state container using bitboards, exposes move validation, Zobrist hashing, and move generation helpers.

**Strengths**
- Clear public API for move making/unmaking and hash retrieval; exposes candidate move generation for search usage.
- Lazy Zobrist initialization kept private; incremental hash maintenance described in comments.
- Small helper struct `Move` with ordering/comparison utilities, aiding containers and sorting.

**Issues / Risks**
- Mixed naming conventions (`side_to_move` vs `sideToMove()` and helper aliases) complicate clarity on preferred usage.
- `makeMove`/`unmakeMove` use raw coordinates; no overload accepting `Move`, leading to duplication at call sites and potential mismatch with `MoveGuard`.
- Incremental helpers (`placeStone`, `removeStone`, `setSideToMove`) lack context on intended usage (e.g., setup only) and safety guarantees; comments partially present but not strict.
- `unmakeMove`’s contract assumes caller supplies correct coordinates; no link to history tracking—risk of misuse in search without move stack.
- Missing explicit size accessors or constants (board dimension, maximum moves) that other modules might need; hardcoded 12×12 repeated in comments and private helpers.
- `getCandidateMoves` bounding-box heuristic is specified but without parameters to tune radius; might limit future experimentation.

**Recommendations**
- Standardize naming by introducing `sideToMove()`/`setSideToMove()` as canonical and marking legacy aliases deprecated in comments; align member naming style or hide the member.
- Add `bool makeMove(const Move&)`/`bool unmakeMove(const Move&)` overloads to reduce coordinate duplication and ensure consistent usage with `MoveGuard`.
- Clarify usage scope of setup-style mutators (e.g., “not for search; for initial position loading only”) and specify whether they maintain move history or invariants.
- Provide board dimension constants (e.g., `static constexpr int BoardSize = 12`) and maybe `maxMoves()` to avoid magic numbers across modules.
- Document that callers must maintain their own move stack for `unmakeMove`, or expose optional history tracking to prevent misuse.
- Consider parameterizing or at least documenting the bounding-box radius used by `getCandidateMoves`, and whether it is symmetrical for both players.

## include/search/seach_types.h
**Role:** Shared search primitives (scores, limits, result struct, RAII move guard).

**Strengths**
- Explicit score constants and `SearchResult` capture essential search metadata (PV, nodes, flags).
- `MoveGuard` RAII helper enforces make/unmake symmetry, reducing leak risk in recursive search.
- Comments note score perspective in `SearchResult::bestScore` (root side), aligning with design goal.

**Issues / Risks**
- Filename misspelled (`seach_types.h`); may cause include churn and typos.
- Score semantics documented in `SearchResult` but not reflected in `EvalScore` type alias or constants; mate score normalization details absent here (delegated to TT but not referenced).
- `MoveGuard` stores `Move` by value and silently ignores invalid moves; callers may not realize when `makeMove` failed without checking `isValid`.
- `SearchLimits` fields lack units clarity (depth as plies? time as ms); no notion of iterative deepening stop conditions.

**Recommendations**
- Rename header to `search_types.h` and update includes to match; prevents accidental double-includes or missing file errors.
- Add explicit comment that all scores are relative to the root side to move at search start, including constants, to avoid mixing evaluator perspective.
- Document units for `SearchLimits` fields (depth in plies, times in ms) and behavior when zero (e.g., unlimited nodes/time?).
- Consider returning validity state in `MoveGuard` ctor or asserting in debug builds; optionally store `bool valid()` as public to emphasize check necessity.

## include/search/evaluator.h
**Role:** Interface for static evaluation function used by search.

**Strengths**
- Clear emphasis on speed and perspective (`maxPlayer`) in comments.
- Dependency-free interface keeps evaluator modular.

**Issues / Risks**
- Return perspective (root-relative vs maxPlayer) is only implied; must be consistent with search expecting root-relative scores.
- Lack of const qualifier on `evaluate` (non-const method) may hinder stateless implementations but allows caches; intent not clarified.
- Ownership semantics: interface doesn’t state thread-safety or reuse constraints (important for concurrent search or per-search reuse).

**Recommendations**
- Add explicit statement that evaluation score is from `maxPlayer`’s perspective and that search passes root side as `maxPlayer` to keep semantics aligned; warn against returning absolute scores.
- Consider making `evaluate` `const` if evaluator caches can be marked `mutable`, or document that implementations may be stateful.
- Document thread-safety expectations or that one evaluator instance is used per search thread.

## include/search/history_heuristic.h
**Role:** Interface for history-based move ordering statistics.

**Strengths**
- Minimal, focused API (query, record beta cutoff, record PV, clear).
- Perspective parameter `sideToMove` present for per-side tables.

**Issues / Risks**
- Lacks guidance on scale/units of returned scores and whether higher is always better; could lead to inconsistent ordering across implementations.
- Does not specify whether methods are expected to be cheap / O(1) or thread-safe.

**Recommendations**
- Document expected score range/sign (e.g., non-negative, larger = better) and whether saturation/clamping is expected.
- Clarify performance expectations and intended use (e.g., called every node; should be lock-free; per-search thread instance recommended).

## include/search/time_manager.h
**Role:** Manages per-search time checks.

**Strengths**
- Simple API with start/check/elapsed and internal stop flag.
- Uses steady_clock for monotonic timing.

**Issues / Risks**
- `checkStopCondition` signature uses `nodesVisited` and `inPanic` but return semantics vs internal `stop_` updates are not fully documented; unclear if caller must also read `isStopped`.
- `SearchLimits` embedded here (from search_types) lacks clarity on interaction with panic/extra time and whether zero disables limits.
- `start` takes limits by value; no link to current player or game stage for time allocation strategy.

**Recommendations**
- Document exact stopping rules (e.g., stop when elapsed >= timeLimitMs + optional panicExtraTimeMs if inPanic) and whether repeated calls are needed.
- Clarify how `maxNodes` interacts with time; specify behavior when limits are zero or negative.
- Consider returning a structured status (continue/stop reason) to aid instrumentation.

## include/search/transposition_table.h
**Role:** Zobrist-keyed TT storing bounds, eval, best move; helper to normalize mate scores.

**Strengths**
- Clear separation of TT entry fields with perspective comment for `value`/`eval` (root side).
- Provides mate normalization helpers and replacement-ready `store` API.

**Issues / Risks**
- `probe` returns mutable pointer; ownership and lifetime rules not documented (e.g., not stable after next store?).
- Missing const overload for read-only probing; forces mutable access in const contexts.
- `value`/`eval` semantics ambiguous: distinction between search score and static eval not fully described; when to fill each not specified.
- Table size fixed at construction; no mention of thread safety or replacement policy (e.g., depth-preferred vs always replace).

**Recommendations**
- Document pointer validity and that entries may be overwritten; consider returning `const TTEntry*` for const contexts.
- Clarify intended meaning of `value` vs `eval` and when each is stored/read; specify node type expectations.
- Add comments on replacement policy (e.g., “always replace”, “depth-preferred”) and thread-safety (likely single-threaded per table).
- Provide constructor comment on expected size units (entries vs bytes) and alignment with hash key masking.

## include/tactics/Ithreat_solver.h
**Role:** Abstract interface for threat analysis (VCF) with threat analysis result structure.

**Strengths**
- Minimal API with analyze + incremental notifications; exposes winning line and defensive moves for integration.
- Non-owning, read-only contract for `Board` is stated.

**Issues / Risks**
- Namespace closing brace placement seems off (`};` before namespace comment), causing potential syntax/formatting confusion.
- `ThreatAnalysis` doesn’t clarify perspective of `defensiveMoves` (moves for defender vs attacker) leading to ambiguity in search usage.
- Lack of explicit guarantee on incremental methods order (must mirror board moves?) and whether they require corresponding Board updates.

**Recommendations**
- Fix namespace closure formatting for readability and to avoid accidental extra brace.
- Document that `defensiveMoves` are moves for the defender that refute attacker threats, and whether empty means safe or unknown.
- Explicitly state that `notifyMove`/`notifyUndo` must be called in tandem with Board updates and are not thread-safe.

## include/tactics/threat_solver.h
**Role:** Concrete threat solver with rich threat/sequence structures, pimpl encapsulation, and search helpers for VCF/VCN detection.

**Strengths**
- Comprehensive documentation of threat types, directions, and usage patterns; rich data structures for threats and sequences.
- Pimpl employed to hide implementation details; copy/move constructors declared to manage unique_ptr.
- Provides both full sequence search and lightweight threat queries for evaluation.

**Issues / Risks**
- `ThreatSolver` includes implementation of `analyzeThreats` inline in header, defeating pimpl separation and adding compile dependencies; mixes adapter logic with interface.
- Constructor stores `const Board&` reference per comment but data member absent; ownership/lifetime unclear relative to pimpl.
- Incremental sync functions lack clarity on whether solver maintains its own board snapshot or relies solely on notifications; risk of desynchronization.
- Several helper structs use public vectors without capacity guidance; potential heavy copying; no const-correctness (non-const methods) for query functions except getters.
- Score/perspective semantics not referenced, though threat results feed into search decisions; also no link to SearchLimits/TimeManager for timeouts except optional abort flag.
- Copy/move constructors/assignments declared but no comment on cost or validity with pimpl; may imply deep copy but unclear.

**Recommendations**
- Move `analyzeThreats` adapter definition to .cpp to preserve pimpl boundary and avoid coupling; keep only declaration in header.
- Clarify whether solver holds its own board snapshot; if so, document ownership and need to keep it synced via `syncFromBoard`/notify methods.
- Add explicit note that `computeDefensiveSet` returns defenses for opponent of defender’s opponent, clarifying perspective; ensure consistency with `ThreatAnalysis`.
- Consider making query methods (`hasImmediateWinningThreat`, `collectCurrentForcingThreats`, etc.) const-correct relative to internal caches and document thread-safety.
- Document cost/semantics of copy/move; if copying is expensive or unsupported, consider deleting copy or providing explicit `clone` semantics.

## include/search_engine.h
**Role:** Main search orchestrator wiring Board, evaluator, threat solver, history heuristic, TT, and time manager; exposes best-move search.

**Strengths**
- Dependency injection constructor matches layering goals; external ownership clarified in comment.
- Private search/quiescence methods note score perspective relative to root side, aligning with design requirement.
- Maintains search statistics and exposes last result for inspection.

**Issues / Risks**
- Constructor takes raw pointers for optional dependencies (`IThreatSolver*`, `IHistoryHeuristic*`) without nullability or ownership semantics; risk of null dereference and unclear lifetime.
- Score perspective is documented on search/quiescence but not reiterated in public `searchBestMove` or `SearchResult`; potential mismatch with evaluator expectations.
- No configurable move ordering or TT parameters exposed; transposition table size fixed internally with no ctor arg.
- Missing constness on accessor methods (only `getLastSearchResult` const); `searchBestMove` signature lacks limit validation semantics or iterative deepening control knobs.
- Lacks handling guidance for multi-threading or reentrancy; member TT/time manager are not thread-safe.

**Recommendations**
- Document pointer nullability; consider using references or `std::optional`/`nullptr`-checked behavior, or require non-null and use references to enforce DI.
- Expand public API docs to state score perspective of returned `SearchResult` and relation to `IEvaluator` (pass root side as maxPlayer).
- Allow configuring TT size/time manager policies via constructor or setter to aid tuning.
- Add note on thread-safety and single-threaded expectation; clarify that a SearchEngine instance should not be shared across concurrent searches.

