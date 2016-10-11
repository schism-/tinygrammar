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
    
    auto s5_1 = make_polyline_rect({350.0, -325.0}, {400.0, -275.0}, 1.0);
    auto s5_2 = make_polyline_rect({350.0, -225.0}, {400.0, -175.0}, 1.0);
    auto s5_3 = make_polyline_rect({350.0, -125.0}, {400.0, -75.0}, 1.0);
    auto s5_4 = make_polyline_rect({350.0, -25.0},  {400.0, 25.0}, 1.0);
    auto s5_5 = make_polyline_rect({350.0, 75.0},   {400.0, 125.0}, 1.0);
    auto s5_6 = make_polyline_rect({350.0, 175.0},  {400.0, 225.0}, 1.0);
    auto s5_7 = make_polyline_rect({350.0, 275.0},  {400.0, 325.0}, 1.0);

    auto s1   = make_polyline_rect({-100.0, -100.0}, {100.0, 100.0}, resolution);
    auto s2   = make_polyline_rect({0.0, 0.0}, {200.0, 200.0}, resolution);
    auto s3   = make_polyline_rect({-300.0, -300.0}, {300.0, 300.0}, resolution);
    auto s4   = make_polyline_segment({-400.0, -200.0}, {-400.0, -200.0}, resolution);
    
    auto n1   = CSGTree::AddShape(tree, new AnimatedShape({s1}));
    auto n2   = CSGTree::AddShape(tree, new AnimatedShape({s2}));
    auto n3   = CSGTree::AddShape(tree, new AnimatedShape({s3}));
    auto n4   = CSGTree::AddShape(tree, new AnimatedShape({s4}));
    
    auto n5_1 = CSGTree::AddShape(tree, new AnimatedShape({s5_1}));
    auto n5_2 = CSGTree::AddShape(tree, new AnimatedShape({s5_2}));
    auto n5_3 = CSGTree::AddShape(tree, new AnimatedShape({s5_3}));
    auto n5_4 = CSGTree::AddShape(tree, new AnimatedShape({s5_4}));
    auto n5_5 = CSGTree::AddShape(tree, new AnimatedShape({s5_5}));
    auto n5_6 = CSGTree::AddShape(tree, new AnimatedShape({s5_6}));
    auto n5_7 = CSGTree::AddShape(tree, new AnimatedShape({s5_7}));
    
    
    auto op1    = CSGTree::Union(tree, n1, n2);
    auto op2    = CSGTree::Union(tree, op1, n4);
    
    auto op3    = CSGTree::Union(tree, n5_1,  n5_2);
    auto op3_1  = CSGTree::Union(tree, op3,   n5_3);
    auto op3_2  = CSGTree::Union(tree, op3_1, n5_4);
    auto op3_3  = CSGTree::Union(tree, op3_2, n5_5);
    auto op3_4  = CSGTree::Union(tree, op3_3, n5_6);
    auto op3_5  = CSGTree::Union(tree, op3_4, n5_7);
    
    auto op3_6  = CSGTree::Union(tree, op3_5, n3);
    
    auto op6    = CSGTree::PlaceInShape(tree, op3_6, op2);

    save_svg(tree, {800, 800}, {400, 400}, {1.0, 1.0}, "0");
    
    auto bbox = ym_range2r({-1000.0, -1000.0}, {1000.0, 1000.0});

    auto mat = ym_affine2r({{1.0, 0.0},{0.0, 1.0}, {1.0, -1.0}});
    auto am  = AnimatorMatrix(bbox, mat);
    auto akf = AnimatorKeyframes(am, 30);
    auto anim_new = Animator(anim_single, akf);
    
    auto mat2 = ym_affine2r({{0.99, 0.0},{0.0, 0.99}, {0.0, 0.0}});
    auto am2  = AnimatorMatrix(bbox, mat2);
    auto anim_new2 = Animator(anim_single, {am2, 30});
    
    auto anim_new3 = Animator(anim_group, {move_towards_point(bbox, {0.0, 0.0}, 25.0), 30});
    
    auto anim_new4 = Animator(anim_group, {move_towards_point(bbox, {1000.0, 0.0}, 25.0), 30});
    
    for (auto i = 0; i <= 29; i++){
        CSGTree::UpdateLeafNode(tree, n2, anim_new2, i);
        CSGTree::UpdateLeafNode(tree, n3, anim_new2, i);
        CSGTree::UpdateLeafNode(tree, n4, anim_new, i);
        
        if (i > 10 && i <= 20) CSGTree::UpdateLeafNode(tree, {n5_1, n5_2, n5_3, n5_4, n5_5, n5_6, n5_7} , anim_new3, i);

        if (i > 20) CSGTree::UpdateLeafNode(tree, {n5_1, n5_2, n5_3, n5_4, n5_5, n5_6, n5_7} , anim_new4, i);
        
        save_svg(tree, {800, 800}, {400, 400}, {1.0, 1.0}, std::to_string(i+1));
    }

    printf("end main");
}
