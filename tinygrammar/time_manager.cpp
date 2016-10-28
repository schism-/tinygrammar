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

vector<TimeManager::TimeSlice*> TimeManager::GetAllSlices (TimeManager::TimeLine* t, TimeManager::NodeTimeLine* ntl){
    auto res = vector<TimeManager::TimeSlice*>();
    if (ntl == nullptr){
        for (auto&& ntls : t->timelines)
            for (auto&& s : ntls->slices)
                res.push_back(s);
    }
    else{
        for (auto&& s : ntl->slices)
            res.push_back(s);
    }
    return res;
}

void TimeManager::TimeSliceCut (TimeManager::TimeLine* t, TimeManager::TimeSlice* slice,
                                const vector<double>& cutPoints, const string& new_tag){
    auto ntm = TimeManager::FindTimeLine(t, slice);
    if (ntm != nullptr){
        auto new_slices = vector<TimeManager::TimeSlice*>();
        for (auto&& cp : cutPoints) {
            auto temp = new TimeManager::TimeSlice(slice->duration * cp);
            temp->animation = copy(slice->animation);
            if (new_tag != "") temp->ts_tag = new_tag;
            else temp->ts_tag = slice->ts_tag;
            new_slices.push_back(temp);
        }
        auto pos = find(ntm->slices.begin(), ntm->slices.end(), slice);
        ntm->slices.erase(pos);
        ntm->slices.insert(pos, new_slices.begin(), new_slices.end());
    }
    else {
        printf("[ERROR] TimeSliceCut : Trying to cut a slice from a NodeTimeLine that doesn't exist.\n");
    }
}

void TimeManager::TimeLineSplit (TimeManager::TimeLine* t, TimeManager::NodeTimeLine* nodeTimeLine,
                                 const string& new_tag, bool complete){
    // TODO: Evaluate if splitting nodes into singular shapes.
    // In that case, NodeTimeLine cannot use Node as object to apply tranformations on, but they have to act
    // directly on AnimatedShapes.
    
    auto pos = find(t->timelines.begin(), t->timelines.end(), nodeTimeLine);
    if (pos != t->timelines.end()){
        auto new_tm = new TimeManager::NodeTimeLine(nodeTimeLine->duration);
        new_tm->node = nodeTimeLine->node;
        new_tm->slices = vector<TimeManager::TimeSlice*>();
        for (auto&& s : nodeTimeLine->slices){
            auto new_s = new TimeManager::TimeSlice(s->duration);
            new_s->ts_tag = new_tag;
            new_tm->slices.push_back(new_s);
        }
        t->timelines.insert(pos, new_tm);
    }
    else{
        printf("[ERROR] TimeLineSplit : Trying to split a NodeTimeLine that doesn't exist.\n");
    }
}

void TimeManager::TimeLineMerge (TimeManager::TimeLine* t, const vector<TimeManager::NodeTimeLine*>& nodeTimeLines){
    
}

void TimeManager::AnimateTimeLine(TimeManager::TimeLine* t){
    // This won't be void in the end.
}
