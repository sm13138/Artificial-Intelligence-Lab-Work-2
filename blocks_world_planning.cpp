#include <iostream>
#include <vector>
#include <set>
using namespace std;

const int MAX_BLOCKS = 4;

struct State {
    int on[MAX_BLOCKS];
    bool onTable[MAX_BLOCKS];
    bool clear[MAX_BLOCKS];
    int holding;
};

// ---------------- STATE HASH (for visited) ----------------
string hashState(const State &s) {
    string h = "";
    for (int i = 0; i < MAX_BLOCKS; i++) {
        h += to_string(s.on[i]) + ",";
        h += to_string(s.onTable[i]) + ",";
        h += to_string(s.clear[i]) + ",";
    }
    h += to_string(s.holding);
    return h;
}

// ---------------- PRINT ----------------
void printState(const State &s, const string &name) {
    cout << name << ":\n";
    for (int i = 0; i < MAX_BLOCKS; ++i) {
        cout << "Block " << char('A' + i) << ": ";
        if (s.onTable[i]) cout << "on table ";
        if (s.on[i] != -1) cout << "on " << char('A' + s.on[i]) << " ";
        if (s.clear[i]) cout << "clear ";
        cout << "\n";
    }
    if (s.holding != -1)
        cout << "Arm holding " << char('A' + s.holding) << "\n";
    else
        cout << "Arm empty\n";
    cout << "-----------------\n";
}

// ---------------- STATES ----------------
State makeInitialState() {
    State s;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        s.on[i] = -1;
        s.onTable[i] = false;
        s.clear[i] = true;
    }

    s.onTable[0] = true;
    s.onTable[2] = true;
    s.onTable[3] = true;

    s.on[1] = 0;

    s.clear[0] = false;
    s.clear[1] = true;
    s.clear[2] = true;
    s.clear[3] = true;

    s.holding = -1;
    return s;
}

State makeGoalState() {
    State s;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        s.on[i] = -1;
        s.onTable[i] = false;
        s.clear[i] = true;
    }

    s.onTable[0] = true;
    s.on[1] = 0;
    s.on[2] = 1;
    s.onTable[3] = true;

    s.clear[0] = false;
    s.clear[1] = false;
    s.clear[2] = true;
    s.clear[3] = true;

    s.holding = -1;
    return s;
}

// ---------------- COMPARE ----------------
bool sameState(const State &a, const State &b) {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (a.on[i] != b.on[i]) return false;
        if (a.onTable[i] != b.onTable[i]) return false;
        if (a.clear[i] != b.clear[i]) return false;
    }
    return a.holding == b.holding;
}

// ---------------- ACTIONS ----------------
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
    ns.holding = x;
    ns.clear[y] = true;
    return ns;
}

bool canStack(const State &s, int x, int y) {
    return s.holding == x && s.clear[y];
}

State stack(const State &s, int x, int y) {
    State ns = s;
    ns.holding = -1;
    ns.on[x] = y;
    ns.clear[y] = false;
    return ns;
}

// ---------------- DFS WITH VISITED ----------------
bool dfsPlan(const State &current, const State &goal,
             int depthLimit,
             vector<string> &path,
             set<string> &visited) {

    string h = hashState(current);

    // ✅ VISITED CHECK (IMPORTANT FIX)
    if (visited.count(h)) return false;
    visited.insert(h);

    if (sameState(current, goal)) {
        printState(current, "Goal reached");

        cout << "\nFINAL SOLUTION PATH:\n";
        for (auto &p : path)
            cout << p << endl;

        return true;
    }

    if (depthLimit == 0) return false;

    // PICKUP
    for (int x = 0; x < MAX_BLOCKS; x++) {
        if (canPickupFromTable(current, x)) {
            State next = pickupFromTable(current, x);

            path.push_back("PICKUP " + string(1, char('A' + x)) + " FROM TABLE");

            if (dfsPlan(next, goal, depthLimit - 1, path, visited))
                return true;

            path.pop_back();
        }
    }

    // UNSTACK
    for (int x = 0; x < MAX_BLOCKS; x++) {
        for (int y = 0; y < MAX_BLOCKS; y++) {
            if (x != y && canUnstack(current, x, y)) {
                State next = unstack(current, x, y);

                path.push_back("UNSTACK " + string(1, char('A' + x)) +
                               " FROM " + char('A' + y));

                if (dfsPlan(next, goal, depthLimit - 1, path, visited))
                    return true;

                path.pop_back();
            }
        }
    }

    // PUTDOWN
    if (canPutDownToTable(current)) {
        State next = putDownToTable(current);

        path.push_back("PUTDOWN");

        if (dfsPlan(next, goal, depthLimit - 1, path, visited))
            return true;

        path.pop_back();
    }

    // STACK
    if (current.holding != -1) {
        int x = current.holding;
        for (int y = 0; y < MAX_BLOCKS; y++) {
            if (x != y && canStack(current, x, y)) {
                State next = stack(current, x, y);

                path.push_back("STACK " + string(1, char('A' + x)) +
                               " ON " + char('A' + y));

                if (dfsPlan(next, goal, depthLimit - 1, path, visited))
                    return true;

                path.pop_back();
            }
        }
    }

    return false;
}

// ---------------- MAIN ----------------
int main() {
    State initial = makeInitialState();
    State goal = makeGoalState();

    printState(initial, "Initial state");
    printState(goal, "Goal state");

    vector<string> path;
    set<string> visited;

    int depthLimit = 15;

    if (!dfsPlan(initial, goal, depthLimit, path, visited)) {
        cout << "No plan found within depth limit\n";
    }

    return 0;
}
