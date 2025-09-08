#include "RedBlackTree.h"
#include <malloc.h>
#include <cassert>


Node::Node(const size_t data) {
    color = RED;
    _dataSize = data;
}

Node::Node(void* ptr) {
    _dataPtr = ptr;
}

Node::Node(const size_t data, void* ptr) {
    color = RED;
    _dataSize = data;
    _dataPtr = ptr;
    childs[0] = nullptr;
    childs[1] = nullptr;
}

inline int Node::howManyChilds() const {
    return (childs[LEFT] != nullptr) + (childs[RIGHT] != nullptr);
}

inline bool Node::hasChilds() const {
    return howManyChilds() != 0;
}

void* Node::getPtr() const {
    return _dataPtr;
}

size_t Node::getSize() const {
    return _dataSize;
}

void Node::setSize(const size_t size) {
    _dataSize = size;
}

bool Node::wouldJoin() {
    return leftListNode->isFree || rightListNode->isFree;
}

bool Node::leftListNodeIsFree() {
    return leftListNode->isFree;
}

bool Node::rightListNodeIsFree() {
    return rightListNode->isFree;
}

void Node::leftJoin() {
    if (!leftListNode->isFree) return;
    _dataSize += leftListNode->_dataSize;
    _dataPtr = leftListNode->_dataPtr;
    leftListNode = leftListNode->rightListNode;
    leftListNode->rightListNode = this;
}

void Node::rightJoin() {
    if (!rightListNode->isFree) return;
    _dataSize += rightListNode->_dataSize;
    rightListNode = rightListNode->rightListNode;
    rightListNode->leftListNode = this;
}

void Node::split(Node* rightNone) {
    rightNone->_dataPtr = (char*)_dataPtr + _dataSize;
    rightNone->leftListNode = this;
    rightNone->rightListNode = rightListNode;
    rightListNode = rightNone;
}

Node* Node::getSibling(const Node* node) {
    if(node == childs[LEFT])
        return childs[RIGHT];
    return childs[LEFT];;
}

NodePosition Node::getNodePosition(const Node* node) {
    if (node == childs[LEFT])
        return LEFT;
    return RIGHT;
}

Node* Node::getListNode(NodePosition position) {
    return position ? leftListNode : rightListNode;
}

bool Node::operator==(Node node) {
    return  _dataPtr == node._dataPtr && _dataSize == node._dataSize;
}

bool Node::operator>(Node node) {
    return _dataPtr > node._dataPtr && _dataSize > node._dataSize;
}

bool Node::operator<(Node node) {
    return _dataPtr < node._dataPtr && _dataSize < node._dataSize;
}

bool Node::operator<=(Node node) {
    return _dataPtr <= node._dataPtr && _dataSize <= node._dataSize;;
}

bool Node::operator>=(Node node) {
    return _dataPtr >= node._dataPtr && _dataSize >= node._dataSize;;
}

bool Node::operator==(size_t size) {
    return  _dataSize == size;
}
bool Node::operator>(size_t size) {
    return  _dataSize > size;
}
bool Node::operator<(size_t size) {
    return  _dataSize < size;
}
bool Node::operator<=(size_t size) {
    return  _dataSize <= size;
}
bool Node::operator>=(size_t size) {
    return  _dataSize >= size;
}

void* RedBlackTree::insert(size_t size) {
    Node* currentNode = root;
    NodePosition currentNodePosition = LEFT;

    //binary inserction/search
    
    while(currentNode != nullptr && !(*currentNode >= size && currentNode->isFree) ) {
        currentNodePosition = (NodePosition) (*currentNode < size);
        currentNode = currentNode->childs[currentNodePosition];
    }

    if (currentNode == nullptr) {
        return nullptr;
    }

    split(currentNode, size);

    currentNode->isFree = false;

    return currentNode->getPtr();
}

void RedBlackTree::rotation(Node* node, NodePosition rotation) {
    NodePosition oppositeRotation = (NodePosition) (rotation == LEFT);

    assert(nullptr != node->childs[oppositeRotation]);

    Node* secondNode = node->childs[oppositeRotation];

    node->childs[oppositeRotation] = secondNode->childs[rotation];
    if (node->childs[rotation] != nullptr)
        node->childs[rotation]->parent = node;
    secondNode->parent = node->parent;
    if (node->parent == nullptr)
        root = secondNode;
    else {
        NodePosition nodePosition = node->parent->getNodePosition(node);
        secondNode->parent->childs[nodePosition] = secondNode;
    }
    node->parent = secondNode;
    secondNode->childs[rotation] = node;
}


Node* RedBlackTree::search(void* ptr) {

    if (ptr == nullptr) return nullptr;


    Node* currentNode = root;
    while (currentNode->getPtr() != ptr) {
        const NodePosition goToRight = (NodePosition)(currentNode->getPtr() > ptr);
        currentNode = currentNode->getListNode(goToRight);
    }

    return currentNode;
}

bool RedBlackTree::remove(void* ptr) {
    Node* nodeToRemove = search(ptr);
    if (nodeToRemove == nullptr) return false;

    if (!nodeToRemove->wouldJoin()) {
        nodeToRemove->isFree = true;
        return true;
    }

    remove(nodeToRemove);
   
    if (nodeToRemove->leftListNodeIsFree()) {
        remove(nodeToRemove->leftListNode);
        nodeToRemove->leftJoin();
    }
    if (nodeToRemove->rightListNodeIsFree()) {
        remove(nodeToRemove->rightListNode);
        nodeToRemove->rightJoin();
    }
    nodeToRemove->color = RED;

    insertFreeNode(nodeToRemove);
    return true;
}



RedBlackTree::RedBlackTree(void* startPtr, size_t size) {

    root = new Node(size, startPtr);
}

RedBlackTree::~RedBlackTree() {
    deleteAll(root);
}

void RedBlackTree::fixInsertViolation(Node* node) {
    assert(nullptr != node);

    if (node->parent == nullptr) return;
    
    Node* currentNode = node;

    while (currentNode != root && currentNode->parent->color == RED) {
        Node* grandparent = currentNode->parent->parent;
        Node* uncle = grandparent->getSibling(currentNode->parent);
        NodePosition parentPosition = grandparent->getNodePosition(currentNode->parent);
        NodePosition oppositeParentPosition = (NodePosition)(parentPosition == LEFT);

        if (uncle == nullptr || uncle->color == BLACK) {
            if (currentNode->parent->getNodePosition(currentNode) == oppositeParentPosition) {
                rotation(currentNode->parent, parentPosition);
            }
            currentNode->parent->color = BLACK;
            grandparent->color = RED;
            rotation(grandparent, oppositeParentPosition);
            continue;
        }

        currentNode->parent->color = BLACK;
        uncle->color = BLACK;
        grandparent->color = RED;
        currentNode = grandparent;
    }
    root->color = BLACK;
}

void RedBlackTree::fixRemoveViolation(Node* node) {
    assert(nullptr != node);
    
    Node* currentNode = node;
    while (currentNode != root && currentNode != nullptr && currentNode->color == BLACK) {
        Node* curretNodeSibling = currentNode->parent->getSibling(currentNode);
        NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
        NodePosition oppositePosition = (NodePosition) (currentNodePosition == LEFT);
        if (curretNodeSibling != nullptr && curretNodeSibling->color == RED) {
            curretNodeSibling->color = BLACK;
            currentNode->parent->color = RED;
            rotation(currentNode->parent, currentNodePosition);
            curretNodeSibling = currentNode->parent->childs[oppositePosition];
        }
        //both child are black
        if (curretNodeSibling->howManyChilds() == 0 || (curretNodeSibling->childs[LEFT]->color == BLACK &&
            curretNodeSibling->childs[RIGHT]->color == BLACK)) {
            curretNodeSibling->color = RED;
            currentNode = currentNode->parent;
            continue;
        }

        if (curretNodeSibling->childs[oppositePosition] == nullptr ||
            curretNodeSibling->childs[oppositePosition]->color == BLACK) {

            if (curretNodeSibling->childs[currentNodePosition] != nullptr)
                curretNodeSibling->childs[currentNodePosition]->color = BLACK;

            curretNodeSibling->color = RED;
            rotation(curretNodeSibling, oppositePosition);
            curretNodeSibling = currentNode->parent->childs[oppositePosition];
        }

        curretNodeSibling->color = currentNode->parent->color;
        currentNode->parent->color = BLACK;
        if (curretNodeSibling->childs[oppositePosition] != nullptr)
            curretNodeSibling->childs[oppositePosition]->color = BLACK;
        rotation(currentNode->parent, currentNodePosition);
        currentNode = root;
    }
    if (currentNode != nullptr) currentNode->color = BLACK;
}

void RedBlackTree::remove(Node* node) {

    Node* replaceNode = node;
    NodePosition nodePosition = node->parent->getNodePosition(node);
    NodePosition currentNodePosition;

     //BST remove
    switch (node->howManyChilds()) {
    case 2:
        replaceNode = node->childs[RIGHT];
        while (replaceNode->childs[LEFT] != nullptr) replaceNode = replaceNode->childs[LEFT];

        currentNodePosition = (NodePosition) (replaceNode->parent == node);
        if (replaceNode->childs[RIGHT] != nullptr) {
            replaceNode->parent->childs[currentNodePosition] = replaceNode->childs[RIGHT];
            replaceNode->childs[RIGHT]->parent = replaceNode->parent;
        }

        *(replaceNode->childs) = *(node->childs);
        replaceNode->parent = node->parent;

        break;
    case 1:
        replaceNode = node->childs[node->childs[LEFT] == nullptr];
        replaceNode->parent = node->parent;

        if (node == root) break;

        nodePosition = node->parent->getNodePosition(node);
        node->parent->childs[nodePosition] = replaceNode;
    default:
        break;
    }

    if (node->color == BLACK) fixRemoveViolation(replaceNode);
}


void RedBlackTree::insertFreeNode(Node* node) {
    Node* currentNode = root;
    NodePosition currentNodePosition = LEFT;

    //binary inserction
    while (currentNode != nullptr) {
        node->parent = currentNode;
        currentNodePosition = (NodePosition)(currentNode <= node);
        currentNode = currentNode->childs[currentNodePosition];
    }

    if (node->parent == nullptr) {
        root = node;
        root->color = BLACK;
        return;
    }

    node->parent->childs[currentNodePosition] = node;

    if (node->parent->color == RED) fixInsertViolation(node);

}


void RedBlackTree::split(Node* node, size_t size) {

    size_t newSize = node->getSize() - size;
    if (newSize <= MIN_SPLIT) return;

    node->setSize(size);

    Node* newNode = new Node(newSize);
    node->split(newNode);
    insertFreeNode(newNode);
}

void RedBlackTree::deleteAll(Node* node) {
    if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
    if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
    delete node;
}

