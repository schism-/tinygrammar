//
//  expansion_manager.hpp
//  tinygrammar
//
//  Created by Christian Santoni on 6/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef expansion_manager_h
#define expansion_manager_h

#include "grammar_core.h"
#include "time_manager.h"

using namespace std;


// |==============================|
// |======== HISTORY API =========|
// |==============================|

struct BaseExpansion {
    Rule* applied_rule;
    bool terminal;
    
    ~BaseExpansion() {};
};

struct Expansion : BaseExpansion {
    ShapeGroup shapes;
    
    ShapeGroup match;
    ShapeGroup added;
    ShapeGroup remainder;
    
    Expansion() { terminal = false; };
    Expansion(const ShapeGroup& s) { shapes = s;  terminal = false; };
    Expansion(const PartitionShapeGroup& p) {
        match = p.match;
        added = p.added;
        remainder = p.remainder;
        for (auto s : remainder) shapes.push_back(s);
        for (auto s : added) shapes.push_back(s);
        terminal = false;
    };
};

struct ExpansionAnim : BaseExpansion {
    CSGTree::Tree* tree;
    TimeManager::TimeLine* timeline;
    
    //TODO : think about adding a partition for slices
    ExpansionAnim() { tree = new CSGTree::Tree(); timeline = new TimeManager::TimeLine(); terminal = false; };
    ExpansionAnim(Rule* a_r) { terminal = false; applied_rule = a_r; };
    
};

struct History {
    int history_type = -1;
    
    ~History() {};
};

struct HistoryTree : History{
    tree<Expansion*>* history;
    
    HistoryTree() {
        history = new tree<Expansion*>();
        history_type = tree_history;
    }
};

struct HistoryLinear : History{
    vector<Expansion*> history;
    
    HistoryLinear() {
        history_type = linear_history;
        history = vector<Expansion*>();
    }
};

struct HistoryAnim : History{
    vector<ExpansionAnim*> history;
    
    HistoryAnim() {
        history_type = animation_history;
        history = vector<ExpansionAnim*>();
    }
};



History* make_history(int h_type);

void free_history(History* history);

void expand_init(History* h);
bool expand(History* history);

void expand_init(HistoryAnim* h);
bool expand(HistoryAnim* history);

BaseExpansion* get_expansion(History* history, Shape* sel_shape);

void update_history(History* h, const PartitionShapeGroup& matched_shapes, Rule* matched_rule, CSGTree::Tree* tree = nullptr);

#endif /* expansion_manager_h */
