#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// Decision tree learning (ID3-style) for the "waiting for a table" problem.
// We use a very small dataset and build a tree automatically from it.

// Attributes:
// 0: alt       (0=no, 1=yes)
// 1: hungry    (0=no, 1=yes)
// 2: patrons   (0=none, 1=some, 2=full)
// 3: price     (0=cheap, 1=normal, 2=expensive)
// 4: wait      (0=0-10, 1=10-30, 2=30-60, 3=>60)
// Target label: waitDecision (0=no, 1=yes)

struct Example {
    int attr[5];   // attributes encoded as small integers
    int label;     // 0 = do not wait, 1 = wait
};

// Very small dataset (toy data, inspired by textbook)
const int N = 8;
Example dataset[N] = {
    {{1, 1, 2, 2, 3}, 0}, // alt=yes, hungry=yes, patrons=full,  price=exp, wait=>60  -> no
    {{1, 1, 2, 1, 1}, 1}, // alt=yes, hungry=yes, patrons=full,  price=norm, wait=10-30 -> yes
    {{0, 1, 1, 0, 0}, 1}, // alt=no,  hungry=yes, patrons=some, price=cheap, wait=0-10 -> yes
    {{1, 0, 1, 2, 0}, 0}, // alt=yes, hungry=no,  patrons=some, price=exp,  wait=0-10 -> no
    {{0, 1, 0, 1, 0}, 0}, // alt=no,  hungry=yes, patrons=none, price=norm, wait=0-10 -> no
    {{0, 0, 2, 1, 2}, 0}, // alt=no,  hungry=no,  patrons=full, price=norm, wait=30-60 -> no
    {{1, 1, 1, 1, 1}, 1}, // alt=yes, hungry=yes, patrons=some, price=norm, wait=10-30 -> yes
    {{1, 0, 2, 0, 1}, 1}  // alt=yes, hungry=no,  patrons=full, price=cheap, wait=10-30 -> yes
};

// Number of possible values for each attribute (for splitting)
int attributeDomainSize[5] = {2, 2, 3, 3, 4};

struct Node {
    bool isLeaf;
    int  classLabel;       // only valid if isLeaf == true (0 or 1)
    int  attributeIndex;   // which attribute to test at this node
    int  numChildren;      // how many children this attribute has
    Node* children[4];     // child for each attribute value (max 4 here)

    Node() {
        isLeaf = false;
        classLabel = 0;
        attributeIndex = -1;
        numChildren = 0;
        for (int i = 0; i < 4; ++i) children[i] = nullptr;
    }
};

double entropy(double pYes, double pNo) {
    auto log2safe = [](double x) {
        if (x <= 0.0) return 0.0;
        return log(x) / log(2.0);
    };
    double h = 0.0;
    if (pYes > 0.0) h -= pYes * log2safe(pYes);
    if (pNo  > 0.0) h -= pNo  * log2safe(pNo);
    return h;
}

// Compute information gain for splitting "examples" on attribute "attrIndex"
double informationGain(const vector<int> &exampleIndices, int attrIndex) {
    if (exampleIndices.empty()) return 0.0;

    int yesCount = 0, noCount = 0;
    for (int idx : exampleIndices) {
        if (dataset[idx].label == 1) yesCount++;
        else noCount++;
    }
    double total = (double)exampleIndices.size();
    double pYes = yesCount / total;
    double pNo  = noCount / total;
    double baseEntropy = entropy(pYes, pNo);

    int k = attributeDomainSize[attrIndex];
    // counts per attribute value
    int valueTotal[4] = {0, 0, 0, 0};
    int valueYes[4]   = {0, 0, 0, 0};
    int valueNo[4]    = {0, 0, 0, 0};

    for (int idx : exampleIndices) {
        int v = dataset[idx].attr[attrIndex];
        valueTotal[v]++;
        if (dataset[idx].label == 1) valueYes[v]++;
        else valueNo[v]++;
    }

    double condEntropy = 0.0;
    for (int v = 0; v < k; ++v) {
        if (valueTotal[v] == 0) continue;
        double py = valueYes[v] / (double)valueTotal[v];
        double pn = valueNo[v]  / (double)valueTotal[v];
        condEntropy += (valueTotal[v] / total) * entropy(py, pn);
    }

    return baseEntropy - condEntropy;
}

// Check if all examples have the same label
bool allSameLabel(const vector<int> &exampleIndices, int &labelOut) {
    if (exampleIndices.empty()) return false;
    labelOut = dataset[exampleIndices[0]].label;
    for (int idx : exampleIndices) {
        if (dataset[idx].label != labelOut) return false;
    }
    return true;
}

// Majority label in a set of examples
int majorityLabel(const vector<int> &exampleIndices) {
    int yesCount = 0, noCount = 0;
    for (int idx : exampleIndices) {
        if (dataset[idx].label == 1) yesCount++;
        else noCount++;
    }
    return (yesCount >= noCount) ? 1 : 0;
}

// ID3-style recursive tree construction
Node* buildTree(const vector<int> &exampleIndices, const vector<int> &remainingAttributes) {
    Node* node = new Node();

    // Stopping condition 1: all examples same label
    int sameLabel;
    if (allSameLabel(exampleIndices, sameLabel)) {
        node->isLeaf = true;
        node->classLabel = sameLabel;
        return node;
    }

    // Stopping condition 2: no attributes left
    if (remainingAttributes.empty()) {
        node->isLeaf = true;
        node->classLabel = majorityLabel(exampleIndices);
        return node;
    }

    // Choose attribute with maximum information gain
    double bestGain = -1.0;
    int bestAttr = -1;
    for (int attr : remainingAttributes) {
        double g = informationGain(exampleIndices, attr);
        if (g > bestGain) {
            bestGain = g;
            bestAttr = attr;
        }
    }

    if (bestAttr == -1) {
        node->isLeaf = true;
        node->classLabel = majorityLabel(exampleIndices);
        return node;
    }

    node->attributeIndex = bestAttr;
    int k = attributeDomainSize[bestAttr];
    node->numChildren = k;

    // Prepare remaining attribute list for children
    vector<int> childAttrs;
    for (int attr : remainingAttributes) {
        if (attr != bestAttr) childAttrs.push_back(attr);
    }

    // Partition examples by attribute value and recurse
    for (int v = 0; v < k; ++v) {
        vector<int> subset;
        for (int idx : exampleIndices) {
            if (dataset[idx].attr[bestAttr] == v) {
                subset.push_back(idx);
            }
        }
        if (subset.empty()) {
            // Leaf with majority label of parent examples
            Node* leaf = new Node();
            leaf->isLeaf = true;
            leaf->classLabel = majorityLabel(exampleIndices);
            node->children[v] = leaf;
        } else {
            node->children[v] = buildTree(subset, childAttrs);
        }
    }

    return node;
}

// Classify a new example by walking the tree
int classify(Node* root, const Example &ex) {
    Node* node = root;
    while (!node->isLeaf) {
        int attr = node->attributeIndex;
        int v = ex.attr[attr];
        if (v < 0 || v >= node->numChildren || node->children[v] == nullptr) {
            // Fallback to majority label if something is wrong
            return 0;
        }
        node = node->children[v];
    }
    return node->classLabel;
}

// Optional: print tree structure (simple text)
void printTree(Node* node, int depth = 0) {
    for (int i = 0; i < depth; ++i) cout << "  ";
    if (node->isLeaf) {
        cout << "Leaf: label=" << (node->classLabel ? "WAIT" : "NO_WAIT") << "\n";
        return;
    }
    cout << "Test attribute " << node->attributeIndex << "\n";
    for (int v = 0; v < node->numChildren; ++v) {
        for (int i = 0; i < depth + 1; ++i) cout << "  ";
        cout << "value " << v << ":\n";
        if (node->children[v]) {
            printTree(node->children[v], depth + 2);
        }
    }
}

int main() {
    cout << "Decision Tree Learning: Restaurant Waiting Problem\n\n";

    // Build tree from the dataset and all attributes {0,1,2,3,4}
    vector<int> exampleIndices;
    for (int i = 0; i < N; ++i) exampleIndices.push_back(i);
    vector<int> attributes = {0, 1, 2, 3, 4};

    Node* root = buildTree(exampleIndices, attributes);

    cout << "Learned decision tree structure:\n";
    printTree(root);
    cout << "\n";

    // Read a new situation from user
    Example ex;
    cout << "Enter attributes for a new situation.\n";
    cout << "Alternate restaurant available? (1=yes, 0=no): ";
    cin >> ex.attr[0];
    cout << "Hungry? (1=yes, 0=no): ";
    cin >> ex.attr[1];
    cout << "Patrons (0=none, 1=some, 2=full): ";
    cin >> ex.attr[2];
    cout << "Price (0=cheap, 1=normal, 2=expensive): ";
    cin >> ex.attr[3];
    cout << "Wait estimate (0=0-10, 1=10-30, 2=30-60, 3=>60): ";
    cin >> ex.attr[4];

    int decision = classify(root, ex);
    if (decision == 1) {
        cout << "Decision: WAIT for a table.\n";
    } else {
        cout << "Decision: DO NOT WAIT.\n";
    }

    return 0;
}


