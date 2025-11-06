#include <BPlusTree.h>
BPlusTree::~BPlusTree() {
    clear();
}

void BPlusTree::clear() {
    deleteTree(root);
    root = nullptr;
}

void BPlusTree::deleteTree(Node* node) {
    if (!node) return;
    
    if (!node->isLeaf) {
        for (Node* child : node->children) {
            deleteTree(child);
        }
    }
    delete node;
}

Node* BPlusTree::findLeaf(int key) {
    if (!root) return nullptr;
    
    Node* current = root;
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->keys.size() && key >= current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }
    return current;
}


void BPlusTree::splitChild(Node* parent, int index, Node* child) {
    Node* newNode = new Node(child->isLeaf);
    
    if (child->isLeaf) {

        int mid = order - 1;
        for (int i = mid; i < child->keys.size(); i++) {
            newNode->keys.push_back(child->keys[i]);
            newNode->values.push_back(child->values[i]);
        }
        child->keys.erase(child->keys.begin() + mid, child->keys.end());
        child->values.erase(child->values.begin() + mid, child->values.end());
        
        // Update leaf node links
        newNode->next = child->next;
        if (newNode->next) {
            newNode->next->prev = newNode;
        } 
        child->next = newNode;
        newNode->prev = child;
        
        parent->keys.insert(parent->keys.begin() + index, newNode->keys[0]);
    } 
    else {
        int mid = order - 1;
        for (int i = mid + 1; i < child->keys.size(); i++) {
            newNode->keys.push_back(child->keys[i]);
        }
        parent->keys.insert(parent->keys.begin() + index, child->keys[mid]);
        child->keys.erase(child->keys.begin() + mid, child->keys.end());
        
        for (int i = mid + 1; i < child->children.size(); i++) {
            newNode->children.push_back(child->children[i]);
        }
        child->children.erase(child->children.begin() + mid + 1, child->children.end());
    }
    
    parent->children.insert(parent->children.begin() + index + 1, newNode);
}



void BPlusTree::insertNonFull(Node* node, int key, int pageIndex, int rowIndex) {
    int i = node->keys.size() - 1;
    
    if (node->isLeaf) {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        node->keys.insert(node->keys.begin() + i + 1, key);
        node->values.insert(node->values.begin() + i + 1, PageRowPair(pageIndex, rowIndex));
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        
        if (node->children[i]->keys.size() == 2 * order - 1) {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key, pageIndex, rowIndex);
    }
}

void BPlusTree::insert(int key, int pageIndex, int rowIndex) {
    if (!root) {
        root = new Node(true);
        root->keys.push_back(key);
        root->values.push_back(PageRowPair(pageIndex, rowIndex));
        return;
    }

    if (root->keys.size() == 2 * order - 1) {
        Node* newRoot = new Node(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0, root);
        root = newRoot;
        insertNonFull(root, key, pageIndex, rowIndex);
        return;
    } 
    insertNonFull(root, key, pageIndex, rowIndex);
    

}

PageRowPair BPlusTree::search(int key) {
    Node* leaf = findLeaf(key);
    if (!leaf) return PageRowPair(-1, -1);
    
    auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    if (it != leaf->keys.end() && *it == key) {
        int pos = it - leaf->keys.begin();
        return leaf->values[pos];
    }
    return PageRowPair(-1, -1);
}

// void BPlusTree::printTree() {
//     if (!root) {
//         cout << "Tree is empty" << endl;
//         return;
//     }
    
//     vector<Node*> currentLevel = {root};
//     while (!currentLevel.empty()) {
//         vector<Node*> nextLevel;
//         for (Node* node : currentLevel) {
//             cout << "[";
//             for (int i = 0; i < node->keys.size(); i++) {
//                 cout << node->keys[i];
//                 if (i < node->keys.size() - 1) cout << " ";
//             }
//             cout << "] ";
            
//             if (!node->isLeaf) {
//                 nextLevel.insert(nextLevel.end(), node->children.begin(), node->children.end());
//             }
//         }
//         cout << endl;
//         currentLevel = nextLevel;
//     }
// }
void printTreeRec(Node* node, int level) {
    if (!node) return;

    // Print current node
    cout << "Level " << level << ": ";
    for (int i = 0; i < node->keys.size(); i++) {
        cout << node->keys[i] << " ";
    }
    cout << endl;

    if (!node->isLeaf) {
        for (int i = 0; i <= node->keys.size(); i++) {
            if (node->children[i]) {
                printTreeRec(node->children[i], level + 1);
            }
        }
    }
}
Node* BPlusTree::getRoot() {
    return root;
}
void BPlusTree::printTree() {
    Node* node  = this->getRoot();
    int level = 0;
    printTreeRec(node, level);
}

void BPlusTree::printLeafNodes() {
    if (!root) {
        cout << "Tree is empty" << endl;
        return;
    }

    // Find the leftmost leaf node
    Node* current = root;
    while (!current->isLeaf) {
        current = current->children[0];
    }

    // Traverse through all leaf nodes using next pointer
    cout << "Leaf Nodes:" << endl;
    cout << "-----------" << endl;
    while (current) {
        cout << "Node: [";
        for (int i = 0; i < current->keys.size(); i++) {
            cout << current->keys[i];
            if (i < current->keys.size() - 1) {
                cout << ", ";
            }
        }
        cout << "]";
        
        // Print values (page and row indices)
        cout << " Values: [";
        for (int i = 0; i < current->values.size(); i++) {
            cout << "(" << current->values[i].pageIndex << "," << current->values[i].rowIndex << ")";
            if (i < current->values.size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;

        current = current->next;
    }
    cout << "-----------" << endl;
}

void BPlusTree::saveToFile(string filename) {
    string fullFilename = filename;
    if (fullFilename.find('.') == string::npos) {
        fullFilename += ".bpt";
    }
    
    ofstream file(fullFilename);
    if (!file.is_open()) {
        cout << "Error opening file for writing\n";
        return;
    }

    // Save tree structure using the new serialization format
    function<void(Node*)> saveNode = [&](Node* node) {
        if (!node) {
            file << "null\n";
            return;
        }

        // Write node type
        file << (node->isLeaf ? "LEAF" : "INTERNAL") << "\n";
        
        // Write number of keys
        file << "KEYS: " << node->keys.size() << "\n";
        
        // Write keys
        file << "VALUES: ";
        for (int key : node->keys) {
            file << key << " ";
        }
        file << "\n";
        
        if (node->isLeaf) {
            // Write page and row indices
            file << "PAGES: ";
            for (const PageRowPair& pair : node->values) {
                file << pair.pageIndex << " " << pair.rowIndex << " ";
            }
            file << "\n";
            
            // Write next and prev pointers
            file << "NEXT: " << (node->next ? "1" : "0") << "\n";
            file << "PREV: " << (node->prev ? "1" : "0") << "\n";
        } else {
            // Write children
            file << "CHILDREN: " << node->children.size() << "\n";
            for (Node* child : node->children) {
                saveNode(child);
            }
        }
        file << "END_NODE\n";
    };

    saveNode(root);
    file.close();
}

Node* BPlusTree::loadNode(ifstream& file) {
    string line;
    if (!getline(file, line)) return nullptr;

    if (line == "null") return nullptr;

    Node* node = new Node(false);
    
    // Read node type
    node->isLeaf = (line == "LEAF");
    
    // Read number of keys
    getline(file, line);
    size_t pos = line.find(": ");
    int numKeys = stoi(line.substr(pos + 2));
    
    // Read keys
    getline(file, line);
    pos = line.find(": ");
    stringstream ss(line.substr(pos + 2));
    node->keys.resize(numKeys);
    for (int i = 0; i < numKeys; i++) {
        ss >> node->keys[i];
    }
    
    if (node->isLeaf) {
        // Read page and row indices
        getline(file, line);
        pos = line.find(": ");
        ss = stringstream(line.substr(pos + 2));
        node->values.resize(numKeys);
        for (int i = 0; i < numKeys; i++) {
            int pageIndex, rowIndex;
            ss >> pageIndex >> rowIndex;
            node->values[i] = PageRowPair(pageIndex, rowIndex);
        }
        
        // Read next and prev pointers
        getline(file, line);
        pos = line.find(": ");
        bool hasNext = (line.substr(pos + 2) == "1");
        
        getline(file, line);
        pos = line.find(": ");
        bool hasPrev = (line.substr(pos + 2) == "1");
        
        // We'll set these pointers later in loadFromFile
        node->next = nullptr;
        node->prev = nullptr;
    } else {
        // Read children
        getline(file, line);
        pos = line.find(": ");
        int numChildren = stoi(line.substr(pos + 2));
        node->children.resize(numChildren);
        for (int i = 0; i < numChildren; i++) {
            node->children[i] = loadNode(file);
            if (node->children[i]) {
                node->children[i]->parent = node;
            }
        }
    }
    
    // Read END_NODE marker
    getline(file, line);
    
    return node;
}

BPlusTree* BPlusTree::loadFromFile(string filename) {
    string fullFilename = filename;
    if (fullFilename.find('.') == string::npos) {
        fullFilename += ".bpt";
    }
    
    ifstream file(fullFilename);
    if (!file.is_open()) {
        return nullptr;
    }

    // Clear existing tree if any
    if (root) {
        clear();
    }

    this->root = loadNode(file);
    file.close();

    // Validate the loaded tree
    if (!root) {
        return nullptr;
    }

    // Reconstruct leaf node links
    vector<Node*> leafNodes;
    function<void(Node*)> collectLeaves = [&](Node* node) {
        if (!node) return;
        if (node->isLeaf) {
            leafNodes.push_back(node);
            return;
        }
        for (Node* child : node->children) {
            collectLeaves(child);
        }
    };
    collectLeaves(root);

    // Sort leaf nodes by their first key to maintain order
    sort(leafNodes.begin(), leafNodes.end(), [](Node* a, Node* b) {
        return a->keys[0] < b->keys[0];
    });

    // Reconstruct next and prev pointers
    for (size_t i = 0; i < leafNodes.size(); i++) {
        if (i > 0) {
            leafNodes[i]->prev = leafNodes[i-1];
        }
        if (i < leafNodes.size() - 1) {
            leafNodes[i]->next = leafNodes[i+1];
        }
    }

    return this;
}

void BPlusTree::generateDotFile(string filename) {
    ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        cout << "Error opening file for writing dot file\n";
        return;
    }

    dotFile << "digraph BPlusTree {\n";
    dotFile << "  node [shape=record, height=.1];\n";
    dotFile << "  edge [arrowtail=dot, dir=both];\n\n";

    // First pass: collect all leaf nodes
    vector<Node*> leafNodes;
    function<void(Node*)> collectLeaves = [&](Node* node) {
        if (!node) return;
        if (node->isLeaf) {
            leafNodes.push_back(node);
            return;
        }
        for (Node* child : node->children) {
            collectLeaves(child);
        }
    };
    collectLeaves(root);

    // Function to recursively generate node definitions
    function<void(Node*)> generateNode = [&](Node* node) {
        if (!node) return;

        // Generate unique node ID
        string nodeId = "node" + to_string(reinterpret_cast<uintptr_t>(node));
        
        // Create node label with keys
        string label = "<f0>";
        for (int i = 0; i < node->keys.size(); i++) {
            label += " | <f" + to_string(i + 1) + "> " + to_string(node->keys[i]);
        }
        label += " | <f" + to_string(node->keys.size() + 1) + ">";
        
        // Write node definition
        dotFile << "  " << nodeId << " [label=\"" << label << "\"];\n";
        
        if (!node->isLeaf) {
            // Recursively generate child nodes and their connections
            for (int i = 0; i < node->children.size(); i++) {
                string childId = "node" + to_string(reinterpret_cast<uintptr_t>(node->children[i]));
                dotFile << "  " << nodeId << ":f" << i << " -> " << childId << ";\n";
                generateNode(node->children[i]);
            }
        }
    };

    // Generate all nodes first
    if (root) {
        generateNode(root);
    }

    // Add leaf node connections
    dotFile << "\n  // Leaf node connections\n";
    for (Node* leaf : leafNodes) {
        string leafId = "node" + to_string(reinterpret_cast<uintptr_t>(leaf));
        
        if (leaf->next) {
            string nextId = "node" + to_string(reinterpret_cast<uintptr_t>(leaf->next));
            dotFile << "  " << leafId << " -> " << nextId << " [constraint=false, style=dashed, color=blue, label=\"next\"];\n";
        }
        if (leaf->prev) {
            string prevId = "node" + to_string(reinterpret_cast<uintptr_t>(leaf->prev));
            dotFile << "  " << leafId << " -> " << prevId << " [constraint=false, style=dashed, color=red, label=\"prev\"];\n";
        }
    }

    dotFile << "}\n";
    dotFile.close();
}

// Deletion methods
void BPlusTree::updateInternalKeys(Node* node, int oldKey, int newKey) {
    if (!node || node->isLeaf) return;

    // Update keys in current node
    for (int i = 0; i < node->keys.size(); i++) {
        if (node->keys[i] == oldKey) {
            node->keys[i] = newKey;
        }
    }

    // Recursively update keys in children
    for (Node* child : node->children) {
        updateInternalKeys(child, oldKey, newKey);
    }
}

// Helper function to find first position of key in a leaf node
int BPlusTree::findFirstPositionOfKey(Node* leaf, int key) {
    if (!leaf) return -1;
    for (int i = 0; i < leaf->keys.size(); i++) {
        if (leaf->keys[i] == key) {
            return i;
        }
    }
    return -1;
}

// Helper function to get next available key for internal node updates
int BPlusTree::getNextKey(Node* leaf) {
    if (!leaf) return -1;
    
    // If leaf has more keys, use the next one
    if (leaf->keys.size() > 1) {
        return leaf->keys[1];
    }
    
    // If leaf has a next sibling, use its first key
    if (leaf->next && !leaf->next->keys.empty()) {
        return leaf->next->keys[0];
    }
    
    return -1;
}

// Helper function to find child index in parent node
int BPlusTree::findChildIndex(Node* parent, Node* child) {
    if (!parent || !child) return -1;
    for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == child) {
            return i;
        }
    }
    return -1;
}

// Function to delete first instance of a key
bool BPlusTree::deleteFirstInstance(int key) {
    Node* leaf = findLeaf(key);
    if (!leaf) return false;
    
    int pos = findFirstPositionOfKey(leaf, key);
    if (pos == -1) return false;
    
    // If this is the first key in leaf and it's not the leftmost leaf
    if (pos == 0 && leaf->prev != nullptr) {
        int oldKey = leaf->keys[0];
        int newKey = getNextKey(leaf);
        if (newKey != -1) {
            updateInternalKeys(root, oldKey, newKey);
        }
    }
    
    // Remove the key-value pair
    leaf->keys.erase(leaf->keys.begin() + pos);
    leaf->values.erase(leaf->values.begin() + pos);
    
    return true;
}

// Function to update the entire tree
void BPlusTree::updateTree() {
    // 1. Update Leaf Nodes
    Node* current = NULL;
    while (current) {
        if (current != root && current->keys.size() < order - 1) {
            handleLeafUnderflow(current);
        }
        current = current->next;
    }
    
    // 2. Update Internal Nodes
    updateInternalNodesRecursive(root);
    
    // 3. Update Root
    while (root && !root->isLeaf && root->children.size() == 1) {
        Node* oldRoot = root;
        root = root->children[0];
        delete oldRoot;
    }
    
    if (root && root->keys.empty()) {
        if (root->isLeaf) {
            delete root;
            root = nullptr;
        } else if (root->children.size() == 1) {
            Node* newRoot = root->children[0];
            delete root;
            root = newRoot;
        }
    }
}

// Function to handle leaf node underflow
void BPlusTree::handleLeafUnderflow(Node* leaf) {
    if (!leaf || leaf == root) return;
    
    Node* parent = findParent(root, leaf);
    int nodeIndex = findChildIndex(parent, leaf);
    
    // Try borrowing from left sibling
    if (nodeIndex > 0) {
        Node* leftSibling = parent->children[nodeIndex - 1];
        if (leftSibling && leftSibling->keys.size() > order - 1) {
            // Borrow from left
            leaf->keys.insert(leaf->keys.begin(), 
                leftSibling->keys[leftSibling->keys.size() - 1]);
            leaf->values.insert(leaf->values.begin(), 
                leftSibling->values[leftSibling->values.size() - 1]);
            leftSibling->keys.pop_back();
            leftSibling->values.pop_back();
            
            // Update parent key
            parent->keys[nodeIndex - 1] = leaf->keys[0];
            return;
        }
    }
    
    // Try borrowing from right sibling
    if (nodeIndex < parent->children.size() - 1) {
        Node* rightSibling = parent->children[nodeIndex + 1];
        if (rightSibling && rightSibling->keys.size() > order - 1) {
            // Borrow from right
            leaf->keys.push_back(rightSibling->keys[0]);
            leaf->values.push_back(rightSibling->values[0]);
            rightSibling->keys.erase(rightSibling->keys.begin());
            rightSibling->values.erase(rightSibling->values.begin());
            
            // Update parent key
            parent->keys[nodeIndex] = rightSibling->keys[0];
            return;
        }
    }
    
    // If can't borrow, merge with sibling
    if (nodeIndex > 0) {
        // Merge with left sibling
        Node* leftSibling = parent->children[nodeIndex - 1];
        mergeWithLeftSibling(leaf, leftSibling);
        parent->keys.erase(parent->keys.begin() + nodeIndex - 1);
        parent->children.erase(parent->children.begin() + nodeIndex);
        delete leaf;
    } else {
        // Merge with right sibling
        Node* rightSibling = parent->children[nodeIndex + 1];
        mergeWithRightSibling(leaf, rightSibling);
        parent->keys.erase(parent->keys.begin() + nodeIndex);
        parent->children.erase(parent->children.begin() + nodeIndex + 1);
        delete rightSibling;
    }
    
    // Handle parent underflow if needed
    if (parent != root && parent->keys.size() < order - 1) {
        handleInternalNodeUnderflow(parent);
    }
}

// Function to recursively update internal nodes
void BPlusTree::updateInternalNodesRecursive(Node* node) {
    if (!node || node->isLeaf) return;
    
    // Update keys in current node
    for (int i = 0; i < node->keys.size(); i++) {
        if (i + 1 < node->children.size()) {
            Node* child = node->children[i + 1];
            while (!child->isLeaf) {
                child = child->children[0];
            }
            node->keys[i] = child->keys[0];
        }
    }
    
    // Recursively update children
    for (Node* child : node->children) {
        updateInternalNodesRecursive(child);
    }
    
    // Handle underflow in internal node
    if (node != root && node->keys.size() < order - 1) {
        handleInternalNodeUnderflow(node);
    }
}

// Main remove function that deletes all instances of a key
void BPlusTree::remove(int key) {
    if (!root) {
        cout << "Tree is empty, nothing to delete" << endl;
        return;
    }
    
    int count = 0;
    while (true) {
        if (!deleteFirstInstance(key)) {
            break;
        }
        count++;
    }
    
    if (count > 0) {
        updateTree();
        cout << "Removed " << count << " instance(s) of key " << key << " from the tree" << endl;
    } else {
        cout << "Key " << key << " not found in the tree" << endl;
    }
}

void BPlusTree::handleInternalNodeUnderflow(Node* node) {
    Node* parent = findParent(root, node);
    int nodeIndex = -1;
    for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == node) {
            nodeIndex = i;
            break;
        }
    }

    // Try borrowing from left sibling
    if (nodeIndex > 0) {
        Node* leftSibling = parent->children[nodeIndex - 1];
        if (leftSibling->keys.size() > order - 1) {
            // Move parent key down
            node->keys.insert(node->keys.begin(), parent->keys[nodeIndex - 1]);
            
            // Move rightmost child from left sibling
            node->children.insert(node->children.begin(), 
                leftSibling->children[leftSibling->children.size() - 1]);
            
            // Move rightmost key from left sibling to parent
            parent->keys[nodeIndex - 1] = leftSibling->keys[leftSibling->keys.size() - 1];
            
            leftSibling->keys.pop_back();
            leftSibling->children.pop_back();
            return;
        }
    }

    // Try borrowing from right sibling
    if (nodeIndex < parent->children.size() - 1) {
        Node* rightSibling = parent->children[nodeIndex + 1];
        if (rightSibling->keys.size() > order - 1) {
            // Move parent key down
            node->keys.push_back(parent->keys[nodeIndex]);
            
            // Move leftmost child from right sibling
            node->children.push_back(rightSibling->children[0]);
            
            // Move leftmost key from right sibling to parent
            parent->keys[nodeIndex] = rightSibling->keys[0];
            
            rightSibling->keys.erase(rightSibling->keys.begin());
            rightSibling->children.erase(rightSibling->children.begin());
            return;
        }
    }

    // Merge with a sibling
    if (nodeIndex > 0) {
        // Merge with left sibling
        Node* leftSibling = parent->children[nodeIndex - 1];
        
        // Add parent key to left sibling
        leftSibling->keys.push_back(parent->keys[nodeIndex - 1]);
        
        // Move all keys and children from node to left sibling
        leftSibling->keys.insert(leftSibling->keys.end(), 
            node->keys.begin(), node->keys.end());
        leftSibling->children.insert(leftSibling->children.end(), 
            node->children.begin(), node->children.end());
        
        // Remove parent key and pointer
        parent->keys.erase(parent->keys.begin() + nodeIndex - 1);
        parent->children.erase(parent->children.begin() + nodeIndex);
        
        delete node;
        
        // Handle parent underflow if needed
        if (parent != root && parent->keys.size() < order - 1) {
            handleInternalNodeUnderflow(parent);
        } else if (parent == root && parent->keys.empty()) {
            delete root;
            root = leftSibling;
        }
    } else {
        // Merge with right sibling
        Node* rightSibling = parent->children[nodeIndex + 1];
        
        // Add parent key to node
        node->keys.push_back(parent->keys[nodeIndex]);
        
        // Move all keys and children from right sibling
        node->keys.insert(node->keys.end(), 
            rightSibling->keys.begin(), rightSibling->keys.end());
        node->children.insert(node->children.end(), 
            rightSibling->children.begin(), rightSibling->children.end());
        
        // Remove parent key and pointer
        parent->keys.erase(parent->keys.begin() + nodeIndex);
        parent->children.erase(parent->children.begin() + nodeIndex + 1);
        
        delete rightSibling;
        
        // Handle parent underflow if needed
        if (parent != root && parent->keys.size() < order - 1) {
            handleInternalNodeUnderflow(parent);
        } else if (parent == root && parent->keys.empty()) {
            delete root;
            root = node;
        }
    }
}

void BPlusTree::deleteFromLeaf(Node* leaf, int key) {
    // Find all occurrences of the key and remove them
    for (int i = leaf->keys.size() - 1; i >= 0; i--) {
        if (leaf->keys[i] == key) {
            leaf->keys.erase(leaf->keys.begin() + i);
            leaf->values.erase(leaf->values.begin() + i);
        }
    }
}

void BPlusTree::deleteFromInternal(Node* node, int key) {
    // Find the key in the internal node
    int keyIndex = -1;
    for (int i = 0; i < node->keys.size(); i++) {
        if (node->keys[i] == key) {
            keyIndex = i;
            break;
        }
    }
    
    if (keyIndex == -1) {
        // Key not found in this node, find the appropriate child
        int i = 0;
        while (i < node->keys.size() && key >= node->keys[i]) {
            i++;
        }
        deleteFromInternal(node->children[i], key);
        return;
    }
    
    // Key found in this node
    // Find the predecessor (maximum key in the left subtree)
    Node* predecessor = node->children[keyIndex];
    while (!predecessor->isLeaf) {
        predecessor = predecessor->children[predecessor->keys.size()];
    }
    
    // Replace the key with the predecessor
    int predecessorKey = predecessor->keys[predecessor->keys.size() - 1];
    node->keys[keyIndex] = predecessorKey;
    
    // Delete the predecessor from the leaf node
    deleteFromLeaf(predecessor, predecessorKey);
    
    // Handle underflow if necessary
    if (predecessor != root && predecessor->keys.size() < order - 1) {
        handleUnderflow(predecessor);
    }
}

void BPlusTree::borrowFromLeftSibling(Node* node, Node* leftSibling) {
    // Move the last key from the left sibling to the current node
    node->keys.insert(node->keys.begin(), leftSibling->keys.back());
    leftSibling->keys.pop_back();
    
    if (node->isLeaf) {
        // Move the corresponding value
        node->values.insert(node->values.begin(), leftSibling->values.back());
        leftSibling->values.pop_back();
    } else {
        // Move the last child from the left sibling to the current node
        Node* child = leftSibling->children.back();
        node->children.insert(node->children.begin(), child);
        leftSibling->children.pop_back();
    }
    
    // Update the parent key
    Node* parent = findParent(root, node);
    for (int i = 0; i < parent->keys.size(); i++) {
        if (parent->children[i + 1] == node) {
            parent->keys[i] = node->keys[0];
            break;
        }
    }
}

void BPlusTree::borrowFromRightSibling(Node* node, Node* rightSibling) {
    // Move the first key from the right sibling to the current node
    node->keys.push_back(rightSibling->keys.front());
    rightSibling->keys.erase(rightSibling->keys.begin());
    
    if (node->isLeaf) {
        // Move the corresponding value
        node->values.push_back(rightSibling->values.front());
        rightSibling->values.erase(rightSibling->values.begin());
    } else {
        // Move the first child from the right sibling to the current node
        Node* child = rightSibling->children.front();
        node->children.push_back(child);
        rightSibling->children.erase(rightSibling->children.begin());
    }
    
    // Update the parent key
    Node* parent = findParent(root, node);
    for (int i = 0; i < parent->keys.size(); i++) {
        if (parent->children[i] == node) {
            parent->keys[i] = rightSibling->keys[0];
            break;
        }
    }
}

void BPlusTree::mergeWithLeftSibling(Node* node, Node* leftSibling) {
    // Move all keys and values from the current node to the left sibling
    for (int i = 0; i < node->keys.size(); i++) {
        leftSibling->keys.push_back(node->keys[i]);
        if (node->isLeaf) {
            leftSibling->values.push_back(node->values[i]);
        }
    }
    
    if (!node->isLeaf) {
        // Move all children from the current node to the left sibling
        for (int i = 0; i < node->children.size(); i++) {
            leftSibling->children.push_back(node->children[i]);
        }
    }
    
    // Update the links between leaf nodes
    if (node->isLeaf) {
        leftSibling->next = node->next;
        if (node->next) {
            node->next->prev = leftSibling;
        }
    }
    
    // Remove the key from the parent that separates the two nodes
    Node* parent = findParent(root, node);
    for (int i = 0; i < parent->keys.size(); i++) {
        if (parent->children[i + 1] == node) {
            parent->keys.erase(parent->keys.begin() + i);
            parent->children.erase(parent->children.begin() + i + 1);
            break;
        }
    }
    
    // Delete the current node
    delete node;
}

void BPlusTree::mergeWithRightSibling(Node* node, Node* rightSibling) {
    // Move all keys and values from the right sibling to the current node
    for (int i = 0; i < rightSibling->keys.size(); i++) {
        node->keys.push_back(rightSibling->keys[i]);
        if (node->isLeaf) {
            node->values.push_back(rightSibling->values[i]);
        }
    }
    
    if (!node->isLeaf) {
        // Move all children from the right sibling to the current node
        for (int i = 0; i < rightSibling->children.size(); i++) {
            node->children.push_back(rightSibling->children[i]);
        }
    }
    
    // Update the links between leaf nodes
    if (node->isLeaf) {
        node->next = rightSibling->next;
        if (rightSibling->next) {
            rightSibling->next->prev = node;
        }
    }
    
    // Remove the key from the parent that separates the two nodes
    Node* parent = findParent(root, node);
    for (int i = 0; i < parent->keys.size(); i++) {
        if (parent->children[i] == node) {
            parent->keys.erase(parent->keys.begin() + i);
            parent->children.erase(parent->children.begin() + i + 1);
            break;
        }
    }
    
    // Delete the right sibling
    delete rightSibling;
}

void BPlusTree::handleUnderflow(Node* node) {
    if (node == root) {
        return; // Root can have fewer keys
    }
    
    Node* parent = findParent(root, node);
    int nodeIndex = -1;
    
    // Find the index of the current node in the parent's children
    for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == node) {
            nodeIndex = i;
            break;
        }
    }
    
    // Try to borrow from the left sibling
    if (nodeIndex > 0) {
        Node* leftSibling = parent->children[nodeIndex - 1];
        if (leftSibling->keys.size() > order - 1) {
            borrowFromLeftSibling(node, leftSibling);
            return;
        }
    }
    
    // Try to borrow from the right sibling
    if (nodeIndex < parent->children.size() - 1) {
        Node* rightSibling = parent->children[nodeIndex + 1];
        if (rightSibling->keys.size() > order - 1) {
            borrowFromRightSibling(node, rightSibling);
            return;
        }
    }
    
    // If borrowing is not possible, merge with a sibling
    if (nodeIndex > 0) {
        // Merge with the left sibling
        Node* leftSibling = parent->children[nodeIndex - 1];
        mergeWithLeftSibling(node, leftSibling);
    } else {
        // Merge with the right sibling
        Node* rightSibling = parent->children[nodeIndex + 1];
        mergeWithRightSibling(node, rightSibling);
    }
    
    // Check if the parent needs to be handled for underflow
    if (parent != root && parent->keys.size() < order - 1) {
        handleUnderflow(parent);
    }
}

void BPlusTree::updateParentKey(Node* node, int oldKey, int newKey) {
    if (!node || node->isLeaf) return;
    
    for (int i = 0; i < node->keys.size(); i++) {
        if (node->keys[i] == oldKey) {
            node->keys[i] = newKey;
            break;
        }
    }
    
    // Recursively update the keys in the children
    for (int i = 0; i < node->children.size(); i++) {
        updateParentKey(node->children[i], oldKey, newKey);
    }
}

Node* BPlusTree::findParent(Node* root, Node* child) {
    if (!root || root == child) {
        return nullptr;
    }

    // If any of the children is the child node we're looking for
    for (Node* node : root->children) {
        if (node == child) {
            return root;
        }
    }

    // Recursively search in non-leaf nodes
    if (!root->isLeaf) {
        for (Node* node : root->children) {
            Node* parent = findParent(node, child);
            if (parent) {
                return parent;
            }
        }
    }

    return nullptr;
}

