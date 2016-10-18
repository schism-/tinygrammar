//
//  time_manager.cpp
//  tinygrammar
//
//  Created by Christian on 14/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "time_manager.h"

TimeManager::NodeTimeLine* TimeManager::FindTimeLine (TimeManager::TimeLine* t, TimeManager::TimeSlice* slice){
    for (auto&& ntm : t->timelines){
        if (find(ntm->slices.begin(), ntm->slices.end(), slice) != ntm->slices.end()) return ntm;
    }
    printf("[ERROR] FindTimeLine : NodeTimeLine not found \n");
    return nullptr;
}

void TimeManager::TimeSliceCut (TimeManager::TimeLine* t, TimeManager::TimeSlice* slice, vector<double> cutPoints){
    auto ntm = TimeManager::FindTimeLine(t, slice);
    if (ntm != nullptr){
    
    }
    else {
        printf("[ERROR] Trying to cut a slice from a NodeTimeLine that doesn't exist.\n");
    }
}

void TimeManager::TimeLineSplit (TimeManager::TimeLine* t, TimeManager::NodeTimeLine* nodeTimeLine, bool complete){

}

void TimeManager::AnimateTimeLine(TimeManager::TimeLine* t){
    // This won't be void in the end.
}
