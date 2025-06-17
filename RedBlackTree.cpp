#include "RedBlackTree.h"
#include <malloc.h>
#include <cassert>

Node::Node(const size_t size) {
    color = RED;
    dataSize = size;
    data = malloc(dataSize);
}

Node::~Node() {
    delete data;
}

int Node::howManyChilds() const {
    return (childs[LEFT] != nullptr) + (childs[RIGHT] != nullptr);
}

bool Node::hasChilds() const {
    return howManyChilds() != 0;
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

void* RedBlackTree::insert(size_t size) {

    Node* newNode = new Node(size);
    Node* currentNode = root;
    NodePosition currentNodePosition = LEFT;

    //binary inserction
    
    while(currentNode != nullptr ) {
        newNode->parent = currentNode;
        currentNodePosition = (NodePosition) (currentNode->data <= newNode->data);
        currentNode = currentNode->childs[currentNodePosition];
    }

    if (newNode->parent == nullptr) {
        root = newNode;
        root->color = BLACK;
        return newNode;
    }

    newNode->parent->childs[currentNodePosition] = newNode;

    if(newNode->parent->color == RED) fixInsertViolation(newNode);

    return newNode->data;
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

    while (currentNode != nullptr) {

        if (currentNode->data == ptr) return currentNode;
        if (!currentNode->hasChilds()) return nullptr;

        NodePosition currentNodePosition = (NodePosition)(currentNode->data <= ptr);
        currentNode = currentNode->childs[currentNodePosition];
    }

    return currentNode;
}

bool RedBlackTree::remove(void* ptr) {
    Node* nodeToRemove = search(ptr);
    if (nodeToRemove == nullptr) return false;
    Node* replaceNode = nodeToRemove;
    NodePosition nodeToRemovePosition = nodeToRemove->parent->getNodePosition(nodeToRemove);
    NodePosition currentNodePosition;

    //BST remove
    switch (nodeToRemove->howManyChilds()) {
    case 2:
        replaceNode = nodeToRemove->childs[RIGHT];
        while (replaceNode->childs[LEFT] != nullptr) replaceNode = replaceNode->childs[LEFT];

        currentNodePosition = (NodePosition) (replaceNode->parent == nodeToRemove);
        if (replaceNode->childs[RIGHT] != nullptr) {
            replaceNode->parent->childs[currentNodePosition] = replaceNode->childs[RIGHT];
            replaceNode->childs[RIGHT]->parent = replaceNode->parent;
        }

        *(replaceNode->childs) = *(nodeToRemove->childs);
        replaceNode->parent = nodeToRemove->parent;

        break;
    case 1:
        replaceNode = nodeToRemove->childs[nodeToRemove->childs[LEFT] == nullptr];
        replaceNode->parent = nodeToRemove->parent;

        if (nodeToRemove == root) break;

        nodeToRemovePosition = nodeToRemove->parent->getNodePosition(nodeToRemove);
        nodeToRemove->parent->childs[nodeToRemovePosition] = replaceNode;
    default:
        break;
    }

    if (nodeToRemove->color == BLACK) fixRemoveViolation(replaceNode);

    delete nodeToRemove;
    return true;
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

void RedBlackTree::deleteAll(Node* node) {
    if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
    if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
    delete node;
}
