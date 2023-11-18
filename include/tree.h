#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "log.h"

typedef int data_t;
#define DATA_FORMAT "%d"

const int MAX_STR_LEN = 1000;
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
                            TreeDump(tree_ptr, __func__, __LINE__);

#ifdef PROTECT
#define TREE_VERIFICATION(tree_ptr, ret_val_on_fail) if(!IsTreeValid(tree_ptr))\
                                                     {\
                                                         LOG("%s:%s:%d: Error: invalid tree.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                                         TREE_DUMP(tree_ptr);\
                                                         return ret_val_on_fail;\
                                                     }
#else
#define TREE_VERIFICATION(tree_ptr, ...)
#endif

Tree TreeCtor(const data_t init_val);

int TreeDtor(Tree *tree, Node *root);

Node *AddNode(Tree *tree, Node *tree_node, const data_t val, PlacePref pref = AUTO);

Node *TreeSearchVal(Tree *const tree, const data_t val);

Node *TreeSearchParent(Tree *const tree, Node *const search_node);

Node *NodeCtor(const data_t val, Node *const left = NULL, Node *const right = NULL);

void TreeTextDump(Tree *const tree);

void TreeDot(Tree *const tree, const char *png_file_name);

void TreeDump(Tree *tree, const char *func, const int line);

Tree ReadTree(const char *file_name);

#ifdef PROTECT
bool IsTreeValid(Tree *const tree);
#endif

#endif //TREE_H