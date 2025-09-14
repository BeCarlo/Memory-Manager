#include "SecondTree.h"
#include <cassert>

SecondNode::SecondNode(void* dataPtr, Node* SecondNode) {
	_dataPtr = dataPtr;
	_node = SecondNode;
}

inline int SecondNode::howManyChilds() const {
    return (childs[LEFT] != nullptr) + (childs[RIGHT] != nullptr);
}

inline bool SecondNode::hasChilds() const {
    return howManyChilds() != 0;
}


SecondNode* SecondNode::getSibling(const SecondNode* SecondNode) {
    if (childs[LEFT] != nullptr && SecondNode == childs[LEFT])
        return childs[RIGHT];
    return childs[LEFT];
}

NodePosition SecondNode::getNodePosition(const SecondNode* SecondNode) {
    if (childs[LEFT] != nullptr && SecondNode == childs[LEFT])
        return LEFT;
    return RIGHT;
}

bool SecondNode::operator==(SecondNode SecondNode) {
    return  _dataPtr == SecondNode._dataPtr;
}

bool SecondNode::operator>(SecondNode SecondNode) {
    return _dataPtr > SecondNode._dataPtr;
}

bool SecondNode::operator<(SecondNode SecondNode) {
    return _dataPtr < SecondNode._dataPtr;
}

bool SecondNode::operator<=(SecondNode SecondNode) {
    return _dataPtr <= SecondNode._dataPtr;
}

bool SecondNode::operator>=(SecondNode SecondNode) {
    return _dataPtr >= SecondNode._dataPtr;
}

bool SecondNode::operator==(void* ptr) {
    return  _dataPtr == ptr;
}
bool SecondNode::operator>(void* ptr) {
    return  _dataPtr > ptr;
}
bool SecondNode::operator<(void* ptr) {
    return  _dataPtr < ptr;
}
bool SecondNode::operator<=(void* ptr) {
    return  _dataPtr <= ptr;
}
bool SecondNode::operator>=(void* ptr) {
    return  _dataPtr >= ptr;
}


void SecondRedBlackTree::insert(void* ptr, Node* nodeToAdd) {

    SecondNode* newNode = new SecondNode(ptr, nodeToAdd);
    SecondNode* currentNode = _root;
    NodePosition currentNodePosition = LEFT;

    //binary inserction

    while (currentNode != nullptr) {
        newNode->parent = currentNode;
        currentNodePosition = (NodePosition)(*currentNode >= *newNode);
        currentNode = currentNode->childs[currentNodePosition];
    }

    if (newNode->parent == nullptr) {
        _root = newNode;
        _root->color = BLACK;
        return;
    }

    newNode->parent->childs[currentNodePosition] = newNode;

    if (newNode->parent->color == RED) fixInsertViolation(newNode);

}

void SecondRedBlackTree::rotation(SecondNode* node, NodePosition rotation) {
    NodePosition oppositeRotation = (NodePosition)(rotation == LEFT);
    assert(node != nullptr);
    assert(node->childs[oppositeRotation] != nullptr);

    SecondNode* secondNode = node->childs[oppositeRotation];

    // Move secondNode's child to node
    node->childs[oppositeRotation] = secondNode->childs[rotation];
    if (secondNode->childs[rotation] != nullptr)
        secondNode->childs[rotation]->parent = node;

    // Update parent pointers
    secondNode->parent = node->parent;
    if (node->parent == nullptr) {
        _root = secondNode;
    }
    else {
        NodePosition nodePosition = node->parent->getNodePosition(node);
        node->parent->childs[nodePosition] = secondNode;
    }

    node->parent = secondNode;
    secondNode->childs[rotation] = node;
    secondNode->childs[rotation] = node;
}

void SecondRedBlackTree::fixInsertViolation(SecondNode* node) {
    assert(nullptr != node);

    if (node->parent == nullptr) return;

    SecondNode* currentNode = node;

    while (currentNode != _root && currentNode->parent->color == RED) {
        SecondNode* grandparent = currentNode->parent->parent;
        SecondNode* uncle = grandparent->getSibling(currentNode->parent);
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
    _root->color = BLACK;
}


void SecondRedBlackTree::fixRemoveViolation(SecondNode* node) {
    assert(nullptr != node);

    SecondNode* currentNode = node;
    while (currentNode != nullptr && currentNode->parent != nullptr && currentNode->color == BLACK) {
        SecondNode* curretNodeSibling = currentNode->parent->getSibling(currentNode);
        NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
        NodePosition oppositePosition = (NodePosition)(currentNodePosition == LEFT);

        // Fix: handle nullptr sibling as black
        if (curretNodeSibling == nullptr) {
            currentNode = currentNode->parent;
            continue;
        }

        if (curretNodeSibling->color == RED) {
            curretNodeSibling->color = BLACK;
            currentNode->parent->color = RED;
            rotation(currentNode->parent, currentNodePosition);
            curretNodeSibling = currentNode->parent->childs[oppositePosition];
            if (curretNodeSibling == nullptr) {
                currentNode = currentNode->parent;
                continue;
            }
        }

        bool isLeftChildBlack = curretNodeSibling->childs[LEFT] == nullptr || curretNodeSibling->childs[LEFT]->color == BLACK;
        bool isRightChildBlack = curretNodeSibling->childs[RIGHT] == nullptr || curretNodeSibling->childs[RIGHT]->color == BLACK;
        if (curretNodeSibling->howManyChilds() == 0 || (isLeftChildBlack && isRightChildBlack)) {
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
            if (curretNodeSibling == nullptr) {
                currentNode = currentNode->parent;
                continue;
            }
        }

        curretNodeSibling->color = currentNode->parent->color;
        currentNode->parent->color = BLACK;
        if (curretNodeSibling->childs[oppositePosition] != nullptr)
            curretNodeSibling->childs[oppositePosition]->color = BLACK;
        rotation(currentNode->parent, currentNodePosition);
        currentNode = _root;
    }
    if (currentNode != nullptr) currentNode->color = BLACK;
}


Node* SecondRedBlackTree::remove(void* ptr) {
    SecondNode* node = search(ptr);
    SecondNode* replaceNode = node;
    NodePosition nodePosition;
    NodePosition currentNodePosition;

    //BST remove
    switch (node->howManyChilds()) {
    case 2:
        replaceNode = node->childs[RIGHT];
        while (replaceNode->childs[LEFT] != nullptr) replaceNode = replaceNode->childs[LEFT];

        currentNodePosition = (NodePosition)(replaceNode->parent == node);
        if (replaceNode->childs[RIGHT] != nullptr) {
            replaceNode->parent->childs[currentNodePosition] = replaceNode->childs[RIGHT];
            replaceNode->childs[RIGHT]->parent = replaceNode->parent;
        }

        node->childs[LEFT]->parent = replaceNode;
        node->childs[RIGHT]->parent = replaceNode;
        replaceNode->childs[LEFT] = node->childs[LEFT] != replaceNode ? node->childs[LEFT] : nullptr;
        replaceNode->childs[RIGHT] = node->childs[RIGHT] != replaceNode ? node->childs[RIGHT] : nullptr;

        replaceNode->parent = node->parent;
        if (node->parent == nullptr) {
            _root = replaceNode;
            break;
        }

        node->parent->childs[node->parent->getNodePosition(node)] = replaceNode;
        break;
    case 1:

        replaceNode = node->childs[node->childs[LEFT] == nullptr];
        replaceNode->parent = node->parent;

        if (node->parent == nullptr) {
            _root = replaceNode;
            break;
        }

        nodePosition = node->parent->getNodePosition(node);
        node->parent->childs[nodePosition] = replaceNode;
        break;
    default:
        if (node->parent == nullptr) {
            _root = nullptr;
            break;
        }
        nodePosition = node->parent->getNodePosition(node);
        node->parent->childs[nodePosition] = nullptr;
        break;
    }

    if (node->color == BLACK) fixRemoveViolation(replaceNode);
    Node* nodeToReturn = node->getNode();
    delete node;
    return nodeToReturn;
}

SecondNode* SecondRedBlackTree::search(void* ptr) {

    if (ptr == nullptr) return nullptr;

    SecondNode* currentNode = _root;

    while (currentNode != nullptr) {

        if (*currentNode == ptr) return currentNode;
        if (!currentNode->hasChilds()) return nullptr;

        NodePosition currentNodePosition = (NodePosition)(*currentNode >= ptr);
        currentNode = currentNode->childs[currentNodePosition];
    }

    return currentNode;
}

SecondRedBlackTree::~SecondRedBlackTree() {
    deleteAll(_root);
}

void SecondRedBlackTree::deleteAll(SecondNode* node) {
    if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
    if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
    delete node;
}
