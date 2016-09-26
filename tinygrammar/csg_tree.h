#include "shape.h"
#include "animator.h"
#include "clipper_methods.h"

namespace CSGTree
{
    
    struct NodeContent
    {
        vector<AnimatedShape*> shapes;
        NodeContent(vector<AnimatedShape*> shapes) : shapes(shapes) {}
        NodeContent(AnimatedShape* shape) { shapes = vector<AnimatedShape*>(); shapes.push_back(shape); }
        ~NodeContent() {}
    };
    
    struct Node
    {
        Node* parent;
        NodeContent* content;
        Node() { }
        Node(NodeContent* c) : content(c) { }
        Node(vector<AnimatedShape*> shapes) { content = new NodeContent(shapes); }
        ~Node() {}
    };
    
    struct OpNode : Node
    {
        Node* child_left;
        Node* child_right;
        int op_type;
    };
    
    struct LeafNode : Node
    {
        
    };
    
    struct Tree
    {
        Node* root;
        vector<OpNode*> nodes;
        vector<LeafNode*> leaves;
        Tree() { root = nullptr; nodes = vector<OpNode*>(); leaves = vector<LeafNode*>(); }
        Tree(const vector<OpNode*>& nn, const vector<LeafNode*>& ll) : nodes(nn), leaves(ll) { root = nullptr; }
        ~Tree() {}
    };
    
    Tree* InitTree();
    CSGTree::LeafNode* AddShape(Tree* tree, AnimatedShape* shape);

    OpNode* Sum(Tree* tree, Node* a, Node* b);
    OpNode* Union(Tree* tree, Node* a, Node* b);
    OpNode* Difference(Tree* tree, Node* a, Node* b);
    OpNode* Intersection(Tree* tree, Node* a, Node* b);
    OpNode* XOR(Tree* tree, Node* a, Node* b);
    
    void UpdateLeafNode(Tree* tree, LeafNode* a, Animator anim);
    void UpdateOpNode(Tree* tree, OpNode* a);
    
    void AddNode(Tree* tree, LeafNode* node);
    void AddNode(Tree* tree, OpNode* node);
    OpNode* BuildResult(const vector<polygon2r>& shapes, Node* a, Node* b);
}
