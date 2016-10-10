//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include "tangle_utilities.h"
#include "expansion_manager.h"
#include "svg.h"

int main(int argc, const char * argv[]) {
    #if 0
    auto em = make_history(0);
    expand_init(em);
    while (expand(em)){
        printf("...");
    };
    save_svg(em, {600,600}, {300, 300}, {1.0, 1.0});
    return 0;
    #endif
    
    auto tree = CSGTree::InitTree();
    auto s1 = make_polyline_rect({-100.0, -100.0}, {100.0, 100.0}, 1.0);
    auto s2 = make_polyline_rect({0.0, 0.0}, {200.0, 200.0}, 1.0);
    auto s3 = make_polyline_rect({-300.0, -300.0}, {300.0, 300.0}, 1.0);
    auto s4 = make_polyline_segment({-501.6, -199.8}, {201.3, 498.4}, 1.0, true);
    
    
//    auto n1 = CSGTree::AddShape(tree, new AnimatedShape({s1}));
//    auto n2 = CSGTree::AddShape(tree, new AnimatedShape({s2}));
//    auto n1_c = CSGTree::CopyNode(tree, n1);
//    auto n2_c = CSGTree::CopyNode(tree, n2);
    
    auto n1 = CSGTree::AddShape(tree, new AnimatedShape({s1}));
    auto n2 = CSGTree::AddShape(tree, new AnimatedShape({s2}));
    auto n3 = CSGTree::AddShape(tree, new AnimatedShape({s3}));
    auto n4 = CSGTree::AddShape(tree, new AnimatedShape({s4}));
    
    auto op1 = CSGTree::Union(tree, n1, n2);
    auto op2 = CSGTree::Union(tree, op1, n4);
    auto op6 = CSGTree::PlaceInShape(tree, n3, op2);
    save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, "0");
    
    auto r_p = rule_params();
    r_p[0] = 1.0;   r_p[1] = 0.0;
    r_p[2] = 0.0;   r_p[3] = 1.0;
    r_p[4] = 150.0;  r_p[5] = 150.0;
    auto anim = Animator(anim_eulerian, r_p);
    
    auto r_p2 = rule_params();
    r_p2[0] = 1.0;   r_p2[1] = 0.0;
    r_p2[2] = 0.0;   r_p2[3] = 1.0;
    r_p2[4] = 100.0;  r_p2[5] = -100.0;
    auto anim2 = Animator(anim_eulerian, r_p2);
    
    auto bbox = ym_range2r({-1000.0, -1000.0}, {1000.0, 1000.0});
    auto mat = ym_affine2r({{1.0, 0.0},{0.0, 1.0}, {5.0, -5.0}});
    auto am  = AnimatorMatrix(bbox, mat);
    auto akf = AnimatorKeyframes(am, 30);
    auto anim_new = Animator(anim_single, akf);
    
    for (auto i = 0; i <= 10; i++){
        
        CSGTree::UpdateLeafNode(tree, n2, anim_new, i);
        CSGTree::UpdateLeafNode(tree, n3, anim_new, i);
        CSGTree::UpdateLeafNode(tree, n4, anim_new, i);
        
        save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, std::to_string(i+1));
    }

    printf("end main");
}
