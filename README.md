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

## Decision Tree – Restaurant Waiting Problem (`decision_tree_restaurant.cpp`)

### 1. Goal and Idea

- **Goal**: Decide whether to **wait for a table** at a restaurant.
- **Chapter**: Learning decision trees (ID3) – Chapter 19.
- **Key idea**: Learn a **decision tree** from examples using **entropy** and **information gain**, then use that tree to classify new situations (not hard‑coded rules).

---

### 2. Data Representation

#### 2.1 Training example

```cpp
struct Example {
    int attr[5];   // 0: alt, 1: hungry, 2: patrons, 3: price, 4: wait
    int label;     // 0 = DO NOT WAIT, 1 = WAIT
};
```

- **Attributes**:
  - `attr[0]` (`alt`): 0 = no alternate restaurant, 1 = alternate exists.
  - `attr[1]` (`hungry`): 0 = not hungry, 1 = hungry.
  - `attr[2]` (`patrons`): 0 = none, 1 = some, 2 = full.
  - `attr[3]` (`price`): 0 = cheap, 1 = normal, 2 = expensive.
  - `attr[4]` (`wait`): 0 = 0–10, 1 = 10–30, 2 = 30–60, 3 = >60 minutes.

- **Label**:
  - `label = 1` → **WAIT**.
  - `label = 0` → **DO NOT WAIT**.

A small fixed array `dataset[N]` of `Example` objects is used as the training set.

#### 2.2 Tree node

```cpp
struct Node {
    bool isLeaf;
    int  classLabel;     // valid if isLeaf == true
    int  attributeIndex; // index of tested attribute (0..4)
    int  numChildren;    // number of possible values
    Node* children[4];   // child per attribute value
};
```

- **Leaf node**: `isLeaf = true`, `classLabel` is 0 or 1 (final decision).
- **Internal node**: `isLeaf = false`,
  - `attributeIndex` tells which attribute to test,
  - `children[v]` points to the subtree used when that attribute has value `v`.

---

### 3. Entropy and Information Gain

#### 3.1 Entropy

```cpp
double entropy(double pYes, double pNo);
```

- Computes binary entropy:
  \[
    H(Y) = -p_{\text{yes}}\log_2 p_{\text{yes}} - p_{\text{no}}\log_2 p_{\text{no}}
  \]
- If all examples have the same label → entropy = 0 (pure node).
- Mixed labels → higher entropy (impure node).

#### 3.2 Information gain

```cpp
double informationGain(const vector<int> &exampleIndices, int attrIndex);
```

- **Inputs**:
  - `exampleIndices`: indices of examples at the current node.
  - `attrIndex`: attribute we consider splitting on.
- **Steps**:
  1. Use all `exampleIndices` to compute base entropy \(H(Y)\).
  2. For each value `v` of that attribute:
     - Collect the subset where `attr[attrIndex] == v`.
     - Compute entropy of that subset.
     - Weight by the fraction of examples in that subset.
  3. Sum weighted entropies → \(H(Y \mid \text{attr})\).
  4. Return gain:
     \[
     \text{Gain}(Y,\text{attr}) = H(Y) - H(Y \mid \text{attr})
     \]
- The attribute with **maximum information gain** is chosen for splitting (ID3 rule).

---

### 4. Helper Functions

#### 4.1 `allSameLabel`

```cpp
bool allSameLabel(const vector<int> &exampleIndices, int &labelOut);
```

- Returns `true` if **all** examples at this node have the same label.
- Sets `labelOut` to that label (0 or 1).
- Used as a **stopping condition**: if true, we create a leaf node.

#### 4.2 `majorityLabel`

```cpp
int majorityLabel(const vector<int> &exampleIndices);
```

- Counts how many examples are label 0 and label 1.
- Returns the label that occurs more often.
- Used when:
  - No attributes remain to split on, or
  - A branch gets no training examples (empty subset).

---

### 5. Building the Tree (ID3)

#### 5.1 `buildTree`

```cpp
Node* buildTree(const vector<int> &exampleIndices,
                const vector<int> &remainingAttributes);
```

- **Inputs**:
  - `exampleIndices`: examples in this node.
  - `remainingAttributes`: attributes still allowed for splitting.

- **Algorithm**:
  1. If `allSameLabel` is true → return a **leaf** with that label.
  2. If `remainingAttributes` is empty → return a **leaf** with `majorityLabel`.
  3. For each attribute in `remainingAttributes`, compute `informationGain`.
  4. Choose `bestAttr` = attribute with **max gain**.
  5. Create an internal `Node`:
     - `attributeIndex = bestAttr`, `numChildren` = domain size.
  6. For each value `v` of `bestAttr`:
     - Build a subset of examples with `attr[bestAttr] == v`.
     - If subset is empty → child is a leaf with parent `majorityLabel`.
     - Else → child is `buildTree(subset, remainingAttributes − {bestAttr})`.

This is a direct, small implementation of the **ID3** decision tree algorithm.

---

### 6. Classification and Output

#### 6.1 `classify`

```cpp
int classify(Node* root, const Example &ex);
```

- Start at `root` and repeat:
  - If `node->isLeaf` → return `node->classLabel`.
  - Otherwise:
    - Let `a = node->attributeIndex`.
    - Let `v = ex.attr[a]`.
    - Move to `node = node->children[v]`.
- Result:
  - `1` → print **“WAIT for a table.”**
  - `0` → print **“DO NOT WAIT.”**

#### 6.2 `main` function flow

- Build `exampleIndices = {0..N-1}` and `attributes = {0,1,2,3,4}`.
- Call `buildTree(exampleIndices, attributes)` to learn the tree.
- Call `printTree(root)` to show the structure (for explanation).
- Read a new situation from the user (values for all 5 attributes).
- Pack into an `Example ex` and call `classify(root, ex)`.
- Print the final decision (wait / do not wait).

This completes a simple, end‑to‑end demonstration of **learning and using** a decision tree for the restaurant waiting problem.

- **Decision tree (restaurant)**:
  ```bash
  g++ decision_tree_restaurant.cpp -o dtree
  ./dtree
  ```

On Windows (PowerShell or CMD), replace `./program` with `program.exe` (for example: `ttt.exe`).

