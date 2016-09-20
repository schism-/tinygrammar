#include "shape.h"
#include "clipper_methods.h"

namespace CSGTree
{
    
    enum {
        union_op = 0,
        difference_op,
        intersection_op,
        xor_op
    };
    
    struct NodeContent
    {
        vector<AnimatedShape*> shapes;
        NodeContent(vector<AnimatedShape*> shapes) : shapes(shapes) {}
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
        vector<Node*> nodes;
        Tree() { nodes = vector<Node*>(); }
        Tree(const vector<Node*>& nn) : nodes(nn) { }
        ~Tree() {}
    };
    
    Tree* InitTree();
    void AddNode(Tree* tree, LeafNode* node);
    Node* BuildResult(const vector<polygon2r>& shapes, Node* a, Node* b);
    
    void Union(Tree* tree, Node* a, Node* b);
    void Difference(Tree* tree, Node* a, Node* b);
    void Intersection(Tree* tree, Node* a, Node* b);
    
}
