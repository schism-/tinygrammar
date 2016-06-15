//
//  expansion_manager.hpp
//  tinygrammar
//
//  Created by Christian Santoni on 6/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef expansion_manager_h
#define expansion_manager_h

#include "common.h"
#include "shape.h"

using namespace std;

enum {
    linear_history = 0,
    tree_history = 1
};


// |==============================|
// |======== HISTORY API =========|
// |==============================|

struct Expansion {
    Shape* shape;
    Shape* annotation;
    
    Expansion(Shape* s, Shape* a) : shape(s), annotation(a) {};
    
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
    }
};



History* make_history(int h_type);
void free_history(History* history);




// |=========================================|
// |======== HISTORY IMPLEMENTATION =========|
// |=========================================|

History* make_history(int h_type) {
    History* ret;
    switch (h_type){
        case linear_history:
            ret = new HistoryLinear();
        case tree_history:
            ret = new HistoryTree();
        default:
            ret = new HistoryLinear();
    }
    
    // TODO: initialize all data structures
    
    return ret;
}

void free_history(History* history){ delete history; }

void expand(History* history) {

}

void get_expansion(History* history, int exp_id){
    
}

void add_node(History* history) {}

#endif /* expansion_manager_h */
