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
    auto s1 = make_polyline_rect({-100, -100}, {100, 100});
    auto s2 = make_polyline_rect({0, 0}, {200, 200});
    auto s3 = make_polyline_rect({-300, -300}, {300, 300});
    
    auto n1 = CSGTree::AddShape(tree, new AnimatedShape({s1}));
    auto n2 = CSGTree::AddShape(tree, new AnimatedShape({s2}));
    auto n1_c = CSGTree::CopyNode(tree, n1);
    auto n2_c = CSGTree::CopyNode(tree, n2);
    
    auto n3 = CSGTree::AddShape(tree, new AnimatedShape({s3}));
    
    auto op1 = CSGTree::XOR(tree, n1, n2);
    save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, "1");
    
    auto op2 = CSGTree::Difference(tree, n3, op1);
    auto op4 = CSGTree::XOR(tree, n1_c, n2_c);
    auto op3 = CSGTree::Sum(tree, op2, op4);
    save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, "2");
    
    auto r_p = rule_params();
    r_p[0] = 1.0; r_p[1] = 0.0; r_p[2] = 0.0; r_p[3] = 1.0;
    r_p[4] = 30.0; r_p[5] = 30.0;
    auto anim = Animator(anim_eulerian, r_p);
    
    auto r_p_2 = rule_params();
    r_p_2[0] = 1.0;  r_p_2[1] = 0.0; r_p_2[2] = 0.0; r_p_2[3] = 1.0;
    r_p_2[4] = 10.0; r_p_2[5] = -10.0;
    auto anim2 = Animator(anim_eulerian, r_p_2);
    
    for (auto i = 0; i <= 20; i++){
        CSGTree::UpdateLeafNode(tree, n1, anim, 1.0/20.0);
        CSGTree::UpdateLeafNode(tree, n2, anim2, 1.0/20.0);
        save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, std::to_string(i));
    }

    printf("end main");
}
