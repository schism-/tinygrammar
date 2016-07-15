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
//            for (auto exp : ((HistoryLinear*)h)->history){
//                if (not exp->terminal) res.push_back(exp);
//            }
            if (not ((HistoryLinear*)h)->history.empty()) res.push_back(((HistoryLinear*)h)->history.back());
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
            printf("[get_active_nodes] ERROR: Shouldn't have gotten here! Invalid history_type \n");
            break;
        }
    }
    return res;
}

Expansion* get_expansion(History* h, Shape* sel_shape){
    if (h->history_type == linear_history){
        for (auto exp : ((HistoryLinear*)h)->history){
            if (contains(exp->shapes, sel_shape)) return exp;
        }
    }
    else if (h->history_type == tree_history){
        // TODO : implementa metodo per tree
    }
    return nullptr;
}

void update_history(History* h, const PartitionShapeGroup& matched_shapes, Rule* matched_rule){
    switch(h->history_type){
        case linear_history:
        {
            if(matched_shapes.match.empty()){
                // We're dealing with an init
                auto exp = new Expansion(matched_shapes);
                ((HistoryLinear*)h)->history.push_back(exp);
            }
            else{
                auto exp = new Expansion(matched_shapes);
                ((HistoryLinear*)h)->history.push_back(exp);
            }
            break;
        }
        case tree_history:
        {
            // TODO : implementa metodo per tree
            break;
        }
        default:
        {
            printf("[update_history] ERROR: Shouldn't have gotten here! Invalid history_type in update_history \n");
            break;
        }
    }
}

ShapeGroup to_shapes(const vector<Expansion*>& active_nodes){
    auto res = ShapeGroup();
    for (auto an : active_nodes){
        for (auto s : an->shapes) res.push_back(s);
    }
    return res;
}

void expand_init(History* h) {
    auto grammar = get_grammar(grammar_filename);
    auto init_step = matching_init();
    auto init_shapes = init_step->op(ShapeGroup(), init_step->parameters, grammar->rn);
    auto init_partition = PartitionShapeGroup();
    init_partition.added = init_shapes;
    init_partition.remainder = ShapeGroup();
    init_partition.match = ShapeGroup();
    update_history(h, init_partition, init_step);
}

void expand(History* h) {
    //retrieve active nodes
    auto front = get_active_nodes(h);
    //pass them to grammar core
    auto grammar = get_grammar(grammar_filename);
    auto grammar_step = matching(to_shapes(front));
    
    if (grammar_step.second != nullptr){
        //if an appliable rule has been found, apply it and retrieve results
        grammar_step.first.added = grammar_step.second->op(grammar_step.first.match, grammar_step.second->parameters, grammar->rn);
        //update model
        update_history(h, grammar_step.first, grammar_step.second);
    }
    else{
        printf("No more expansions available\n");
    }
}
