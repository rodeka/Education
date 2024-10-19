#include <stdlib.h>
#include <stdio.h>
#include "rbTree.h"

RBNode* createNode(int data) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    node->data = data;
    node->color = RED;
    node->left = node->right = node->parent = NULL;
    return node;
}

void rotateLeft(RBNode** root, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != NULL) y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    return;
}

void rotateRight(RBNode** root, RBNode* x) {
    RBNode *y = x->left;
    x->left = y->right;
    if (y->right != NULL) y->right->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
    return;
}

void fixViolation(RBNode** root, RBNode* z) {
    while (z != *root && z->parent->color == RED) {
        RBNode* grandparent = z->parent->parent;
        if (z->parent == grandparent->left) {
            RBNode* y = grandparent->right;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(root, z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                rotateRight(root, grandparent);
            }
        } else {
            RBNode* y = grandparent->left;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(root, z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                rotateLeft(root, grandparent);
            }
        }
    }
    (*root)->color = BLACK;
    return;
}

void insertRBTree(RBNode** root, int data) {
    RBNode* z = createNode(data);
    RBNode* y = NULL;
    RBNode* x = *root;

    while (x != NULL) {
        y = x;
        if (z->data < x->data) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;
    if (y == NULL) {
        *root = z;
    } else if (z->data < y->data) {
        y->left = z;
    } else {
        y->right = z;
    }

    fixViolation(root, z);
    return;
}

void inorderTraversal(RBNode* root) {
    if (root == NULL) {
        return;
    }
    inorderTraversal(root->left);
    printf("%d%c ", root->data, root->color ? 'b' : 'r');
    inorderTraversal(root->right);
    return;
}

void freeRBTree(RBNode* root) {
    if(root == NULL) {
        return;
    }

    freeRBTree(root->right);
    freeRBTree(root->left);

    free(root);
    return;
}
