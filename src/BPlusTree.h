#pragma once
#include <global.h>
#include <stdexcept>
using namespace std;

struct PageRowPair {
    int pageIndex;
    int rowIndex;
    PageRowPair(int page = -1, int row = -1) : pageIndex(page), rowIndex(row) {}
};

struct Node {
    bool isLeaf;
    vector<int> keys;
    vector<Node*> children;
    Node* next; // For leaf nodes to connect to next leaf
    Node* prev; // For leaf nodes to connect to previous leaf
    vector<PageRowPair> values; // For leaf nodes to store page and row pointers
    Node* parent;

    Node(bool isLeaf = false) : isLeaf(isLeaf), next(nullptr), prev(nullptr), parent(nullptr) {}
};

class BPlusTree {
private:
    Node* root;
    int order; // Maximum number of keys in a node

    // Helper functions
    void insertIntoLeaf(Node* leaf, int key, int pageIndex, int rowIndex);
    void splitLeaf(Node* leaf);
    void insertIntoInternal(Node* node, int key, Node* child);
    void splitInternal(Node* node);
    void insertNonFull(Node* node, int key, int pageIndex, int rowIndex); 
    void splitChild(Node* parent, int index, Node* child);

public:
    BPlusTree(int order = 3) : root(nullptr), order(order) {}
    ~BPlusTree();

    // Main operations
    void insert(int key, int pageIndex, int rowIndex);
    PageRowPair search(int key);
    void printTree();
    void clear();
    Node* getRoot();
    
    // File operations
    void saveToFile(string filename);
    Node* loadNode(ifstream& file);
    BPlusTree* loadFromFile(string filename);
    Node* findLeaf(int key);
    void generateDotFile(string filename);
    void printLeafNodes();

    void deleteFromLeaf(Node* leaf, int key);
    void deleteFromInternal(Node* node, int key);
    bool borrowFromPrev(Node* node, int idx);
    bool borrowFromNext(Node* node, int idx);
    void mergeNodes(Node* node, int idx);
    void handleUnderflow(Node* node);
    void handleInternalNodeUnderflow(Node* node);
    void updateInternalKeys(Node* node, int oldKey, int newKey);
    int findKeyIndex(Node* node, int key);
    Node* findParent(Node* node, Node* child);
    void updateParentKey(Node* node, int oldKey, int newKey);
    
    void borrowFromLeftSibling(Node* node, Node* leftSibling);
    void borrowFromRightSibling(Node* node, Node* rightSibling);
    void mergeWithLeftSibling(Node* node, Node* leftSibling);
    void mergeWithRightSibling(Node* node, Node* rightSibling);
    
    // New helper functions for deletion
    int findFirstPositionOfKey(Node* leaf, int key);
    int getNextKey(Node* leaf);
    int findChildIndex(Node* parent, Node* child);
    bool deleteFirstInstance(int key);
    void updateTree();
    void handleLeafUnderflow(Node* leaf);
    void updateInternalNodesRecursive(Node* node);
    void remove(int key);

private:
    // Helper function to delete tree
    void deleteTree(Node* node);
};