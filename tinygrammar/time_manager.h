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
#include "threadpool.h"


namespace TimeManager {

    static double anim_current_time = 0.0;
    
    struct NodeTimeLine;
    
    struct TimeSlice{
        int ts_tag = -1;
        double duration;
        AnimatorKeyframes animation;
        
        NodeTimeLine* __expand__to_timeline__ = nullptr;
        
        TimeSlice (double d) : duration(d){}
        TimeSlice (double d, int tag) : duration(d), ts_tag(tag) {animation = AnimatorKeyframes();}
        TimeSlice (double d, const AnimatorKeyframes& akf) : duration(d), animation(akf) {}
    };

    struct NodeTimeLine{
        double duration;
        vector<TimeSlice*> slices;
        CSGTree::Node* node;
        
        NodeTimeLine (double d) : duration(d) {slices = vector<TimeSlice*>();}
        NodeTimeLine (double d, TimeSlice* ts) : duration(d), slices({ts}){}
        NodeTimeLine (double d, const AnimatorKeyframes& akf) : duration(d), slices({new TimeSlice(d, akf)}){}
        
        ~NodeTimeLine(){}
    };

    struct TimeLine{
        double duration;
        double current_time;
        
        vector<NodeTimeLine*> timelines;
        
        TimeLine () { timelines = vector<NodeTimeLine*>(); current_time = 0.0; }
        TimeLine (double d) : duration(d), timelines({new NodeTimeLine(d)}){ current_time = 0.0; }
        TimeLine (double d, const AnimatorKeyframes& akf) : duration(d), timelines({new NodeTimeLine(d, akf)}){
            current_time = 0.0;
        }
        
        ~TimeLine(){}
    };
    
    NodeTimeLine* FindTimeLine(TimeLine* t, TimeSlice* s);
    NodeTimeLine* FindTimeLine (TimeLine* t, CSGTree::Node* n);
    vector<TimeSlice*> GetAllSlices (TimeLine* t, NodeTimeLine* tl = nullptr);
    
    vector<TimeManager::TimeSlice*> TimeSliceCut  (TimeLine* t, TimeSlice* slice, rule_params cutPoints, rule_tags new_tags);
    
    void printTimeLine(TimeLine* t);

    void TimeLineSplit (TimeLine* t, NodeTimeLine* nodeTimeLine, int new_tag = -1, bool complete = false);

    //void TimeLineMerge (TimeLine* t, const vector<NodeTimeLine*>& nodeTimeLines);
    
    void AnimateTimeLine(TimeLine* t, CSGTree::Tree* tree, double current_time, double incr);
    void AnimateNodeTimeLine(NodeTimeLine* ntl, CSGTree::Tree* tree, double current_time, double incr);
}

struct TimeSliceShape : Shape {
    TimeManager::TimeSlice* slice = nullptr;
    
    TimeSliceShape () {shape_type = time_shape;};
    TimeSliceShape (TimeManager::TimeSlice* s) {shape_type = time_shape; slice = s;};
};

#endif /* time_manager_h */
