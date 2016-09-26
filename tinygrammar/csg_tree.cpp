#include "csg_tree.h"

CSGTree::Tree* CSGTree::InitTree() {
    return new Tree();
}

void CSGTree::AddNode(CSGTree::Tree* tree, CSGTree::LeafNode* node){
    tree->leaves.push_back(node);
    if (tree->root  == nullptr) { tree->root = node; }
}

void CSGTree::AddNode(CSGTree::Tree* tree, CSGTree::OpNode* node){
    tree->nodes.push_back(node);
    if (tree->root  == nullptr) { tree->root = node; }
    else if ((tree->root == node->child_left) || (tree->root == node->child_right)){ tree->root = node; }
}

CSGTree::LeafNode* CSGTree::AddShape(CSGTree::Tree* tree, AnimatedShape* shape){
    auto node = new CSGTree::LeafNode();
    auto nodeC = new CSGTree::NodeContent(shape);
    node->content = nodeC;
    CSGTree::AddNode(tree, node);
    return node;
}

CSGTree::LeafNode* CSGTree::CopyNode(CSGTree::Tree* tree, CSGTree::LeafNode* a){
    auto copy = new CSGTree::LeafNode();
    //TODO: decide whether keep a shallow copy or deep one
    copy->content = new CSGTree::NodeContent(a->content->shapes);
    a->copies.push_back(copy);
    return copy;
}

CSGTree::OpNode* CSGTree::BuildResult(const vector<polygon2r>& shapes, CSGTree::Node* a, CSGTree::Node* b){
    auto result_shapes = vector<AnimatedShape*>();
    for (auto&& r : shapes){
        auto as = new AnimatedShape();
        as->poly = r;
        result_shapes.push_back(as);
    }
    auto node = new CSGTree::OpNode();
    node->child_left = a;
    node->child_right = b;
    node->content = new NodeContent(result_shapes);
    return node;
}

CSGTree::OpNode* CSGTree::Sum(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    a_polys.insert(a_polys.end(), b_polys.begin(), b_polys.end());
    auto res_node = CSGTree::BuildResult(a_polys, a, b);
    res_node->op_type = sum_op;
    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::Union(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = union_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(result, a, b);
    res_node->op_type = union_op;
    if(update) {
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::Difference(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = subtract_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(result, a, b);
    res_node->op_type = difference_op;
    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::Intersection(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = intersect_polygons(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(result, a, b);
    res_node->op_type = intersection_op;
    if(update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::XOR(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = xor_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(result, a, b);
    res_node->op_type = xor_op;
    if(update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}


void CSGTree::UpdateLeafNode(CSGTree::Tree* tree, CSGTree::LeafNode* a, Animator anim, double delta, bool update){
    if (update){
        auto new_shapes = anim(a->content->shapes, delta);
        a->content = new NodeContent(new_shapes);
    }
    
    auto cur_node = (OpNode*)(a->parent);
    while (cur_node != nullptr){
        switch (cur_node->op_type) {
            case union_op:
            {
                printf("Updating union \n");
                auto temp = CSGTree::Union(tree, cur_node->child_left, cur_node->child_right, false);
                cur_node->content = temp->content;
                break;
            }
            case intersection_op:
            {
                printf("Updating intersection \n");
                auto temp = CSGTree::Intersection(tree, cur_node->child_left, cur_node->child_right, false);
                cur_node->content = temp->content;
                break;
            }
            case difference_op:
            {
                printf("Updating difference \n");
                auto temp = CSGTree::Difference(tree, cur_node->child_left, cur_node->child_right, false);
                cur_node->content = temp->content;
                break;
            }
            case xor_op:
            {
                printf("Updating xor \n");
                auto temp = CSGTree::XOR(tree, cur_node->child_left, cur_node->child_right, false);
                cur_node->content = temp->content;
                break;
            }
            case sum_op:
            {
                printf("Updating sum \n");
                auto temp = CSGTree::Sum(tree, cur_node->child_left, cur_node->child_right, false);
                cur_node->content = temp->content;
                break;
            }
            default:
                printf("ERROR - Invalid animator op for update leaf content. \n");
                break;
        }
        cur_node = (OpNode*)(cur_node->parent);
    }
    
    for (auto&& c : a->copies) CSGTree::UpdateLeafNode(tree, c, anim, delta, false);
}


void CSGTree::UpdateOpNode(CSGTree::Tree* tree, CSGTree::OpNode* a){
    
}
