# GOMOKU_2
A competitive gomoku project under development.

## Goal: 
every non-core module can be implemented/tested independently, as long as it sees:
	•	core/board.h + core/move.h (+ Player).
	•	Its own interface header (threat_solver.h, history_heuristic.h, evaluator.h, …).

## Dependency graph (allowed directions)
	•	core: depends on nothing.
	•	tactics: depends on core only.
	•	eval: depends on core (and search/evaluator.h for the interface).
	•	search: depends on core, tactics/threat_solver.h, search/history_heuristic.h, search/evaluator.h, search/transposition_table.h, etc.

No module in core/, tactics/ or eval/ is allowed to include search/ headers (to avoid circular design).

## Conventions (should be encoded directly into header comments and parameter naming:)
1.	Score perspective
	•	All scores in search_types.h, transposition_table.h, search_engine.h are explicitly documented as:
	•	“from the perspective of rootSideToMove at the start of the search”.
	•	Eval interface always has a Player maxPlayer or uses a comment: “maxPlayer must be the root side”.
2.	Player semantics
	•	In every interface that takes a Player:
	•	attacker → threat solver.
	•	player in history heuristic → always side-to-move at the node.
	•	maxPlayer → eval.
	•	We never have a generic “player” whose meaning isn’t obvious from name & doc.
3.	Move safety
	•	Only Board mutates game state.
	•	Search internals use a single RAII helper (in src/search/node_search.cpp) to wrap makeMove/unmakeMove.
	•	No public interface in headers exposes setSideToMove(); if it exists, it’s clearly documented as “setup only, not for search”.
