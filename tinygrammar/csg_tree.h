#include "shape.h"
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
        vector<Node*> nodes;
        Tree() { root = nullptr; nodes = vector<Node*>(); }
        Tree(const vector<Node*>& nn) : nodes(nn) { root = nullptr; }
        ~Tree() {}
    };
    
    Tree* InitTree();
    CSGTree::LeafNode* AddShape(Tree* tree, AnimatedShape* shape);

    CSGTree::OpNode* Union(Tree* tree, Node* a, Node* b);
    CSGTree::OpNode* Difference(Tree* tree, Node* a, Node* b);
    CSGTree::OpNode* Intersection(Tree* tree, Node* a, Node* b);
    
    
    void AddNode(Tree* tree, LeafNode* node);
    void AddNode(Tree* tree, OpNode* node);
    OpNode* BuildResult(const vector<polygon2r>& shapes, Node* a, Node* b);
}
