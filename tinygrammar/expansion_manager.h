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
#include "csg_tree.h"

using namespace std;


// |==============================|
// |======== HISTORY API =========|
// |==============================|

struct Expansion {
    ShapeGroup shapes;
    
    ShapeGroup match;
    ShapeGroup added;
    ShapeGroup remainder;
    
    Rule* applied_rule;
    bool terminal;
    
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
    
    
    ~Expansion() {};
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



History* make_history(int h_type);

void free_history(History* history);

void expand_init(History* h);
bool expand(History* history);

Expansion* get_expansion(History* history, Shape* sel_shape);

#endif /* expansion_manager_h */
