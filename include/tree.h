#ifndef TREE_H
#define TREE_H

#include "log.h"

typedef int data_t;
#define DTS "%d"

struct Node
{
    data_t data;

    Node *left;
    Node *right;
};

struct Tree
{
    Node *root;

    size_t size;
};

enum PlacePref
{
    LEFT  = -1,
    AUTO  =  0,
    RIGHT =  1
};

#define TREE_DUMP(tree_ptr) LOG("%s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            TreeDump(tree_ptr);\
                            TreeDot(tree_ptr, "tree.dot")

#ifdef PROTECT
#define TREE_VER(tree_ptr, ret_val_on_fail) if(TreeVer(tree_ptr))\
                                            {\
                                                LOG("%s:%s:%d: Error: invalid tree.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                                TREE_DUMP(tree_ptr);\
                                                return ret_val_on_fail;\
                                            }
#else
#define TREE_VER(tree_ptr, ...)
#endif

Tree TreeCtor(const data_t init_val);

int TreeDtor(Tree *tree, Node *root);

Node *AddNode(Tree *tree, Node *tree_node, const data_t val, PlacePref pref = AUTO);

Node *TreeSearchVal(Tree *const tree, const data_t val);

Node *TreeSearchParent(Tree *const tree, Node *const search_node);

Node *NodeCtor(const data_t val, Node *const left = NULL, Node *const right = NULL);

void TreeDot(Tree *const tree, const char *path);

void TreeDump(Tree *const tree);

Tree ReadTree(const char *path);

#ifdef PROTECT
int TreeVer(Tree *const tree);
#endif

#endif //TREE_H