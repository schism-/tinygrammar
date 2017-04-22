    #include "csg_tree.h"

CSGTree::Tree* CSGTree::InitTree() {
    return new Tree();
}

CSGTree::LeafNode* CSGTree::FindNode(CSGTree::Tree* tree, AnimatedShape* shape){
    for (auto&& l : tree->leaves){
        if (find(l->shapes.begin(), l->shapes.end(), shape) != l->shapes.end()){
            return l;
        }
    }
    return nullptr;
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

int CSGTree::get_node_id(CSGTree::Tree* tree){
    return tree->node_id++;
}

CSGTree::LeafNode* CSGTree::AddShape(CSGTree::Tree* tree, AnimatedShape* shape){
    auto node = new CSGTree::LeafNode();
    node->node_tag = get_node_id(tree);
    node->shapes = {shape};
    CSGTree::AddNode(tree, node);
    return node;
}

CSGTree::LeafNode* CSGTree::CopyNode(CSGTree::Tree* tree, CSGTree::LeafNode* a){
    auto copy = new CSGTree::LeafNode();
    //TODO: decide whether keep a shallow copy or deep one
    copy->shapes = a->shapes;
    a->copies.push_back(copy);
    return copy;
}

CSGTree::OpNode* CSGTree::BuildResult(CSGTree::Tree* tree, const vector<polygon2r>& shapes, CSGTree::Node* a, CSGTree::Node* b){
    auto result_shapes = vector<AnimatedShape*>();
    for (auto&& r : shapes){
        auto as = new AnimatedShape();
        as->poly = r;
        for (auto&& acs : a->shapes){
            if (r == acs->poly) {
                as->border_color = acs->border_color;
                as->fill_color = acs->fill_color;
                break;
            }
        }
        for (auto&& bcs : b->shapes){
            if (r == bcs->poly) {
                as->border_color = bcs->border_color;
                as->fill_color = bcs->fill_color;
                break;
            }
        }
        result_shapes.push_back(as);
    }
    
    auto node = new CSGTree::OpNode();
    node->node_tag = get_node_id(tree);
    node->child_left = a;
    node->child_right = b;
    node->shapes = result_shapes;
    return node;
}

CSGTree::OpNode* CSGTree::BuildResult(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    auto node = new CSGTree::OpNode();
    node->node_tag = get_node_id(tree);
    node->child_left = a;
    node->child_right = b;

    node->shapes.reserve(a->shapes.size()+b->shapes.size());
    for(auto s : a->shapes) {
        auto as = new AnimatedShape();
        as->poly = s->poly;
        as->border_color = s->border_color;
        as->fill_color = s->fill_color;
        node->shapes.push_back(as);
    }
    for(auto s : b->shapes) {
        auto as = new AnimatedShape();
        as->poly = s->poly;
        as->border_color = s->border_color;
        as->fill_color = s->fill_color;
        node->shapes.push_back(as);
    }
    return node;
}

#if SPEEDUP_SUM

CSGTree::OpNode* CSGTree::Sum(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    auto res_node = CSGTree::BuildResult(tree, a, b);
    res_node->op_type = sum_op;
    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

#else

CSGTree::OpNode* CSGTree::Sum(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->content->shapes, [&](AnimatedShape* shape){return shape->poly;});
    a_polys.insert(a_polys.end(), b_polys.begin(), b_polys.end());
    auto res_node = CSGTree::BuildResult(tree, a_polys, a, b);
    res_node->op_type = sum_op;
    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

#endif

CSGTree::OpNode* CSGTree::Union(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = union_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = union_op;
    if(update) {
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::Difference(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = subtract_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = difference_op;
    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::Intersection(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = intersect_polygons(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = intersection_op;
    if(update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::XOR(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = xor_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = xor_op;
    if(update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

CSGTree::OpNode* CSGTree::PlaceInShape(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b, bool update){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto temp = intersect_polygons(b_polys, a_polys);
    auto result = subtract_polygons_clipper(a_polys, temp);
    result.insert(result.end(), temp.begin(), temp.end());
    
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = place_in_op;
    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
    }
    return res_node;
}

//void CSGTree::UpdateLeafNode(CSGTree::Tree* tree, CSGTree::LeafNode* a, Animator anim, double delta, bool update){
//    if (update){
//        auto new_shapes = anim(a->content->shapes, delta);
//        a->content = new NodeContent(new_shapes);
//    }
//    
//    UpdateContent(tree, a);
//    
//    for (auto&& c : a->copies) CSGTree::UpdateLeafNode(tree, c, anim, delta, false);
//}

void CSGTree::UpdateLeafNode(CSGTree::Tree* tree, CSGTree::LeafNode* a, const Animator& anim, double current_time, double incr, double total_dur, bool update){
    if (update){
//        auto new_shapes = anim(a->content->shapes, current_time, incr, total_dur);
//        a->content = new NodeContent(new_shapes);
        anim(a->shapes, current_time, incr, total_dur);
    }
}

void CSGTree::UpdateLeafNode(CSGTree::Tree* tree, const vector<CSGTree::LeafNode*>& as, const Animator& anim, int frame, bool update){
    for (auto&& a : as){
        UpdateLeafNode(tree, a, anim, frame, frame, update);
    }
}

void CSGTree::UpdateContent(CSGTree::Tree* tree, CSGTree::LeafNode* a){
    auto cur_node = (OpNode*)(a->parent);
    while (cur_node != nullptr){
        UpdateOpNode(tree, cur_node);
        cur_node = (OpNode*)(cur_node->parent);
    }
}

void CSGTree::UpdateTree(CSGTree::Tree* tree){
    for (auto&& nl : tree->nodes) UpdateOpNode(tree, nl);
}

void CSGTree::PropagateContent(CSGTree::Tree* tree, CSGTree::LeafNode* a){
    auto root = tree->root;
    for (auto&& as : a->shapes){
        for (auto r_as : root->shapes){
            if (r_as->poly == as->poly){
                r_as->border_color = as->border_color;
                r_as->fill_color = as->fill_color;
            }
        }
    }
}


void CSGTree::UpdateOpNode(CSGTree::Tree* tree, CSGTree::OpNode* a){
    switch (a->op_type) {
        case union_op:
        {
            //printf("Updating union \n");
            auto temp = CSGTree::Union(tree, a->child_left, a->child_right, false);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case intersection_op:
        {
            //printf("Updating intersection \n");
            auto temp = CSGTree::Intersection(tree, a->child_left, a->child_right, false);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case difference_op:
        {
            //printf("Updating difference \n");
            auto temp = CSGTree::Difference(tree, a->child_left, a->child_right, false);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case xor_op:
        {
            //printf("Updating xor \n");
            auto temp = CSGTree::XOR(tree, a->child_left, a->child_right, false);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case sum_op:
        {
            //printf("Updating sum \n");
            auto temp = CSGTree::Sum(tree, a->child_left, a->child_right, false);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case place_in_op:
        {
            //printf("Updating place_in \n");
            auto temp = CSGTree::PlaceInShape(tree, a->child_left, a->child_right, false);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        default:
            printf("ERROR - Invalid animator op for update leaf content. \n");
            break;
    }
}
