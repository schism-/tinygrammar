    #include "csg_tree.h"

CSGTree::Tree* CSGTree::InitTree() {
    return new Tree();
}
//caching?
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
    static auto def_border_color = AnimatedShape().border_color;
    static auto def_fill_color = AnimatedShape().fill_color;
    auto result_shapes = vector<AnimatedShape*>();
    auto acol = false, bcol = false;
    for(auto s : a->shapes)
        if(!(s->border_color == def_border_color) || !(s->fill_color == def_fill_color)) {
            acol = true; break;
        }
    for(auto s : b->shapes)
        if(!(s->border_color == def_border_color) || !(s->fill_color == def_fill_color)) {
            bcol = true; break;
        }
    for (auto&& r : shapes){
        auto as = new AnimatedShape();
        as->poly = r;
        if(acol) {
            for (auto&& acs : a->shapes){
                if (r == acs->poly) {
                    as->border_color = acs->border_color;
                    as->fill_color = acs->fill_color;
                    break;
                }
            }
        }
        if(bcol) {
            for (auto&& bcs : b->shapes){
                if (r == bcs->poly) {
                    as->border_color = bcs->border_color;
                    as->fill_color = bcs->fill_color;
                    break;
                }
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

CSGTree::OpNode* CSGTree::BuildResult_Sum(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    auto node = new CSGTree::OpNode();
    node->node_tag = get_node_id(tree);
    node->child_left = a;
    node->child_right = b;

    node->shapes.reserve(a->shapes.size()+b->shapes.size());
    for (auto&& s : a->shapes){
        auto as = new AnimatedShape();
        as->poly = s->poly;
        as->border_color = s->border_color;
        as->fill_color = s->fill_color;
        node->shapes.push_back(as);
    }
    for (auto&& s : b->shapes){
        auto as = new AnimatedShape();
        as->poly = s->poly;
        as->border_color = s->border_color;
        as->fill_color = s->fill_color;
        node->shapes.push_back(as);
    }
    
    return node;
}

CSGTree::OpNode* CSGTree::BuildResult_SumAlias(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b) {
    auto node = new CSGTree::OpNode();
    node->node_tag = get_node_id(tree);
    node->child_left = a;
    node->child_right = b;
    
    node->shapes.reserve(a->shapes.size()+b->shapes.size());
    for (auto&& s : a->shapes) {
        node->shapes.push_back(s);
    }
    for (auto&& s : b->shapes) {
        node->shapes.push_back(s);
    }
    
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

void CSGTree::Update_Sum(CSGTree::Tree* tree, CSGTree::OpNode* node){
    auto a = node->child_left, b = node->child_right;
    
#if SPEEDUP_ALIASSUM
    node->shapes.clear();
    node->shapes.reserve(a->shapes.size()+b->shapes.size());
    node->shapes.insert(node->shapes.end(), a->shapes.begin(), a->shapes.end());
    node->shapes.insert(node->shapes.end(), b->shapes.begin(), b->shapes.end());
    return;
#endif

#if SPEEDUP_SUM == 0
    for(auto s : node->shapes) delete s;
    node->shapes.clear();
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
#elif SPEEDUP_SUM == 1
    if(node->shapes.size() != a->shapes.size()+b->shapes.size()) printf("BUG!!!!!!!!!!!!!!!\n");
    for(auto i = 0; i < a->shapes.size(); i ++) {
        node->shapes[i]->poly = a->shapes[i]->poly;
        node->shapes[i]->border_color = a->shapes[i]->border_color;
        node->shapes[i]->fill_color = a->shapes[i]->fill_color;
    }
    for(auto i = 0; i < b->shapes.size(); i ++) {
        node->shapes[i+a->shapes.size()]->poly = b->shapes[i]->poly;
        node->shapes[i+a->shapes.size()]->border_color = b->shapes[i]->border_color;
        node->shapes[i+a->shapes.size()]->fill_color = b->shapes[i]->fill_color;
    }
#else
#error Wrong option
#endif
}

CSGTree::OpNode* CSGTree::Update_Union(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = union_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = union_op;
    return res_node;
}

CSGTree::OpNode* CSGTree::Update_Difference(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = subtract_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = difference_op;
    return res_node;
}

CSGTree::OpNode* CSGTree::Update_Intersection(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b) {
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = intersect_polygons(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = intersection_op;
    return res_node;
}

CSGTree::OpNode* CSGTree::Update_XOR(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = xor_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = xor_op;
    return res_node;
}

CSGTree::OpNode* CSGTree::Update_PlaceInShape(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto temp = intersect_polygons(b_polys, a_polys);
    auto result = subtract_polygons_clipper(a_polys, temp);
    result.insert(result.end(), temp.begin(), temp.end());
    
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = place_in_op;
    return res_node;
}


#if 1

CSGTree::OpNode* CSGTree::New_Sum(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b) {
#if SPEEDUP_ALIASSUM
    auto res_node = CSGTree::BuildResult_SumAlias(tree, a, b);
#else
    auto res_node = CSGTree::BuildResult_Sum(tree, a, b);
#endif
    res_node->op_type = sum_op;
    //    if (update){
    a->parent = res_node;
    b->parent = res_node;
    CSGTree::AddNode(tree, res_node);
    //    }
    return res_node;
}

#else

CSGTree::OpNode* CSGTree::New_Sum(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b) {
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    a_polys.insert(a_polys.end(), b_polys.begin(), b_polys.end());
    auto res_node = CSGTree::BuildResult(tree, a_polys, a, b);
    res_node->op_type = sum_op;
    //    if (update){
    a->parent = res_node;
    b->parent = res_node;
    CSGTree::AddNode(tree, res_node);
    //    }
    return res_node;
}

#endif

CSGTree::OpNode* CSGTree::New_Union(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = union_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = union_op;
//    if(update) {
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
//    }
    return res_node;
}

CSGTree::OpNode* CSGTree::New_Difference(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = subtract_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = difference_op;
//    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
//    }
    return res_node;
}

CSGTree::OpNode* CSGTree::New_Intersection(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b) {
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = intersect_polygons(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = intersection_op;
//    if(update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
//    }
    return res_node;
}

CSGTree::OpNode* CSGTree::New_XOR(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto result = xor_polygons_clipper(a_polys, b_polys);
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = xor_op;
//    if(update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
//    }
    return res_node;
}

CSGTree::OpNode* CSGTree::New_PlaceInShape(CSGTree::Tree* tree, CSGTree::Node* a, CSGTree::Node* b){
    vector<polygon2r> a_polys = make_vector(a->shapes, [&](AnimatedShape* shape){return shape->poly;});
    vector<polygon2r> b_polys = make_vector(b->shapes, [&](AnimatedShape* shape){return shape->poly;});
    auto temp = intersect_polygons(b_polys, a_polys);
    auto result = subtract_polygons_clipper(a_polys, temp);
    result.insert(result.end(), temp.begin(), temp.end());
    
    auto res_node = CSGTree::BuildResult(tree, result, a, b);
    res_node->op_type = place_in_op;
//    if (update){
        a->parent = res_node;
        b->parent = res_node;
        CSGTree::AddNode(tree, res_node);
//    }
    return res_node;
}

void CSGTree::UpdateLeafNode(CSGTree::Tree* tree, CSGTree::LeafNode* a, const Animator& anim, double current_time, double incr, double total_dur, bool update){
    anim(a->shapes, current_time, incr, total_dur);
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


void CSGTree::UpdateOpNode(CSGTree::Tree* tree, CSGTree::OpNode* a) {
    switch (a->op_type) {
        case union_op:
        {
            printf("Updating union \n");
            auto temp = CSGTree::Update_Union(tree, a->child_left, a->child_right);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case intersection_op:
        {
            printf("Updating intersection \n");
            auto temp = CSGTree::Update_Intersection(tree, a->child_left, a->child_right);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case difference_op:
        {
            printf("Updating difference \n");
            auto temp = CSGTree::Update_Difference(tree, a->child_left, a->child_right);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case xor_op:
        {
            printf("Updating xor \n");
            auto temp = CSGTree::Update_XOR(tree, a->child_left, a->child_right);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        case sum_op:
        {
            printf("Updating sum \n");
            CSGTree::Update_Sum(tree, a);
            break;
        }
        case place_in_op:
        {
            printf("Updating place_in \n");
            auto temp = CSGTree::Update_PlaceInShape(tree, a->child_left, a->child_right);
            for(auto s : a->shapes) if(s) delete s;
            a->shapes = temp->shapes;
            break;
        }
        default:
            printf("ERROR - Invalid animator op for update leaf content. \n");
            break;
    }
}
