// Checkers AI — minimax with alpha-beta pruning
// Checkers has a smaller branching factor than chess, so we can search deeper

class CheckersAI {
    constructor() {
        this.gamesPlayed = 0;
        this.level = 1;
    }

    setLevel(level) { this.level = Math.max(1, Math.min(5, level)); }
    getLevel()      { return this.level; }
    getLevelName()  { return ['Beginner', 'Novice', 'Intermediate', 'Advanced', 'Expert'][this.level - 1]; }

    // Checkers has avg ~8 moves/position; alpha-beta effective branching ~3
    // Depths chosen so even Expert finishes in well under a second
    getDepth() { return [2, 3, 5, 7, 9][this.level - 1]; }

    getBestMove(engine, color) {
        const moves = engine.getAllLegalMoves(color);
        if (moves.length === 0) return null;

        // Beginner: 50% random
        if (this.level === 1 && Math.random() < 0.5) {
            return moves[Math.floor(Math.random() * moves.length)];
        }

        // Order jumps first — better alpha-beta pruning
        moves.sort((a, b) => (b.isJump ? 1 : 0) - (a.isJump ? 1 : 0));

        let bestMove = null;
        let bestScore = -Infinity;
        const depth = this.getDepth();

        for (const move of moves) {
            const saved = this.saveState(engine);
            engine.makeMove(move);
            const score = this.minimax(engine, depth - 1, -Infinity, Infinity, false, color);
            this.restoreState(engine, saved);
            if (score > bestScore) { bestScore = score; bestMove = move; }
        }

        return bestMove || moves[0];
    }

    minimax(engine, depth, alpha, beta, isMaximizing, aiColor) {
        if (depth === 0 || engine.gameStatus === 'gameover') {
            return this.evaluate(engine, aiColor);
        }

        const moves = engine.getAllLegalMoves(engine.currentTurn);
        if (moves.length === 0) return this.evaluate(engine, aiColor);

        // Order captures first
        moves.sort((a, b) => (b.isJump ? 1 : 0) - (a.isJump ? 1 : 0));

        if (isMaximizing) {
            let max = -Infinity;
            for (const move of moves) {
                const saved = this.saveState(engine);
                engine.makeMove(move);
                max = Math.max(max, this.minimax(engine, depth - 1, alpha, beta, false, aiColor));
                this.restoreState(engine, saved);
                alpha = Math.max(alpha, max);
                if (beta <= alpha) break;
            }
            return max;
        } else {
            let min = Infinity;
            for (const move of moves) {
                const saved = this.saveState(engine);
                engine.makeMove(move);
                min = Math.min(min, this.minimax(engine, depth - 1, alpha, beta, true, aiColor));
                this.restoreState(engine, saved);
                beta = Math.min(beta, min);
                if (beta <= alpha) break;
            }
            return min;
        }
    }

    saveState(engine) {
        return {
            board:       engine.board.map(r => r.map(c => c ? { ...c } : null)),
            currentTurn: engine.currentTurn,
            moveHistory: [...engine.moveHistory],
            gameStatus:  engine.gameStatus,
            winner:      engine.winner
        };
    }

    restoreState(engine, state) {
        engine.board       = state.board;
        engine.currentTurn = state.currentTurn;
        engine.moveHistory = state.moveHistory;
        engine.gameStatus  = state.gameStatus;
        engine.winner      = state.winner;
    }

    // ── Board evaluation ──────────────────────────────────────────────────────

    // Positional table — rewards centre control and advancement
    static POS = [
        [0,  4,  0,  4,  0,  4,  0,  4],
        [4,  0,  3,  0,  3,  0,  3,  0],
        [0,  3,  0,  2,  0,  2,  0,  2],
        [2,  0,  2,  0,  2,  0,  2,  0],
        [0,  2,  0,  2,  0,  2,  0,  2],
        [2,  0,  2,  0,  2,  0,  2,  0],
        [0,  3,  0,  3,  0,  3,  0,  3],
        [4,  0,  4,  0,  4,  0,  4,  0]
    ];

    evaluate(engine, aiColor) {
        if (engine.gameStatus === 'gameover') {
            return engine.winner === aiColor ? 100000 : -100000;
        }

        const opp   = engine.opponent(aiColor);
        const level = this.level;
        let score   = 0;

        for (let r = 0; r < 8; r++) {
            for (let c = 0; c < 8; c++) {
                const piece = engine.board[r][c];
                if (!piece) continue;

                const forAI = piece.color === aiColor;
                const sign  = forAI ? 1 : -1;

                // Material: king = 3× man
                let value = piece.type === 'king' ? 300 : 100;

                // Level 2+: positional bonus
                if (level >= 2) {
                    const pr = forAI ? r : 7 - r;
                    value += CheckersAI.POS[pr][c];
                }

                // Level 3+: reward protecting the back row (prevents opponent kings)
                if (level >= 3) {
                    const backRow = piece.color === 'red' ? 7 : 0;
                    if (r === backRow && piece.type === 'man') value += 20;
                }

                score += sign * value;
            }
        }

        // Level 4+: mobility — more moves = more options
        if (level >= 4) {
            const aiMoves  = engine.getAllLegalMoves(aiColor).length;
            const oppMoves = engine.getAllLegalMoves(opp).length;
            score += (aiMoves - oppMoves) * 5;
        }

        // Level 5: penalise isolated or exposed pieces
        if (level >= 5) {
            score += this.safetyBonus(engine, aiColor);
        }

        return score;
    }

    safetyBonus(engine, aiColor) {
        let bonus = 0;
        const opp = engine.opponent(aiColor);
        for (let r = 1; r < 7; r++) {
            for (let c = 1; c < 7; c++) {
                const piece = engine.board[r][c];
                if (!piece) continue;
                const sign = piece.color === aiColor ? 1 : -1;
                // Bonus for having friendly piece behind (diagonally)
                const backDir = piece.color === 'red' ? 1 : -1;
                for (const dc of [-1, 1]) {
                    const br = r + backDir, bc = c + dc;
                    if (br >= 0 && br < 8 && bc >= 0 && bc < 8 && engine.board[br][bc]?.color === piece.color) {
                        bonus += sign * 5;
                    }
                }
            }
        }
        return bonus;
    }

    onGameComplete() { this.gamesPlayed++; }
}
