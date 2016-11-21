//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include "time_manager.h"
#include "expansion_manager.h"
#include "svg.h"

CSGTree::Tree* initialize_tree(Grammar* g) {
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
    auto s4   = make_polyline_segment({-500.1, -200.6}, {200.3, 400.7}, resolution);
    
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


int main(int argc, const char * argv[]) {
    
    auto em = (HistoryAnim*)(make_history(animation_history));
    auto grammar = get_grammar(grammar_filename);
    auto tree = initialize_tree(grammar);
    
    auto init_step = matching_init();
    auto init_shapes = init_step->op(ShapeGroup(), init_step->produced_tags, init_step->parameters, grammar->rn, nullptr, nullptr, tree);
    auto init_partition = PartitionShapeGroup();
    init_partition.added = init_shapes;
    init_partition.remainder = ShapeGroup();
    init_partition.match = ShapeGroup();
    
    update_history(em, init_partition, init_step, tree);
    
    ((ExpansionAnim*)(em->history.back()))->tree = tree;
    
    TimeManager::printTimeLine(((ExpansionAnim*)(em->history.back()))->timeline);
    
    while (expand(em)){
        printf("Expanding... \n");
        TimeManager::printTimeLine(((ExpansionAnim*)(em->history.back()))->timeline);
    };

    auto last_exp = ((ExpansionAnim*)(em->history.back()));
    auto duration = last_exp->timeline->duration;
    double frame_rate = 30.0;
    double frame_step = 1.0 / frame_rate;
    auto k = 1;
    double prev_step = 0.0;
    
    stringstream ss;
    ss << std::setfill('0') << std::setw(3) << 0;
    save_svg(last_exp->tree, {1000, 1000}, {500, 500}, {1.0, 1.0}, ss.str());
    
    for (auto i = frame_step; i < duration; i = i + frame_step){
        TimeManager::AnimateTimeLine(last_exp->timeline, last_exp->tree, i, i - prev_step);
        stringstream ss1;
        ss1 << std::setfill('0') << std::setw(3) << k;
        save_svg(last_exp->tree, {1000, 1000}, {500, 500}, {1.0, 1.0}, ss1.str());
        k++;
        prev_step = i;
        printf("Animating frame %d...", k);
    }
    
    // - improve the offset : don't use keyframes anymore, use those indices as POI in the slice timeline
    //              SLICE
    //      |===================|
    //          ###########
    //              ANIM
    // - [ ]easing
    //    - [x]implementation of log mat

    // - [x] play with grouping -> all the rules (matching included) should be the same,
    //   it changes only on what type of slice+shape they act
    
    //convert -density 40 -resize 500x500 svg/*.svg -set filename:base "%[base]" png/"%[filename:base].png"
    //convert -delay 1/8 -loop 0 png/*.png animated4.gif
    
    printf("end main\n");
}
