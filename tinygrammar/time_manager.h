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
        CSGTree::Node* node;
        AnimatorKeyframes animation;
        
        TimeSlice (double d) : duration(d){}
    };

    struct NodeTimeLine{
        double duration;
        vector<TimeSlice*> slices;
        
        NodeTimeLine (double d) : duration(d), slices({new TimeSlice(d)}){}
        
        ~NodeTimeLine(){}
    };

    struct TimeLine{
        double duration;
        double current_time;
        
        vector<NodeTimeLine*> timelines;
        
        TimeLine (double d) : duration(d), timelines({new NodeTimeLine(d)}){ current_time = 0.0; }
        
        ~TimeLine(){}
    };
    
    NodeTimeLine* FindTimeLine(TimeLine* t, TimeSlice* s);
    
    void TimeSliceCut (TimeLine* t, TimeSlice* slice, vector<double> cutPoints = {0.5});
    
    void TimeLineSplit (TimeLine* t, NodeTimeLine* nodeTimeLine, bool complete = false);

    void AnimateTimeLine(TimeLine* t);

}


#endif /* time_manager_h */
