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
    
    auto s1     = make_polyline_rect({-50.0, -50.0}, {50.0, 50.0}, resolution);
    auto s1_1   = make_polyline_rect({-50.0, -50.0}, {50.0, 50.0}, resolution);
    auto s1_2   = make_polyline_rect({-50.0, -50.0}, {50.0, 50.0}, resolution);
    auto s1_3   = make_polyline_rect({-50.0, -50.0}, {50.0, 50.0}, resolution);
    
    auto s3     = make_polyline_rect({-300.0, -300.0}, {300.0, 300.0}, resolution);
    
    auto s4     = make_polyline_segment({-1500.1, -1200.4}, {1200.1, 1500.3}, resolution, true);
    auto s4_1   = make_polyline_segment({-1500.0, -1175.0}, {1175.0, 1500.0}, resolution, true);
    auto s4_2   = make_polyline_segment({-1500.0, -1150.0}, {1150.0, 1500.0}, resolution, true);
    auto s4_3   = make_polyline_segment({-1500.0, -1125.0}, {1125.0, 1500.0}, resolution, true);
    
    auto s5     = make_polyline_circle({300.0, 0.0}, 75.0, resolution);
    
    auto s6     = make_polyline_rect({400.0, -300.0}, {450.0, -250.0}, resolution);
    auto s6_1   = make_polyline_rect({400.0, -200.0}, {450.0, -150.0}, resolution);
    auto s6_2   = make_polyline_rect({400.0, -100.0}, {450.0, -50.0}, resolution);
    
    // Leaf nodes
    auto n1     = CSGTree::AddShape(tree, new AnimatedShape({s1}));
    auto n1_1   = CSGTree::AddShape(tree, new AnimatedShape({s1_1}));
    auto n1_2   = CSGTree::AddShape(tree, new AnimatedShape({s1_2}));
    auto n1_3   = CSGTree::AddShape(tree, new AnimatedShape({s1_3}));
    
    auto n3     = CSGTree::AddShape(tree, new AnimatedShape({s3}));

    auto n4     = CSGTree::AddShape(tree, new AnimatedShape({s4}));
    auto n4_1   = CSGTree::AddShape(tree, new AnimatedShape({s4_1}));
    auto n4_2   = CSGTree::AddShape(tree, new AnimatedShape({s4_2}));
    auto n4_3   = CSGTree::AddShape(tree, new AnimatedShape({s4_3}));
    
    auto n5     = CSGTree::AddShape(tree, new AnimatedShape({s5}));
    
    auto n6     = CSGTree::AddShape(tree, new AnimatedShape({s6}));
    auto n6_1   = CSGTree::AddShape(tree, new AnimatedShape({s6_1}));
    auto n6_2   = CSGTree::AddShape(tree, new AnimatedShape({s6_2}));
    
    // square union
    auto op1_1 = CSGTree::Union(tree, n1, n1_1);
    auto op1_2 = CSGTree::Union(tree, op1_1, n1_2);
    auto op1_3 = CSGTree::Union(tree, op1_2, n1_3);
    
    //Lines union
    auto op2_1 = CSGTree::Sum(tree, n4, n4_1);
    auto op2_2 = CSGTree::Sum(tree, op2_1, n4_2);
    auto op2_3 = CSGTree::Sum(tree, op2_2, n4_3);
    
    //Little squares union
    auto op6_1 = CSGTree::Sum(tree, n6, n6_1);
    auto op6_2 = CSGTree::Sum(tree, op6_1, n6_2);

    //Final shape composition
    auto fop1 = CSGTree::Sum(tree, op1_3, op2_3);
    auto fop2 = CSGTree::Union(tree, n3, n5);
    auto fop3 = CSGTree::PlaceInShape(tree, fop2, fop1);
    auto fop4 = CSGTree::Sum(tree, fop3, op6_2);
    
    save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, "0");
    
    auto r_p = rule_params();
    r_p[0] = 0.3; r_p[1] = 0.0; r_p[2] = 0.0; r_p[3] = 0.3; r_p[4] = 400.0; r_p[5] = 400.0;
    auto anim = Animator(anim_eulerian, r_p);
    
    auto r_p2 = rule_params();
    r_p2[0] = 0.3; r_p2[1] = 0.0; r_p2[2] = 0.0; r_p2[3] = 0.3; r_p2[4] = 400.0; r_p2[5] = -400.0;
    auto anim2 = Animator(anim_eulerian, r_p2);
    
    auto r_p3 = rule_params();
    r_p3[0] = 0.3; r_p3[1] = 0.0; r_p3[2] = 0.0; r_p3[3] = 0.3; r_p3[4] = -400.0; r_p3[5] = 400.0;
    auto anim3 = Animator(anim_eulerian, r_p3);
    
    auto r_p4 = rule_params();
    r_p4[0] = 0.3; r_p4[1] = 0.0; r_p4[2] = 0.0; r_p4[3] = 0.3; r_p4[4] = -400.0; r_p4[5] = -400.0;
    auto anim4 = Animator(anim_eulerian, r_p4);
    
    auto r_p5 = rule_params();
    r_p5[0] = 1.0; r_p5[1] = 0.0; r_p5[2] = 0.0; r_p5[3] = 1.0; r_p5[4] = 0.0; r_p5[5] = 100.0;
    auto anim5 = Animator(anim_eulerian, r_p5);
    
    auto r_p6 = rule_params();
    r_p6[0] = 1.0; r_p6[1] = 0.0; r_p6[2] = 0.0; r_p6[3] = 1.0; r_p6[4] = 0.0; r_p6[5] = 200.0;
    auto anim6 = Animator(anim_wave, r_p6);
    
    auto frames = 30.0;
    auto cur_delta = 0.0;
    for (auto i = 0; i <= frames; i++){
        cur_delta = (double)i / frames;
        
        CSGTree::UpdateLeafNode(tree, n1, anim, 1.0/frames);
        CSGTree::UpdateLeafNode(tree, n1_1, anim2, 1.0/frames);
        CSGTree::UpdateLeafNode(tree, n1_2, anim3, 1.0/frames);
        CSGTree::UpdateLeafNode(tree, n1_3, anim4, 1.0/frames);
        
        CSGTree::UpdateLeafNode(tree, n5, anim5, 1.0/frames);
        
        CSGTree::UpdateLeafNode(tree, n4, anim2, 1.0/frames);
        CSGTree::UpdateLeafNode(tree, n6, anim6, cur_delta);
        CSGTree::UpdateLeafNode(tree, n6_1, anim6, cur_delta);
        CSGTree::UpdateLeafNode(tree, n6_2, anim6, cur_delta);
        
        if (i >= frames/4.0) {
            CSGTree::UpdateLeafNode(tree, n4_1, anim2, 1.25/frames);
        }
        if (i >= 2.0 * frames/4.0){
            CSGTree::UpdateLeafNode(tree, n4_2, anim2, 1.5/frames);
        }
        if (i >= 3.0 * frames/4.0){
            CSGTree::UpdateLeafNode(tree, n4_3, anim2, 1.75/frames);
        }
        
        save_svg(tree, {800, 800}, {350, 350}, {1.0, 1.0}, std::to_string(i+1));
    }

    printf("end main");
}
