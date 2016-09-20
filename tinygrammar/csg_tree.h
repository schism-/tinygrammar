#include "shape.h"

namespace CSGTree
{
    
    struct NodeContent
    {
        Shape& shape;
        NodeContent(Shape& shape) : shape(shape) {}
        ~NodeContent() {}
    };
    
    struct Node
    {
        NodeContent& content;
        Node(NodeContent& c) : content(c) { }
        ~Node() {}
    };
    
    struct OpNode : Node
    {
        OpNode* parent;
        Node* child_left;
        Node* child_right;
    };
    
    struct LeafNode : Node
    {
        OpNode* parent;
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
    
    void Union(Tree* tree, Node* a, Node* b);
    void Difference(Tree* tree, Node* a, Node* b);
    void Intersection(Tree* tree, Node* a, Node* b);
    
}
