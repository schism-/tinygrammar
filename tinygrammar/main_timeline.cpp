//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include "time_manager.h"
#include "expansion_manager.h"

int main(int argc, const char * argv[]) {
//    auto bbox   = ym_range2r({-2000.0, -2000.0}, {2000.0, 2000.0});
//    auto am     = AnimatorMatrix(bbox, {{0.95, 0.0},{0.0, 0.95}, {0.0, 0.0}});
//    auto akf    = AnimatorKeyframes({am}, vector<int>{1});
//    auto anim   = Animator(anim_single, akf);
//    
//    auto tl = new TimeManager::TimeLine(10.0, akf);
//    
//    auto slices = TimeManager::GetAllSlices(tl);
//    
//    TimeManager::TimeSliceCut(tl, slices[0]);
//    slices = TimeManager::GetAllSlices(tl);
//    
//    TimeManager::TimeSliceCut(tl, (TimeManager::GetAllSlices(tl))[0]);
//    slices = TimeManager::GetAllSlices(tl);
//    
//    TimeManager::TimeSliceCut(tl, (TimeManager::GetAllSlices(tl))[0]);
//    slices = TimeManager::GetAllSlices(tl);
    
    auto em = (HistoryAnim*)(make_history(animation_history));
    
    auto grammar = get_grammar(grammar_filename);
    
    auto init_step = matching_init();
    auto init_shapes = init_step->op(ShapeGroup(), init_step->produced_tags, init_step->parameters, grammar->rn);
    auto init_partition = PartitionShapeGroup();
    init_partition.added = init_shapes;
    init_partition.remainder = ShapeGroup();
    init_partition.match = ShapeGroup();
    
    update_history(em, init_partition, init_step);
    
    while (expand(em)){
        printf("Expanding... \n");
    };

    
    printf("end main\n");
}
