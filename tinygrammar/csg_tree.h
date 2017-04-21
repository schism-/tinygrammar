#include "shape.h"
#include "animator.h"
#include "clipper_methods.h"

namespace CSGTree
{
    
    struct Node
    {
        Node* parent = nullptr;
        vector<AnimatedShape*> shapes;
        int node_tag = -1;
        
        Node() { }
        Node(const vector<AnimatedShape*>& s) { shapes = s; }
        
        virtual ~Node() {
            for(auto s : shapes) if(s) delete s;
        }
    };
    
    struct OpNode : Node
    {
        Node* child_left = nullptr;
        Node* child_right = nullptr;
        int op_type;
    };
    
    struct LeafNode : Node
    {
        vector<LeafNode*> copies;
        Animator animation;
    };
    
    struct Tree
    {
        int node_id = 0;
        Node* root = nullptr;
        vector<OpNode*> nodes;
        vector<LeafNode*> leaves;
        Tree() { root = nullptr; nodes = vector<OpNode*>(); leaves = vector<LeafNode*>(); }
        Tree(const vector<OpNode*>& nn, const vector<LeafNode*>& ll) : nodes(nn), leaves(ll) { root = nullptr; }
        ~Tree() {}
    };
    
    Tree* InitTree();
    LeafNode* AddShape(Tree* tree, AnimatedShape* shape);
    LeafNode* CopyNode(Tree* tree, LeafNode* a);

    OpNode* Sum(Tree* tree, Node* a, Node* b, bool update = true);
    OpNode* Union(Tree* tree, Node* a, Node* b, bool update = true);
    OpNode* Difference(Tree* tree, Node* a, Node* b, bool update = true);
    OpNode* Intersection(Tree* tree, Node* a, Node* b, bool update = true);
    OpNode* XOR(Tree* tree, Node* a, Node* b, bool update = true);
    OpNode* PlaceInShape(Tree* tree, Node* a, Node* b, bool update = true);
    
//    void UpdateLeafNode(Tree* tree, LeafNode* a, Animator anim, double delta, bool update = true);
    void UpdateLeafNode(Tree* tree, LeafNode* a, const Animator& anim, double current_time, double incr, double total_dur, bool update = true);
    void UpdateLeafNode(Tree* tree, const vector<LeafNode*>& as, const Animator& anim, int frame, bool update = true);
    void UpdateContent(Tree* tree, LeafNode* a);
    void UpdateTree(Tree* tree);
    void PropagateContent(Tree* tree, LeafNode* a);
    
    void UpdateOpNode(Tree* tree, OpNode* a);
    
    void AddNode(Tree* tree, LeafNode* node);
    void AddNode(Tree* tree, OpNode* node);
    
    LeafNode* FindNode(Tree* tree, AnimatedShape* shape);
    
    OpNode* BuildResult(Tree* tree, const vector<polygon2r>& shapes, Node* a, Node* b);
    OpNode* BuildResult(Tree* tree, Node* a, Node* b);
    
    int get_node_id(Tree* tree);
}
