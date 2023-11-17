#include "include/tree.h"

int main(void)
{
    Tree tree = TreeCtor(4);
    AddNode(&tree, tree.root, 2, LEFT);
    AddNode(&tree, tree.root, 3, LEFT);
    AddNode(&tree, tree.root, 4, AUTO);
    AddNode(&tree, tree.root, 6, AUTO);
    TREE_DUMP(&tree);
    TreeDtor(&tree, tree.root);
}