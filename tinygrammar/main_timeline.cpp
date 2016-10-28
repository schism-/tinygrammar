//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include "time_manager.h"

int main(int argc, const char * argv[]) {
    auto bbox   = ym_range2r({-2000.0, -2000.0}, {2000.0, 2000.0});
    auto am     = AnimatorMatrix(bbox, {{0.95, 0.0},{0.0, 0.95}, {0.0, 0.0}});
    auto akf    = AnimatorKeyframes({am}, vector<int>{1});
    auto anim   = Animator(anim_single, akf);
    
    auto tl = new TimeManager::TimeLine(10.0, akf);
    
    auto slices = TimeManager::GetAllSlices(tl);
    
    TimeManager::TimeSliceCut(tl, slices[0]);
    slices = TimeManager::GetAllSlices(tl);
    
    TimeManager::TimeSliceCut(tl, (TimeManager::GetAllSlices(tl))[0]);
    slices = TimeManager::GetAllSlices(tl);
    
    TimeManager::TimeSliceCut(tl, (TimeManager::GetAllSlices(tl))[0]);
    slices = TimeManager::GetAllSlices(tl);
    
    printf("end main\n");
}
