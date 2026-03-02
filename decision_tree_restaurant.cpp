#include <iostream>
using namespace std;

// Simple decision tree for the "waiting for a table" problem.
// We use a hard-coded tree learned from examples (conceptually like ID3),
// and also show a very small entropy/information gain calculation on a toy dataset.

// Attributes (simplified version of the classic example):
// - Alternate (alt): is there an alternative restaurant? (yes/no)
// - Hungry (hungry): is the customer hungry? (yes/no)
// - Patrons (patrons): number of people in the restaurant (0/some/full)
// - Price (price): cheap/normal/expensive
// - WaitEstimate (wait): estimated waiting time (0-10 / 10-30 / 30-60 / >60)
// Target: Wait? (yes/no)

struct Example {
    bool alt;
    bool hungry;
    int patrons;   // 0 = none, 1 = some, 2 = full
    int price;     // 0 = cheap, 1 = normal, 2 = expensive
    int wait;      // 0 = 0-10, 1 = 10-30, 2 = 30-60, 3 = >60
    bool waitDecision;
};

// Very small dataset (inspired by textbook)
const int N = 4;
Example dataset[N] = {
    {true,  true,  2, 2, 3, false},
    {true,  true,  2, 1, 1, true},
    {false, true,  1, 0, 0, true},
    {true,  false, 1, 2, 0, false}
};

// Compute entropy H(Y) for binary classification.
double entropy(double pYes, double pNo) {
    auto log2safe = [](double x) {
        if (x <= 0.0) return 0.0;
        return log(x) / log(2.0);
    };
    double h = 0.0;
    if (pYes > 0) h -= pYes * log2safe(pYes);
    if (pNo > 0)  h -= pNo * log2safe(pNo);
    return h;
}

// Compute information gain of splitting on "hungry" attribute.
double infoGainOnHungry() {
    int yesCount = 0, noCount = 0;
    for (int i = 0; i < N; ++i) {
        if (dataset[i].waitDecision) yesCount++;
        else noCount++;
    }
    double total = N;
    double pYes = yesCount / total;
    double pNo = noCount / total;
    double baseEntropy = entropy(pYes, pNo);

    // Split by hungry = true/false
    int hungryYesY = 0, hungryYesN = 0, hungryYesTotal = 0;
    int hungryNoY = 0, hungryNoN = 0, hungryNoTotal = 0;
    for (int i = 0; i < N; ++i) {
        if (dataset[i].hungry) {
            hungryYesTotal++;
            if (dataset[i].waitDecision) hungryYesY++; else hungryYesN++;
        } else {
            hungryNoTotal++;
            if (dataset[i].waitDecision) hungryNoY++; else hungryNoN++;
        }
    }

    double condEntropy = 0.0;
    if (hungryYesTotal > 0) {
        double py = hungryYesY / (double)hungryYesTotal;
        double pn = hungryYesN / (double)hungryYesTotal;
        condEntropy += (hungryYesTotal / total) * entropy(py, pn);
    }
    if (hungryNoTotal > 0) {
        double py = hungryNoY / (double)hungryNoTotal;
        double pn = hungryNoN / (double)hungryNoTotal;
        condEntropy += (hungryNoTotal / total) * entropy(py, pn);
    }

    return baseEntropy - condEntropy;
}

// Hard-coded decision tree for demonstration:
// (simplified from the classic restaurant example)
//
// if (patrons == 0) -> don't wait
// else if (patrons == 1) -> wait
// else // patrons == full
//    if (price == expensive && wait == long) -> don't wait
//    else -> wait

bool classifyWaitDecision(bool alt, bool hungry, int patrons, int price, int wait) {
    (void)alt;    // unused in this simplified tree
    (void)hungry; // unused in this simplified tree

    if (patrons == 0) return false;
    if (patrons == 1) return true;
    // patrons == full
    if (price == 2 && (wait == 2 || wait == 3)) {
        return false;
    }
    return true;
}

int main() {
    cout << "Decision Tree: Restaurant Waiting Problem\n\n";

    double gain = infoGainOnHungry();
    cout << "Information gain for splitting on 'Hungry': " << gain << "\n\n";

    bool alt, hungry;
    int patrons, price, wait;

    cout << "Enter attributes for a new situation.\n";
    cout << "Alternate restaurant available? (1=yes, 0=no): ";
    cin >> alt;
    cout << "Hungry? (1=yes, 0=no): ";
    cin >> hungry;
    cout << "Patrons (0=none, 1=some, 2=full): ";
    cin >> patrons;
    cout << "Price (0=cheap, 1=normal, 2=expensive): ";
    cin >> price;
    cout << "Wait estimate (0=0-10, 1=10-30, 2=30-60, 3=>60): ";
    cin >> wait;

    bool decision = classifyWaitDecision(alt, hungry, patrons, price, wait);
    if (decision) {
        cout << "Decision: WAIT for a table.\n";
    } else {
        cout << "Decision: DO NOT WAIT.\n";
    }

    return 0;
}

