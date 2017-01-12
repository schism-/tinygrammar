//
//  input_figures.cpp
//  tinygrammar
//
//  Created by Christian on 22/12/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "input_figures.h"

CSGTree::Tree* standard_example(Grammar* g) {
    auto tree = CSGTree::InitTree();
    auto s5_1 = make_polyline_rect({350.0, -325.0}, {400.0, -275.0}, resolution);
    auto s5_2 = make_polyline_rect({350.0, -225.0}, {400.0, -175.0}, resolution);
    auto s5_3 = make_polyline_rect({350.0, -125.0}, {400.0, -75.0}, resolution);
    auto s5_4 = make_polyline_rect({350.0, -25.0},  {400.0, 25.0}, resolution);
    auto s5_5 = make_polyline_rect({350.0, 75.0},   {400.0, 125.0}, resolution);
    auto s5_6 = make_polyline_rect({350.0, 175.0},  {400.0, 225.0}, resolution);
    auto s5_7 = make_polyline_rect({350.0, 275.0},  {400.0, 325.0}, resolution);
    
    auto s1   = make_polyline_rect({-100.0, -100.0}, {100.0, 100.0}, resolution);
    auto s2   = make_polyline_rect({-100.0, -100.0}, {100.0, 100.0}, resolution);
    
    auto s3   = make_polyline_rect({-300.0, -300.0}, {300.0, 300.0}, resolution);
//    auto s4   = make_polyline_segment({-500.1, -200.6}, {200.3, 400.7}, resolution);
    
    auto l_s_tag = add_tags(g, {"l_s"});
    auto m_s_tag = add_tags(g, {"m_s"});
    auto b_s_tag = add_tags(g, {"b_s"});
    auto seg_tag = add_tags(g, {"seg"});
    
    // Medium squares
    auto n1   = CSGTree::AddShape(tree, new AnimatedShape({s1}, m_s_tag[0], 0));
    auto n2   = CSGTree::AddShape(tree, new AnimatedShape({s2}, m_s_tag[0], 1));
    
    // Big square
    auto n3   = CSGTree::AddShape(tree, new AnimatedShape({s3}, b_s_tag[0], 0));
    
    // Segment
    //auto n4   = CSGTree::AddShape(tree, new AnimatedShape({s4}, seg_tag[0], 0));
    
    // Little squares
    auto n5_1 = CSGTree::AddShape(tree, new AnimatedShape({s5_1}, l_s_tag[0], 0));
    auto n5_2 = CSGTree::AddShape(tree, new AnimatedShape({s5_2}, l_s_tag[0], 1));
    auto n5_3 = CSGTree::AddShape(tree, new AnimatedShape({s5_3}, l_s_tag[0], 2));
    auto n5_4 = CSGTree::AddShape(tree, new AnimatedShape({s5_4}, l_s_tag[0], 3));
    auto n5_5 = CSGTree::AddShape(tree, new AnimatedShape({s5_5}, l_s_tag[0], 4));
    auto n5_6 = CSGTree::AddShape(tree, new AnimatedShape({s5_6}, l_s_tag[0], 5));
    auto n5_7 = CSGTree::AddShape(tree, new AnimatedShape({s5_7}, l_s_tag[0], 6));
    
    // Tree creation
    auto op1    = CSGTree::Union(tree, n1, n2);
    
    auto op3    = CSGTree::Sum(tree, n5_1,  n5_2);
    auto op3_1  = CSGTree::Sum(tree, op3,   n5_3);
    auto op3_2  = CSGTree::Sum(tree, op3_1, n5_4);
    auto op3_3  = CSGTree::Sum(tree, op3_2, n5_5);
    auto op3_4  = CSGTree::Sum(tree, op3_3, n5_6);
    auto op3_5  = CSGTree::Sum(tree, op3_4, n5_7);
    
    auto op6    = CSGTree::PlaceInShape(tree, n3, op1);
    
    auto op3_6  = CSGTree::Sum(tree, op3_5, op6);
    
    return tree;
}

CSGTree::Tree* little_squares(Grammar* g, int extra_value) {
    auto tree = CSGTree::InitTree();
    
    auto num_hor = extra_value;
    auto num_ver = extra_value;
    auto padding = 50.0;
    auto dim = 50.0;
    auto max_hor = dim * (double)num_hor + padding * (double)(num_hor - 1);
    auto max_ver = dim * (double)num_ver + padding * (double)(num_ver - 1);
    
    auto l_s_tag = add_tags(g, {"l_s"});
    auto last_op = (CSGTree::Node*)nullptr;
    
    for (auto j = 0 ; j < num_ver ; j++){
        for (auto i = 0 ; i < num_hor ; i++){
            auto x_0 = (double)i * (padding + dim) - max_hor / 2.0;
            auto x_1 = (double)i * (padding + dim) + dim  - max_hor / 2.0;
            auto y_0 = (double)j * (padding + dim) - max_ver / 2.0;
            auto y_1 = (double)j * (padding + dim) + dim  - max_ver / 2.0;
            auto poly  = make_polyline_rect({x_0, y_0}, {x_1, y_1}, resolution);
            auto shape = CSGTree::AddShape(tree, new AnimatedShape({poly}, l_s_tag[0], i + j * num_hor));
            
            if (i == 0 && j == 0) {
                last_op = shape;
            }
            else{
                last_op = CSGTree::Sum(tree, last_op, shape);
            }
        }
    }
    
    return tree;
}

CSGTree::Tree* squares_and_circles(Grammar* g, int extra_value) {
    auto tree = CSGTree::InitTree();
    
    auto num_hor = extra_value;
    auto num_ver = extra_value;
    auto padding = 50.0;
    auto dim = 50.0;
    auto max_hor = dim * (double)num_hor + padding * (double)(num_hor - 1);
    auto max_ver = dim * (double)num_ver + padding * (double)(num_ver - 1);
    
    auto l_s_tag = add_tags(g, {"l_s"});
    auto l_c_tag = add_tags(g, {"l_c"});
    auto last_op = (CSGTree::Node*)nullptr;
    
    auto poly = polyline2r();
    auto shape = (CSGTree::LeafNode*)nullptr;
    
    auto k = 0;
    for (auto j = 0 ; j < num_ver ; j++){
        for (auto i = 0 ; i < num_hor ; i++){
            if (k % 2 == 0){
                auto x_0 = (double)i * (padding + dim) - max_hor / 2.0;
                auto x_1 = (double)i * (padding + dim) + dim  - max_hor / 2.0;
                auto y_0 = (double)j * (padding + dim) - max_ver / 2.0;
                auto y_1 = (double)j * (padding + dim) + dim  - max_ver / 2.0;
                poly  = make_polyline_rect({x_0, y_0}, {x_1, y_1}, resolution);
                shape = CSGTree::AddShape(tree, new AnimatedShape({poly}, l_s_tag[0], i + j * num_hor));
            }
            else {
                auto center_x = (double)i * (padding + dim) - max_hor / 2.0 + dim/2.0;
                auto center_y = (double)j * (padding + dim) - max_ver / 2.0 + dim/2.0;
                poly  = make_polyline_circle({center_x, center_y}, 25.0, resolution);
                shape = CSGTree::AddShape(tree, new AnimatedShape({poly}, l_c_tag[0], i + j * num_hor));
            }
            
            
            if (i == 0 && j == 0) {
                last_op = shape;
            }
            else{
                last_op = CSGTree::Sum(tree, last_op, shape);
            }
            k++;
        }
    }
    
    return tree;
}

CSGTree::Tree* load_svg(Grammar* g, string filename) {
    auto tree = CSGTree::InitTree();
    auto shapes = load_svg(filename);
    auto tag_count = map<int, int>();
    auto last_op = (CSGTree::Node*)nullptr;
    auto shape = (CSGTree::LeafNode*)nullptr;
    auto first = true;
    for (auto&& s : shapes){
        auto temp = (AnimatedShape*)s;
        
        std::stringstream test(temp->literal_tag);
        std::string segment;
        std::vector<std::string> seglist;
        while(std::getline(test, segment, '_'))
        {
            seglist.push_back(segment);
        }
        
        auto tag_id = add_tags(g, {seglist[0]});
        temp->tag = tag_id[0];
        temp->tid = atoi(seglist[1].c_str());;
        shape = CSGTree::AddShape(tree, temp);
        
        if (first) { last_op = shape; first = false; }
        else last_op = CSGTree::Union (tree, last_op, shape);  //test 2
//        else {
//            if (temp->tag == tag_to_mapping(g, "place")){
//                auto t_bb = bounds_polygon(temp->poly);
//                auto c = ym_rcenter(t_bb);
//                auto sq = make_polyline_rect({c.x - 5.0, c.y - 5.0}, {c.x + 5.0, c.y + 5.0}, resolution);
//                auto s_temp = new AnimatedShape();
//                auto s_t = add_tags(g, {"in_place"});
//                s_temp->literal_tag = "in_place";
//                s_temp->tag = s_t[0];
//                s_temp->tid = 0;
//                s_temp->poly = {sq};
//                auto s_shape = CSGTree::AddShape(tree, s_temp);
//                auto res = CSGTree::PlaceInShape(tree, shape, s_shape);
//                last_op = CSGTree::Sum(tree, last_op, res);
//            }
//            else if (temp->tag == tag_to_mapping(g, "graph")){
//                auto t_bb = bounds_polygon(temp->poly);
//                auto c = ym_rcenter(t_bb);
//                auto seg1 = make_polyline_segment({t_bb.min.x - 10.0, c.y - 3.0}, {t_bb.max.x + 10.0, c.y + 10.0}, resolution);
//                auto seg2 = make_polyline_segment({t_bb.min.x - 10.0, c.y - 6.0}, {t_bb.max.x + 10.0, c.y - 13.0}, resolution);
//                auto s_temp = new AnimatedShape();
//                auto s_t = add_tags(g, {"seg"});
//                s_temp->literal_tag = "seg";
//                s_temp->tag = s_t[0];
//                s_temp->tid = 0;
//                s_temp->poly = {seg1};
//                auto s_shape = CSGTree::AddShape(tree, s_temp);
//                
//                auto s2_temp = new AnimatedShape();
//                auto s2_t = add_tags(g, {"seg2"});
//                s2_temp->literal_tag = "seg2";
//                s2_temp->tag = s2_t[0];
//                s2_temp->tid = 0;
//                s2_temp->poly = {seg2};
//                auto s2_shape = CSGTree::AddShape(tree, s2_temp);
//
//                
//                auto res = CSGTree::Sum(tree, s_shape, s2_shape);
//                res = CSGTree::PlaceInShape(tree, shape, res);
//                
//                last_op = CSGTree::Sum(tree, last_op, res);
//            }
//            else if (temp->tag == tag_to_mapping(g, "tachi")){
//                auto t_bb = bounds_polygon(temp->poly);
//                auto c = ym_rcenter(t_bb);
//                auto seg1 = make_polyline_segment({c.x, t_bb.max.y + 10.0}, {c.x, t_bb.max.y - 10.0}, resolution);
//
//                auto s_temp = new AnimatedShape();
//                auto s_t = add_tags(g, {"seg_tachi"});
//                s_temp->literal_tag = "seg_tachi";
//                s_temp->tag = s_t[0];
//                s_temp->tid = 0;
//                s_temp->poly = {seg1};
//                auto s_shape = CSGTree::AddShape(tree, s_temp);
//                
//                auto res = CSGTree::PlaceInShape(tree, shape, s_shape);
//                
//                last_op = CSGTree::Sum(tree, last_op, res);
//            }
//            else {
//                last_op = CSGTree::Sum(tree, last_op, shape);  //test 3
//            }
//        }
    }
    
    return tree;
}


CSGTree::Tree* initialize_tree(Grammar* g, int example_num, int extra_value, string filename) {
    switch (example_num) {
        case 0: {
            return standard_example(g);
            break;
        }
        case 1: {
            return little_squares(g, extra_value);
            break;
        }
        case 2: {
            return squares_and_circles(g, extra_value);
            break;
        }
        case 3: {
            return load_svg(g, filename);
            break;
        }
        default:
        {
            return standard_example(g);
            break;
        }
    }
}

