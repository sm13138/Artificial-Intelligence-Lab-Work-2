#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Example {
    int attr[5];   // attributes
    int label;     // 0 = NO_WAIT, 1 = WAIT
};

const int N = 8;

// training dataset
Example dataset[N] = {
    {{1, 1, 2, 2, 3}, 0},
    {{1, 1, 2, 1, 1}, 1},
    {{0, 1, 1, 0, 0}, 1},
    {{1, 0, 1, 2, 0}, 0},
    {{0, 1, 0, 1, 0}, 0},
    {{0, 0, 2, 1, 2}, 0},
    {{1, 1, 1, 1, 1}, 1},
    {{1, 0, 2, 0, 1}, 1}
};

int attributeDomainSize[5] = {2, 2, 3, 3, 4};

struct Node {
    bool isLeaf;
    int classLabel;
    int attributeIndex;
    int numChildren;
    Node* children[4];

    Node() {
        isLeaf = false;
        classLabel = 0;
        attributeIndex = -1;
        numChildren = 0;
        for (int i = 0; i < 4; i++) children[i] = nullptr;
    }
};

// entropy calculation
double entropy(double pYes, double pNo) {
    auto log2safe = [](double x) {
        if (x <= 0) return 0.0;
        return log(x) / log(2.0);
    };

    double h = 0;
    if (pYes > 0) h -= pYes * log2safe(pYes);
    if (pNo > 0) h -= pNo * log2safe(pNo);
    return h;
}

// information gain calculation
double informationGain(const vector<int>& idx, int attr) {
    int yes = 0, no = 0;

    for (int i : idx)
        dataset[i].label ? yes++ : no++;

    double total = idx.size();
    double base = entropy(yes / total, no / total);

    int valT[4] = {0}, valY[4] = {0}, valN[4] = {0};

    for (int i : idx) {
        int v = dataset[i].attr[attr];
        valT[v]++;
        dataset[i].label ? valY[v]++ : valN[v]++;
    }

    double cond = 0;

    for (int v = 0; v < attributeDomainSize[attr]; v++) {
        if (!valT[v]) continue;
        double pY = valY[v] / (double)valT[v];
        double pN = valN[v] / (double)valT[v];
        cond += (valT[v] / total) * entropy(pY, pN);
    }

    return base - cond;
}

// check pure node
bool allSameLabel(const vector<int>& idx, int &label) {
    label = dataset[idx[0]].label;
    for (int i : idx)
        if (dataset[i].label != label) return false;
    return true;
}

// majority voting
int majorityLabel(const vector<int>& idx) {
    int y = 0, n = 0;
    for (int i : idx)
        dataset[i].label ? y++ : n++;
    return (y >= n);
}

// ID3 tree building
Node* buildTree(const vector<int>& idx, const vector<int>& attrs) {
    Node* node = new Node();

    int label;
    if (allSameLabel(idx, label)) {
        node->isLeaf = true;
        node->classLabel = label;
        return node;
    }

    if (attrs.empty()) {
        node->isLeaf = true;
        node->classLabel = majorityLabel(idx);
        return node;
    }

    double bestGain = -1;
    int bestAttr = -1;

    for (int a : attrs) {
        double g = informationGain(idx, a);
        if (g > bestGain) {
            bestGain = g;
            bestAttr = a;
        }
    }

    node->attributeIndex = bestAttr;
    node->numChildren = attributeDomainSize[bestAttr];

    vector<int> newAttrs;
    for (int a : attrs)
        if (a != bestAttr) newAttrs.push_back(a);

    for (int v = 0; v < node->numChildren; v++) {
        vector<int> subset;

        for (int i : idx)
            if (dataset[i].attr[bestAttr] == v)
                subset.push_back(i);

        if (subset.empty()) {
            Node* leaf = new Node();
            leaf->isLeaf = true;
            leaf->classLabel = majorityLabel(idx);
            node->children[v] = leaf;
        } else {
            node->children[v] = buildTree(subset, newAttrs);
        }
    }

    return node;
}

// classification
int classify(Node* root, const Example &ex) {
    Node* node = root;

    while (!node->isLeaf) {
        int a = node->attributeIndex;
        int v = ex.attr[a];
        node = node->children[v];
    }

    return node->classLabel;
}

// print tree
void printTree(Node* node, int depth = 0) {
    for (int i = 0; i < depth; i++) cout << "  ";

    if (node->isLeaf) {
        cout << "Leaf: " << (node->classLabel ? "WAIT" : "NO_WAIT") << "\n";
        return;
    }

    cout << "Test attribute " << node->attributeIndex << "\n";

    for (int v = 0; v < node->numChildren; v++) {
        for (int i = 0; i < depth + 1; i++) cout << "  ";
        cout << "value " << v << ":\n";
        if (node->children[v])
            printTree(node->children[v], depth + 2);
    }
}

int main() {
    vector<int> idx, attrs = {0,1,2,3,4};

    for (int i = 0; i < N; i++) idx.push_back(i);

    Node* root = buildTree(idx, attrs);

    cout << "Decision Tree:\n";
    printTree(root);

    Example ex;

    cout << "\nEnter input:\n";
    cin >> ex.attr[0] >> ex.attr[1] >> ex.attr[2] >> ex.attr[3] >> ex.attr[4];

    int res = classify(root, ex);

    cout << (res ? "WAIT" : "NO WAIT") << endl;

    return 0;
}
