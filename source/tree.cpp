#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/tree.h"

Tree TreeCtor(const data_t init_val) {
    Node *root = NodeCtor(init_val);

    ASSERT(root, return {});

    Tree tree = {root, 1};

    return tree;
}


static void SubTreeDtor(Tree *tree, Node *sub_tree) {
    if(!sub_tree) return;

    SubTreeDtor(tree, sub_tree->left );
    SubTreeDtor(tree, sub_tree->right);

    free(sub_tree);

    tree->size--;
}

int TreeDtor(Tree *tree, Node *root) {
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    ASSERT(root, return EXIT_FAILURE);
    ASSERT(root == tree->root || TreeSearchParent(tree, root), return EXIT_FAILURE);

    SubTreeDtor(tree, root->left );
    SubTreeDtor(tree, root->right);

    if(root == tree->root) {
        tree->root = NULL;
    } else {
        root->left  = NULL;
        root->right = NULL;

        Node *parent = TreeSearchParent(tree, root);

        if(parent->left == root) parent->left  = NULL;
        else                     parent->right = NULL;
    }

    free(root);
    tree->size--;

    return EXIT_SUCCESS;
}

Node *AddNode(Tree *tree, Node *tree_node, const data_t val, PlacePref pref) {
    TREE_VERIFICATION(tree, NULL);

    ASSERT(tree_node, return NULL);
    ASSERT(tree_node == tree->root || TreeSearchParent(tree, tree_node), return NULL);

    Node **next = &tree_node;
    while(*next) {
        switch(pref) {
            case LEFT:
                next = &((*next)->left);
                break;
            case RIGHT:
                next = &((*next)->right);
                break;
            case AUTO:
                if(val <= (*next)->data) next = &((*next)->left );
                else                     next = &((*next)->right);
                break;
            default:
                return NULL;
        }
    }

    (*next) = NodeCtor(val);

    ASSERT((*next), return NULL);

    tree->size++;

    return (*next);
}


static Node *SubTreeSearchVal(Node *const tree_node, const data_t val) {
    if(!tree_node) return NULL;
    else if(tree_node->data == val) return tree_node;

    Node *find  = SubTreeSearchVal(tree_node->left , val);

    return (find ? find : SubTreeSearchVal(tree_node->right, val));
}

Node *TreeSearchVal(Tree *const tree, const data_t val) {
    TREE_VERIFICATION(tree, NULL);

    return SubTreeSearchVal(tree->root, val);
}


static Node *SubTreeSearchParent(Node *const tree_node, Node *const search_node) {
    if(!tree_node) return NULL;
    else if(tree_node->left  == search_node ||
            tree_node->right == search_node) return tree_node;

    Node *find  = SubTreeSearchParent(tree_node->left , search_node);

    return (find ? find : SubTreeSearchParent(tree_node->right, search_node));
}

Node *TreeSearchParent(Tree *const tree, Node *const search_node) {
    TREE_VERIFICATION(tree, NULL);

    ASSERT(search_node, return NULL);

    return SubTreeSearchParent(tree->root, search_node);
}


Node *NodeCtor(const data_t val, Node *const left, Node *const right) {
    Node *node = (Node *)calloc(1, sizeof(Node));

    ASSERT(node, return NULL);

    node->data  = val;
    node->left  = left;
    node->right = right;

    return node;
}


static void MakeDumpDir(void) {
    system("rm -rf dump_tree");
    system("mkdir dump_tree");
}


static void SubTreeTextDump(Node *const tree_node) {
    if(!tree_node) {
        LOG("*");
        return;
    }

    LOG("\n\t(");

    LOG(DATA_FORMAT " ", tree_node->data);
    SubTreeTextDump(tree_node->left );
    SubTreeTextDump(tree_node->right);

    LOG(")");
}

void TreeTextDump(Tree *const tree) {
    ASSERT(tree, return);

    LOG("TREE[%p]:  \n"
        "\troot: %p \n"
        "\tsize: %zu\n", tree, tree->root, tree->size);

    ASSERT(tree->root, return);

    SubTreeTextDump(tree->root);
    LOG("\n\n");
}


static void DotGraphCtor(Node *const node, Node *const tree_node, const char *direction, FILE *dot_file) {
    if(!tree_node) return;

    fprintf(dot_file, "node%p[label = \"{<data> data: " DATA_FORMAT " | {<left> l: %p| <right> r: %p}}\"];\n",
                                               tree_node, tree_node->data, tree_node->left, tree_node->right);

    fprintf(dot_file, "node%p:<%s>:s -> node%p:<data>:n;\n", node, direction, tree_node);

    DotGraphCtor(tree_node, tree_node->left , "left" , dot_file);
    DotGraphCtor(tree_node, tree_node->right, "right", dot_file);
}

void TreeDot(Tree *const tree, const char *png_file_name) {
    ASSERT(tree && tree->root, return);
    ASSERT(png_file_name    , return);

    FILE *dot_file = fopen("tree.dot", "wb");
    ASSERT(dot_file, return);

    fprintf(dot_file, "digraph\n"
                      "{\n"
                      "bgcolor = \"grey\";\n"
                      "ranksep = \"equally\";\n"
                      "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n"
                      "{rank = source;");
    fprintf(dot_file, "nodel[label = \"<root> root: %p | <size> size: %zu\"; fillcolor = \"lightblue\"];", tree->root, tree->size);

    fprintf(dot_file, "node%p[label = \"{<data> data: " DATA_FORMAT " | {<left> l: %p| <right> r: %p}}\"; fillcolor = \"orchid\"]};\n",
                                                                    tree->root, tree->root->data, tree->root->left, tree->root->right);

    DotGraphCtor(tree->root, tree->root->left , "left" , dot_file);
    DotGraphCtor(tree->root, tree->root->right, "right", dot_file);

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char sys_cmd[MAX_STR_LEN] = {};
    snprintf(sys_cmd, MAX_STR_LEN, "dot tree.dot -T png -o %s", png_file_name);
    system(sys_cmd);

    remove("tree.dot");
}


void TreeDump(Tree *tree, const char *caller_func, const int line) {
    static int num = 0;

    ASSERT(tree , return);

    if(num == 0) MakeDumpDir();

    TreeTextDump(tree);

    char file_name[MAX_STR_LEN] = {};

    sprintf(file_name, "dump_tree/tree_dump%d__%s:%d__.png", num, caller_func, line);
    TreeDot(tree, file_name);

    num++;
}


static Node *ReadSubTree(FILE *source, size_t *counter) {
    char ch = 0;
    fscanf(source, " %c", &ch);

    switch(ch) {
        case '(': {
            (*counter)++;

            data_t node_val = 0;

            fscanf(source, DATA_FORMAT, &node_val);

            Node *left  = ReadSubTree(source, counter);
            Node *right = ReadSubTree(source, counter);

            fscanf(source, " %c", &ch);

            ASSERT(ch == ')', LOG("Ivalid data.\n");
                              free(left );
                              free(right);
                              return NULL);

            return NodeCtor(node_val, left, right);
        }
        case '*':
            return NULL;
        default:
            LOG("Invalid data.\n");
            return NULL;
    }
}

Tree ReadTree(const char *file_name) {
    FILE *source = fopen(file_name, "rb");
    ASSERT(source, return {});

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadSubTree(source, &counter);
    tree.size = counter;

    fclose(source);

    return tree;
}

#ifdef PROTECT
static void TreeSizeValidation(Tree *const tree, Node *const tree_node, size_t *counter)
{
    if(!(tree_node) || (*counter) > tree->size) return;
    (*counter)++;

    TreeSizeValidation(tree, tree_node->left , counter);
    TreeSizeValidation(tree, tree_node->right, counter);
}

bool IsTreeValid(Tree *const tree)
{
    ASSERT(tree && tree->root    , return false);
    ASSERT(tree->size <= UINT_MAX, return false);

    size_t counter = 0;
    TreeSizeValidation(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return false);

    return true;
}
#endif