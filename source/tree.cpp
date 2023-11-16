#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/tree.h"

Tree TreeCtor(const data_t init_val)
{
    Node *root = NodeCtor(init_val);
    ASSERT(root, return {});

    Tree tree = {root, 1};

    return tree;
}


static void TreeDtorSup(Tree *tree, Node *sub_tree)
{
    if(!sub_tree) return;

    TreeDtorSup(tree, sub_tree->left );
    TreeDtorSup(tree, sub_tree->right);

    free(sub_tree);

    tree->size--;
}

int TreeDtor(Tree *tree, Node *root)
{
    TREE_VER(tree, EXIT_FAILURE);

    ASSERT(root, return EXIT_FAILURE);
    ASSERT(root == tree->root || TreeSearchParent(tree, root), return EXIT_FAILURE);

    TreeDtorSup(tree, root->left );
    TreeDtorSup(tree, root->right);

    if(tree->root != root)
    {
        root->left  = NULL;
        root->right = NULL;

        Node *parent = TreeSearchParent(tree, root);

        if(parent->left == root) parent->left  = NULL;
        else                     parent->right = NULL;
    }
    else
    {
        tree->root = NULL;
    }

    free(root);
    tree->size--;

    return EXIT_SUCCESS;
}

Node *AddNode(Tree *tree, Node *tree_node, const data_t val, PlacePref pref)
{
    ASSERT(val, return NULL);

    TREE_VER(tree, NULL);

    ASSERT(tree_node, return NULL);
    ASSERT(tree_node == tree->root || TreeSearchParent(tree, tree_node), return NULL);

    Node **next = &tree_node;
    while(*next)
    {
        switch(pref)
        {
            case LEFT:
            {
                next = &((*next)->left);

                break;
            }
            case RIGHT:
            {
                next = &((*next)->right);

                break;
            }
            case AUTO:
            {
                if(val <= (*next)->data) next = &((*next)->left );
                else                     next = &((*next)->right);

                break;
            }
            default: return NULL;
        }
    }

    EXEC_ASSERT((*next) = NodeCtor(val), return NULL);
    tree->size++;

    return (*next);
}


static Node *TreeSearchValSup(Node *const tree_node, const data_t val)
{
    if(!tree_node || tree_node->data == val) return tree_node;

    Node *left  = TreeSearchValSup(tree_node->left , val);
    Node *right = TreeSearchValSup(tree_node->right, val);

    return (left ? left : right);
}

Node *TreeSearchVal(Tree *const tree, const data_t val)
{
    TREE_VER(tree, NULL);

    return TreeSearchValSup(tree->root, val);
}


static Node *TreeSearchParentSup(Node *const tree_node, Node *const search_node)
{
    if(!tree_node || tree_node->left  == search_node ||
                     tree_node->right == search_node) return tree_node;

    Node *left  = TreeSearchParentSup(tree_node->left , search_node);
    Node *right = TreeSearchParentSup(tree_node->right, search_node);

    return (left ? left : right);
}

Node *TreeSearchParent(Tree *const tree, Node *const search_node)
{
    ASSERT(search_node, return NULL);

    TREE_VER(tree, NULL);

    return TreeSearchParentSup(tree->root, search_node);
}


Node *NodeCtor(const data_t val, Node *const left, Node *const right)
{
    Node *node = (Node *)calloc(1, sizeof(Node));
    ASSERT(node, return NULL);

    node->data  = val;
    node->left  = left;
    node->right = right;

    return node;
}


static void NodeEdgeGen(Node *const node, Node *const node_next, FILE *file)
{
    if(node_next)
    {
        fprintf(file, "node%p[label = \"{<data> data: " DTS " | {<left> l: %p| <right> r: %p}}\"];\n",
                                        node_next, node_next->data, node_next->left, node_next->right);

        fprintf(file, "node%p:<left>:s -> node%p:<data>:n;\n", node, node_next);

        NodeEdgeGen(node_next, node_next->left , file);
        NodeEdgeGen(node_next, node_next->right, file);
    }
}

void TreeDot(Tree *const tree, const char *path)
{
    ASSERT(tree, return);
    ASSERT(path, return);

    FILE *file = fopen(path, "wb");
    ASSERT(file, return);
    setbuf(file, NULL);

    fprintf(file, "digraph\n"
                  "{\n"
                  "bgcolor = \"grey\";\n"
                  "ranksep = \"equally\";\n"
                  "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n"
                  "{rank = source;");
    fprintf(file, "nodel[label = \"<root> root: %p | <size> size: %zu\"; fillcolor = \"lightblue\"];", tree->root, tree->size);

    ASSERT(tree->root, return);

    fprintf(file, "node%p[label = \"{<data> data: " DTS " | {<left> l: %p| <right> r: %p}}\"; fillcolor = \"orchid\"]};\n",
                                                        tree->root, tree->root->data, tree->root->left, tree->root->right);

    NodeEdgeGen(tree->root, tree->root->left , file);
    NodeEdgeGen(tree->root, tree->root->right, file);

    fprintf(file, "}\n");

    fclose(file);

    char sys_cmd[1000] = {};
    sprintf(sys_cmd, "dot %s -T png -o tree.png", path);
    system(sys_cmd);
}


static void TreeDumpSup(Node *const tree_node)
{
    if(!tree_node) {LOG("*"); return;}

    LOG("\n(");
    LOG(DTS " ", tree_node->data);
    TreeDumpSup(tree_node->left);
    TreeDumpSup(tree_node->right);
    LOG(")");
}

void TreeDump(Tree *const tree)
{
    ASSERT(tree, return);

    LOG("\n\n");
    TreeDumpSup(tree->root);
    LOG("\n\n");
}


static Node *ReadNode(FILE *file, size_t *counter)
{
    char ch = 0;
    fscanf(file, " %c", &ch);

    switch(ch)
    {
        case '(':
        {
            (*counter)++;

            data_t data = 0;
            ASSERT(fscanf(file, DTS, &data) == 1, (*counter) = ULLONG_MAX; return NULL);

            Node *left  = ReadNode(file, counter);
            Node *right = ReadNode(file, counter);

            fscanf(file, " %c", &ch);
            ASSERT(ch == ')', (*counter) = ULLONG_MAX);

            return NodeCtor(data, left, right);
        }
        case '*':
        {
            return NULL;
        }
        default:
        {
            (*counter) = ULLONG_MAX;

            return NULL;
        }
    }
}

Tree ReadTree(const char *path)
{
    FILE *file = fopen(path, "rb");
    ASSERT(file, return {});

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadNode(file, &counter);
    tree.size = counter;

    fclose(file);

    return tree;
}

#ifdef PROTECT
static void TreeSizeVer(Tree *const tree, Node *const tree_node, size_t *counter)
{
    if(!tree_node || ++(*counter) > tree->size) return;

    TreeSizeVer(tree, tree_node->left , counter);
    TreeSizeVer(tree, tree_node->right, counter);
}

int TreeVer(Tree *const tree)
{
    ASSERT(tree && tree->root, return EXIT_FAILURE);

    size_t counter = 0;
    TreeSizeVer(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}
#endif