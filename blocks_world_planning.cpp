#include <iostream>
using namespace std;

// Very small Blocks World planner using simple forward search.
// We use predicates similar to PDDL:
// ON(x, y), ONTABLE(x), CLEAR(x), HOLDING(x), ARMEMPTY
// But all logic is implemented directly in C++ with simple arrays.

const int MAX_BLOCKS = 4;

struct State {
    // on[i] = j means block i is on block j, -1 if not on another block
    int on[MAX_BLOCKS];
    // onTable[i] = true if block i is on the table
    bool onTable[MAX_BLOCKS];
    // clear[i] = true if no block is on top of block i
    bool clear[MAX_BLOCKS];
    // holding = index of block in hand, or -1 if none
    int holding;
};

void printState(const State &s, const string &name) {
    cout << name << ":\n";
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        cout << "Block " << char('A' + i) << ": ";
        if (s.onTable[i]) cout << "on table ";
        if (s.on[i] != -1) cout << "on " << char('A' + s.on[i]) << " ";
        if (s.clear[i]) cout << "clear ";
        cout << "\n";
    }
    if (s.holding != -1) {
        cout << "Arm holding " << char('A' + s.holding) << "\n";
    } else {
        cout << "Arm empty\n";
    }
    cout << "-----------------\n";
}

State makeInitialState() {
    State s;
    // Example: initial: A on table, B on A, C on table, D on table
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        s.on[i] = -1;
        s.onTable[i] = false;
        s.clear[i] = true;
    }
    s.onTable[0] = true; // A
    s.onTable[2] = true; // C
    s.onTable[3] = true; // D
    s.on[1] = 0;         // B on A
    s.clear[0] = false;  // A not clear (B on A)
    s.clear[1] = true;
    s.clear[2] = true;
    s.clear[3] = true;
    s.holding = -1;
    return s;
}

State makeGoalState() {
    State s;
    // Goal: C on B on A on table, D on table
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        s.on[i] = -1;
        s.onTable[i] = false;
        s.clear[i] = true;
    }
    s.onTable[0] = true; // A on table
    s.on[1] = 0;         // B on A
    s.on[2] = 1;         // C on B
    s.onTable[3] = true; // D on table
    s.clear[0] = false;
    s.clear[1] = false;
    s.clear[2] = true;
    s.clear[3] = true;
    s.holding = -1;
    return s;
}

bool sameState(const State &a, const State &b) {
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        if (a.on[i] != b.on[i]) return false;
        if (a.onTable[i] != b.onTable[i]) return false;
        if (a.clear[i] != b.clear[i]) return false;
    }
    if (a.holding != b.holding) return false;
    return true;
}

// Simple actions inspired by PDDL operators

bool canPickupFromTable(const State &s, int x) {
    return s.onTable[x] && s.clear[x] && s.holding == -1;
}

State pickupFromTable(const State &s, int x) {
    State ns = s;
    ns.onTable[x] = false;
    ns.holding = x;
    return ns;
}

bool canPutDownToTable(const State &s) {
    return s.holding != -1;
}

State putDownToTable(const State &s) {
    State ns = s;
    int x = ns.holding;
    ns.holding = -1;
    ns.onTable[x] = true;
    ns.clear[x] = true;
    return ns;
}

bool canUnstack(const State &s, int x, int y) {
    return s.on[x] == y && s.clear[x] && s.holding == -1;
}

State unstack(const State &s, int x, int y) {
    State ns = s;
    ns.on[x] = -1;
    ns.clear[x] = true;
    ns.clear[y] = true;
    ns.holding = x;
    return ns;
}

bool canStack(const State &s, int x, int y) {
    return s.holding == x && s.clear[y];
}

State stack(const State &s, int x, int y) {
    State ns = s;
    ns.holding = -1;
    ns.on[x] = y;
    ns.clear[x] = true;
    ns.clear[y] = false;
    return ns;
}

// Very small depth-limited DFS planner.
bool dfsPlan(const State &current, const State &goal, int depthLimit) {
    if (sameState(current, goal)) {
        printState(current, "Goal reached");
        return true;
    }
    if (depthLimit == 0) return false;

    // Try all possible actions on all blocks
    for (int x = 0; x < MAX_BLOCKS; ++x) {
        // pickup from table
        if (canPickupFromTable(current, x)) {
            State next = pickupFromTable(current, x);
            cout << "Action: PICKUP " << char('A' + x) << " FROM TABLE\n";
            if (dfsPlan(next, goal, depthLimit - 1)) return true;
        }
        // unstack x from y
        for (int y = 0; y < MAX_BLOCKS; ++y) {
            if (x != y && canUnstack(current, x, y)) {
                State next = unstack(current, x, y);
                cout << "Action: UNSTACK " << char('A' + x) << " FROM " << char('A' + y) << "\n";
                if (dfsPlan(next, goal, depthLimit - 1)) return true;
            }
        }
    }
    // put down to table
    if (canPutDownToTable(current)) {
        State next = putDownToTable(current);
        cout << "Action: PUTDOWN " << char('A' + current.holding) << " TO TABLE\n";
        if (dfsPlan(next, goal, depthLimit - 1)) return true;
    }
    // stack in hand onto any block
    if (current.holding != -1) {
        int x = current.holding;
        for (int y = 0; y < MAX_BLOCKS; ++y) {
            if (x != y && canStack(current, x, y)) {
                State next = stack(current, x, y);
                cout << "Action: STACK " << char('A' + x) << " ON " << char('A' + y) << "\n";
                if (dfsPlan(next, goal, depthLimit - 1)) return true;
            }
        }
    }
    return false;
}

int main() {
    State initial = makeInitialState();
    State goal = makeGoalState();

    printState(initial, "Initial state");
    printState(goal, "Goal state");

    int depthLimit = 15;
    if (!dfsPlan(initial, goal, depthLimit)) {
        cout << "No plan found within depth limit " << depthLimit << "\n";
    }
    return 0;
}

