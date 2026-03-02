## Artificial Intelligence Lab 2 – C++ Implementations

This lab contains three small C++ programs that demonstrate:

- **Alpha–beta pruning for game search (Tic‑Tac‑Toe)** – Chapter 6
- **Classical planning / PDDL‑style blocks world** – Chapter 11
- **Learning and using decision trees (restaurant waiting problem)** – Chapter 19

All code uses **simple, standard C++**, with arrays and basic control structures, so it is easy to read and explain in a presentation.

---

## 1. Tic‑Tac‑Toe with Alpha–Beta Pruning (`tic_tac_toe_alpha_beta.cpp`)

### Overview

- **Problem**: Play optimal Tic‑Tac‑Toe on a 3×3 board.
- **Player `X`**: maximizer (computer).
- **Player `O`**: minimizer (human).
- **Algorithm**: **Minimax search with alpha–beta pruning** (Chapter 6).
- **Data structure**: a simple `GameState` struct with a 2D `char` array.

```cpp
struct GameState {
    char board[BOARD_SIZE][BOARD_SIZE];
};
```

#### `initBoard(GameState &state)`

- **Purpose**: Fills the board with spaces `' '` so it is empty.
- **How it works**: Two nested `for` loops set each cell `board[i][j] = ' ';`.
- **Why**: This is the starting state of the game.

#### `printBoard(const GameState &state)`

- **Purpose**: Prints the 3×3 board in a human‑readable form.
- **How it works**:
  - Nested loops print each cell separated by `|`.
  - Prints row separators `---+---+---`.
- **Why**: Allows the user to see the current position.

#### `movesLeft(const GameState &state)`

- **Purpose**: Check if there is at least one empty cell.
- **How it works**:
  - Scans the whole board; if any cell is `' '`, returns `true`.
  - If no empty cell is found, returns `false`.
- **Why**: Used to detect draw / terminal states.

#### `evaluate(const GameState &state)`

- **Purpose**: Static evaluation function for terminal positions.
- **Return values**:
  - `+10` if `X` has three in a row.
  - `-10` if `O` has three in a row.
  - `0` otherwise (no winner).
- **How it works**:
  - Checks each row: if all three symbols are equal and not space.
  - Checks each column similarly.
  - Checks two diagonals.
- **Why**: This is the standard minimax score for Tic‑Tac‑Toe.

#### `alphaBeta(GameState &state, int depth, int alpha, int beta, bool isMaximizing)`

- **Purpose**: Core **alpha–beta minimax** search.
- **Parameters**:
  - `state`: current game state (board).
  - `depth`: search depth (not heavily used here, but useful in general).
  - `alpha`: best value that the maximizer can guarantee so far.
  - `beta`: best value that the minimizer can guarantee so far.
  - `isMaximizing`: `true` when it is `X`’s turn, `false` for `O`.
- **Algorithm (Minimax + Alpha–Beta)**:
  1. Compute `score = evaluate(state)`.
  2. If `score` is `+10` or `-10`, return it (win or loss).
  3. If there are no moves left, return `0` (draw).
  4. If `isMaximizing` (player `X`):
     - Initialize `best = -1000`.
     - For each empty cell:
       - Place `'X'` in that cell.
       - Recursively call `alphaBeta` with `isMaximizing = false`.
       - Undo the move.
       - Update `best = max(best, value)`.
       - Update `alpha = max(alpha, best)`.
       - **If `beta <= alpha`, stop exploring** (beta cut‑off).
  5. If minimizer (player `O`):
     - Similar loop, but:
       - Place `'O'`.
       - Minimize `best`.
       - Update `beta = min(beta, best)`.
       - **If `beta <= alpha`, stop** (alpha cut‑off).
- **Why**: Alpha–beta pruning reduces the number of explored nodes by cutting off branches that cannot affect the final decision.

#### `findBestMove(GameState &state)`

- **Purpose**: Top‑level function to choose the computer’s move.
- **How it works**:
  - For each empty cell `(i, j)`:
    - Temporarily place `'X'`.
    - Call `alphaBeta(..., isMaximizing = false)` to evaluate this move.
    - Undo the move.
    - Track the move with the **maximum** value.
  - Return the best `(row, col)` pair.
- **Why**: This wraps alpha–beta search into a simple action choice.

#### `main()`

- **Purpose**: Runs a complete game between the computer and the human.
- **Flow**:
  - Initialize the board.
  - Loop:
    - Print board.
    - Check for win or draw via `evaluate` + `movesLeft`.
    - If it is the computer’s turn:
      - Call `findBestMove` and place `'X'`.
    - Else (human’s turn):
      - Read row and column from input.
      - Validate and place `'O'`.
    - Alternate turns.

---

## 2. Blocks World Planning (`blocks_world_planning.cpp`)

### Overview

- **Problem**: Transform an **initial stack of blocks** into a **goal stack** using simple moves.
- **Inspired by**: **PDDL / STRIPS‑style operators** (Chapter 11).
- **Data structure**: `State` struct with arrays representing predicates.

```cpp
struct State {
    int  on[MAX_BLOCKS];      // on[x] = y means x is on y, -1 otherwise
    bool onTable[MAX_BLOCKS]; // true if x is directly on the table
    bool clear[MAX_BLOCKS];   // true if nothing is on top of x
    int  holding;             // index of block in hand, or -1 if empty
};
```

### States and Predicates

- **`on[x] = y`**: block `x` is on block `y` (`ON(x, y)`).
- **`onTable[x]`**: block `x` is on the table (`ONTABLE(x)`).
- **`clear[x]`**: there is no block on top of `x` (`CLEAR(x)`).
- **`holding`**:
  - `-1` means arm is empty (`ARMEMPTY`).
  - `k >= 0` means arm is holding block `k` (`HOLDING(k)`).

This is a direct, array‑based encoding of PDDL‑like predicates.

#### `printState(const State &s, const string &name)`

- **Purpose**: Shows the symbolic state (for debugging and explanation).
- **How it works**:
  - Prints each block `A, B, C, D` with its properties.
  - Prints what the arm is holding (if anything).

#### `makeInitialState()` and `makeGoalState()`

- **Purpose**: Hard‑code example initial and goal configurations.
- **Initial example**:
  - `A` on table.
  - `B` on `A`.
  - `C` on table.
  - `D` on table.
- **Goal example**:
  - `A` on table.
  - `B` on `A`.
  - `C` on `B`.
  - `D` on table.
- **How they work**:
  - Set arrays `on`, `onTable`, `clear`, and `holding` to match these predicates.

#### `sameState(const State &a, const State &b)`

- **Purpose**: Compare two states.
- **How it works**:
  - Loop over all blocks and compare `on`, `onTable`, `clear`.
  - Also compare `holding`.
- **Why**: Used as the **goal test**.

### PDDL‑Style Operators as C++ Functions

Each operator has a **precondition checker** and an **effect function**:

- **Pickup from table**:
  - `canPickupFromTable(const State &s, int x)`
    - Preconditions:
      - `onTable[x]` is `true`.
      - `clear[x]` is `true`.
      - `holding == -1` (arm empty).
  - `pickupFromTable(const State &s, int x)`
    - Effects:
      - `onTable[x] = false`.
      - `holding = x`.

- **Put down to table**:
  - `canPutDownToTable(const State &s)`
    - Precondition: `holding != -1`.
  - `putDownToTable(const State &s)`
    - Effects:
      - Put `holding` block on table.
      - Reset `holding = -1`.

- **Unstack x from y**:
  - `canUnstack(const State &s, int x, int y)`
    - Preconditions:
      - `on[x] == y`.
      - `clear[x]` is `true`.
      - `holding == -1`.
  - `unstack(const State &s, int x, int y)`
    - Effects:
      - `on[x] = -1` (x is no longer on y).
      - `clear[x] = true`.
      - `clear[y] = true`.
      - `holding = x`.

- **Stack x on y**:
  - `canStack(const State &s, int x, int y)`
    - Preconditions:
      - `holding == x`.
      - `clear[y]` is `true`.
  - `stack(const State &s, int x, int y)`
    - Effects:
      - `holding = -1`.
      - `on[x] = y`.
      - `clear[y] = false`.

This mirrors PDDL operators with **preconditions** and **effects**, but written with simple arrays.

### Planner: Depth‑Limited DFS – `dfsPlan(const State &current, const State &goal, int depthLimit)`

- **Purpose**: Find a sequence of actions from `current` to `goal`.
- **Algorithm (DFS)**:
  1. If `sameState(current, goal)` is `true`, print the state and succeed.
  2. If `depthLimit == 0`, fail (stop search).
  3. Try all possible actions:
     - For each block `x`:
       - If `canPickupFromTable`, apply `pickupFromTable` and recurse.
       - For each block `y`, if `canUnstack`, apply `unstack` and recurse.
     - If `canPutDownToTable`, apply `putDownToTable` and recurse.
     - If `holding != -1`, try stacking on each possible `y` using `stack`.
  4. If any recursive call succeeds, return `true`.
  5. Otherwise, return `false`.
- **Why**: This is a simple **forward search planner** guided only by the depth limit (no heuristics).

#### `main()`

- Creates the initial and goal states.
- Prints both.
- Calls `dfsPlan(initial, goal, depthLimit)`.
- Outputs actions like:
  - `PICKUP A FROM TABLE`
  - `UNSTACK B FROM A`
  - `STACK C ON B`

---

## 3. Decision Tree – Restaurant Waiting Problem (`decision_tree_restaurant.cpp`)

### Overview

- **Problem**: Decide whether to **wait for a table** at a restaurant.
- **Related topic**: **Decision tree learning** (Chapter 19).
- **Data structure**:

```cpp
struct Example {
    bool alt;       // alternate restaurant available
    bool hungry;    // hungry or not
    int  patrons;   // 0 = none, 1 = some, 2 = full
    int  price;     // 0 = cheap, 1 = normal, 2 = expensive
    int  wait;      // 0 = 0-10, 1 = 10-30, 2 = 30-60, 3 = >60
    bool waitDecision; // target: wait (true) or not (false)
};
```

We keep a **small training dataset** in a fixed array `dataset[N]`.

### Entropy and Information Gain

#### `double entropy(double pYes, double pNo)`

- **Purpose**: Compute binary entropy \(H(Y)\) for the target label.
- **Formula**:  
  \[
    H(Y) = -p_{\text{yes}} \log_2 p_{\text{yes}} - p_{\text{no}} \log_2 p_{\text{no}}
  \]
- **How it works**:
  - A helper `log2safe` returns 0 if probability is 0.
  - Computes the sum as above.
- **Why**: Entropy measures impurity of a node in a decision tree.

#### `double infoGainOnHungry()`

- **Purpose**: Demonstrate how **information gain** is computed for a split on the `hungry` attribute.
- **Steps**:
  1. Count number of positive and negative examples in the whole dataset.
  2. Compute base entropy \(H(Y)\).
  3. Partition the dataset into:
     - `hungry == true` subset.
     - `hungry == false` subset.
  4. For each subset:
     - Compute its entropy \(H(Y \mid hungry = v)\).
  5. Compute conditional entropy:
     \[
     H(Y \mid hungry) = \sum_v P(hungry=v) H(Y \mid hungry=v)
     \]
  6. Return:
     \[
     \text{Gain}(Y, hungry) = H(Y) - H(Y \mid hungry)
     \]
- **Why**: This is exactly what **ID3** does to choose the best attribute for splitting.

### Hard‑Coded Decision Tree: `classifyWaitDecision(...)`

```cpp
bool classifyWaitDecision(bool alt, bool hungry, int patrons, int price, int wait);
```

- **Purpose**: Use a **simple decision tree** (hand‑coded) to classify a new situation.
- **Simplified rules**:
  - If `patrons == 0` → **do not wait**.
  - Else if `patrons == 1` → **wait**.
  - Else (`patrons == full`):
    - If `price == expensive` **and** `wait` is `30–60` or `>60` → **do not wait**.
    - Else → **wait**.
- **Note**:
  - Attributes `alt` and `hungry` are **not used** in this small tree, but they are part of the interface to show how attributes can be added.
- **Relationship to learning**:
  - In a full implementation, a learning algorithm like ID3 would **build this tree automatically** from many examples using entropy and information gain.  
  - Here we demonstrate:
    - How to compute information gain (for one attribute).
    - How a tree can be represented as nested `if/else` code.

#### `main()`

- **Steps**:
  1. Prints the information gain for the `hungry` attribute.
  2. Reads attributes for a new situation from `cin`.
  3. Calls `classifyWaitDecision` to get the decision.
  4. Prints:
     - `"WAIT for a table."` or
     - `"DO NOT WAIT."`

---

## How to Compile and Run

Use any standard C++ compiler (e.g. `g++`).

- **Tic‑Tac‑Toe (alpha–beta)**:
  ```bash
  g++ tic_tac_toe_alpha_beta.cpp -o ttt
  ./ttt
  ```

- **Blocks world planner**:
  ```bash
  g++ blocks_world_planning.cpp -o blocks
  ./blocks
  ```

- **Decision tree (restaurant)**:
  ```bash
  g++ decision_tree_restaurant.cpp -o dtree
  ./dtree
  ```

On Windows (PowerShell or CMD), replace `./program` with `program.exe` (for example: `ttt.exe`).

