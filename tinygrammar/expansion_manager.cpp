//
//  expansion_manager.cpp
//  tinygrammar
//
//  Created by Christian Santoni on 6/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include "expansion_manager.h"

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

void free_history(History* h){ delete h; }

vector<Expansion*> get_active_nodes(History* h){
    auto res = vector<Expansion*>();
    switch(h->history_type){
        case linear_history:
        {
            for (auto exp : ((HistoryLinear*)h)->history){
                if (not exp->terminal) res.push_back(exp);
            }
            break;
        }
        case tree_history:
        {
            tree<Expansion*>::leaf_iterator lit = ((HistoryTree*)h)->history->begin_leaf();
            while(lit != ((HistoryTree*)h)->history->end_leaf()) {
                if (not (*lit)->terminal) res.push_back((*lit));
                ++lit;
            }
            break;
        }
        default:
        {
            printf("Shouldn't have gotten here! Invalid history_type \n");
            break;
        }
    }
    return res;
}

void get_expansion(History* history, Shape* sel_shape){
    if (history->history_type == linear_history){
        
    }
    else if (history->history_type == tree_history){
        
    }
}

ShapeGroup* to_shapes(const vector<Expansion*>& active_nodes){
    auto res = new ShapeGroup();
    for (auto an : active_nodes){
        auto sg = new ShapeGroup();
        for (auto s : an->shapes) res->push_back(s);
    }
    return res;
}

void expand(History* h) {
    //retrieve active nodes
    auto front = get_active_nodes(h);
    //pass them to grammar core
    auto grammar_step = matching(to_shapes(front));
    //retrieve results
    
    //update model
}
