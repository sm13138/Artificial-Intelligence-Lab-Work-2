#include <iostream>
using namespace std;

// Simple Tic-Tac-Toe with alpha-beta pruning on a 3x3 board.
// Player 'X' is the maximizer (computer), 'O' is the minimizer (human or another player).

const int BOARD_SIZE = 3;

struct GameState {
    char board[BOARD_SIZE][BOARD_SIZE];
};

void initBoard(GameState &state) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            state.board[i][j] = ' ';
        }
    }
}

void printBoard(const GameState &state) {
    cout << "\n";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cout << " " << state.board[i][j];
            if (j < BOARD_SIZE - 1) cout << " |";
        }
        cout << "\n";
        if (i < BOARD_SIZE - 1) cout << "---+---+---\n";
    }
    cout << "\n";
}

bool movesLeft(const GameState &state) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (state.board[i][j] == ' ')
                return true;
    return false;
}

// Returns +10 if 'X' has won, -10 if 'O' has won, 0 otherwise
int evaluate(const GameState &state) {
    // Rows
    for (int row = 0; row < BOARD_SIZE; ++row) {
        if (state.board[row][0] != ' ' &&
            state.board[row][0] == state.board[row][1] &&
            state.board[row][1] == state.board[row][2]) {
            return state.board[row][0] == 'X' ? 10 : -10;
        }
    }
    // Columns
    for (int col = 0; col < BOARD_SIZE; ++col) {
        if (state.board[0][col] != ' ' &&
            state.board[0][col] == state.board[1][col] &&
            state.board[1][col] == state.board[2][col]) {
            return state.board[0][col] == 'X' ? 10 : -10;
        }
    }
    // Diagonals
    if (state.board[0][0] != ' ' &&
        state.board[0][0] == state.board[1][1] &&
        state.board[1][1] == state.board[2][2]) {
        return state.board[0][0] == 'X' ? 10 : -10;
    }
    if (state.board[0][2] != ' ' &&
        state.board[0][2] == state.board[1][1] &&
        state.board[1][1] == state.board[2][0]) {
        return state.board[0][2] == 'X' ? 10 : -10;
    }
    return 0;
}

// Alpha-beta minimax
int alphaBeta(GameState &state, int depth, int alpha, int beta, bool isMaximizing) {
    int score = evaluate(state);

    // Terminal states
    if (score == 10 || score == -10)
        return score;
    if (!movesLeft(state))
        return 0;

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (state.board[i][j] == ' ') {
                    state.board[i][j] = 'X';
                    int value = alphaBeta(state, depth + 1, alpha, beta, false);
                    state.board[i][j] = ' ';
                    if (value > best) best = value;
                    if (best > alpha) alpha = best;
                    if (beta <= alpha)
                        return best; // beta cut-off
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (state.board[i][j] == ' ') {
                    state.board[i][j] = 'O';
                    int value = alphaBeta(state, depth + 1, alpha, beta, true);
                    state.board[i][j] = ' ';
                    if (value < best) best = value;
                    if (best < beta) beta = best;
                    if (beta <= alpha)
                        return best; // alpha cut-off
                }
            }
        }
        return best;
    }
}

pair<int,int> findBestMove(GameState &state) {
    int bestVal = -1000;
    pair<int,int> bestMove = {-1, -1};

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state.board[i][j] == ' ') {
                state.board[i][j] = 'X';
                int moveVal = alphaBeta(state, 0, -1000, 1000, false);
                state.board[i][j] = ' ';
                if (moveVal > bestVal) {
                    bestMove = {i, j};
                    bestVal = moveVal;
                }
            }
        }
    }
    return bestMove;
}

int main() {
    GameState state;
    initBoard(state);

    cout << "Tic-Tac-Toe with Alpha-Beta Pruning\n";
    cout << "Computer: X (maximizer), You: O (minimizer)\n";

    bool computerTurn = true;

    while (true) {
        printBoard(state);
        int score = evaluate(state);
        if (score == 10) {
            cout << "Computer (X) wins!\n";
            break;
        } else if (score == -10) {
            cout << "You (O) win!\n";
            break;
        } else if (!movesLeft(state)) {
            cout << "It's a draw!\n";
            break;
        }

        if (computerTurn) {
            pair<int,int> move = findBestMove(state);
            state.board[move.first][move.second] = 'X';
            cout << "Computer plays: (" << move.first << ", " << move.second << ")\n";
        } else {
            int r, c;
            cout << "Enter your move (row and column: 0, 1, or 2): ";
            cin >> r >> c;
            if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || state.board[r][c] != ' ') {
                cout << "Invalid move. Try again.\n";
                continue;
            }
            state.board[r][c] = 'O';
        }
        computerTurn = !computerTurn;
    }

    printBoard(state);
    return 0;
}

