//
//  time_manager.h
//  tinygrammar
//
//  Created by Christian on 14/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef time_manager_h
#define time_manager_h

#include "common.h"
#include "csg_tree.h"
#include "animator_matrix.h"

namespace TimeManager {
    
    struct TimeSlice{
        string ts_tag;
        double duration;
        AnimatorKeyframes animation;
        
        TimeSlice (double d) : duration(d){}
        
        TimeSlice (double d, AnimatorKeyframes akf) : duration(d), animation(akf) {}
    };

    struct NodeTimeLine{
        double duration;
        vector<TimeSlice*> slices;
        CSGTree::Node* node;
        
        NodeTimeLine (double d) : duration(d), slices({new TimeSlice(d)}){}
        NodeTimeLine (double d, AnimatorKeyframes akf) : duration(d), slices({new TimeSlice(d, akf)}){}
        
        ~NodeTimeLine(){}
    };

    struct TimeLine{
        double duration;
        double current_time;
        
        vector<NodeTimeLine*> timelines;
        
        TimeLine (double d) : duration(d), timelines({new NodeTimeLine(d)}){ current_time = 0.0; }
        TimeLine (double d, AnimatorKeyframes akf) : duration(d), timelines({new NodeTimeLine(d, akf)}){
            current_time = 0.0;
        }
        
        ~TimeLine(){}
    };
    
    NodeTimeLine* FindTimeLine(TimeLine* t, TimeSlice* s);
    vector<TimeSlice*> GetAllSlices (TimeLine* t, NodeTimeLine* tl = nullptr);
    
    void TimeSliceCut  (TimeLine* t, TimeSlice* slice, const vector<double>& cutPoints = {0.5, 0.5}, const string& new_tag = "");
    void TimeLineSplit (TimeLine* t, NodeTimeLine* nodeTimeLine, const string& new_tag = "", bool complete = false);
    void TimeLineMerge (TimeLine* t, const vector<NodeTimeLine*>& nodeTimeLines);

    void AnimateTimeLine(TimeLine* t);

}


#endif /* time_manager_h */
