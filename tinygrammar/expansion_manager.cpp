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
        {
            ret = new HistoryLinear();
            break;
        }
        case tree_history:
        {
            ret = new HistoryTree();
            break;
        }
        case animation_history:
        {
            ret = new HistoryAnim();
            break;
        }
        default:
            ret = new HistoryLinear();
    }
    
    // TODO: initialize all data structures
    
    return ret;
}

void free_history(History* h){ delete h; }

vector<BaseExpansion*> get_active_nodes(History* h){
    auto res = vector<BaseExpansion*>();
    switch(h->history_type){
        case linear_history:
        {
            if (not ((HistoryLinear*)h)->history.empty())
                res.push_back(((HistoryLinear*)h)->history.back());
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
        case animation_history:
        {
            if (not ((HistoryAnim*)h)->history.empty())
                res.push_back(((HistoryAnim*)h)->history.back());
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

BaseExpansion* get_expansion(History* h, Shape* sel_shape){
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

void update_history(History* h, const PartitionShapeGroup& matched_shapes, Rule* matched_rule, CSGTree::Tree* tree){
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
        case animation_history:
        {
            if(matched_shapes.match.empty()){
                // We're dealing with an init
                auto exp = new ExpansionAnim();
                auto k = 0;
                
                exp->timeline->timelines.push_back(new TimeManager::NodeTimeLine(matched_rule->parameters[0]));
                exp->timeline->timelines.back()->node = tree->leaves[k];
                k++;
                
                exp->timeline->duration = matched_rule->parameters[0];
                exp->applied_rule = matched_rule;
                
                for (auto&& s : matched_shapes.added){
                    if (s == nullptr){
                        exp->timeline->timelines.push_back(new TimeManager::NodeTimeLine(matched_rule->parameters[0]));
                        exp->timeline->timelines.back()->node = tree->leaves[k];
                        k++;
                        continue;
                    }
                    auto temp = ((TimeSliceShape*)s);
                    exp->timeline->timelines.back()->slices.push_back(temp->slice);
                }
                exp->timeline->timelines.pop_back();
                ((HistoryAnim*)h)->history.push_back(exp);
            }
            else{
                auto exp = new ExpansionAnim();
                auto old_exp = ((HistoryAnim*)h)->history.back();
                
                //TODO: THIS IS WRONG. OPERATORS SHOULDN'T OPERATE WITH TIMELINES, ONLY WITH SLICES
                //      exp_manager should deal with all this stuff. CORRECT THIS!
                
                exp->tree = old_exp->tree;
                exp->timeline = old_exp->timeline;
                exp->terminal = false;
                
                exp->applied_rule = matched_rule;
                
                ((HistoryAnim*)h)->history.push_back(exp);
            }
            break;
        }
        default:
        {
            printf("[update_history] ERROR: Shouldn't have gotten here! Invalid history_type in update_history \n");
            break;
        }
    }
}

ShapeGroup to_shapes(const vector<BaseExpansion*>& active_nodes){
    auto res = ShapeGroup();
    for (auto an : active_nodes){
        for (auto s : ((Expansion*)an)->shapes) res.push_back(s);
    }
    return res;
}

ShapeGroup to_slices(const vector<BaseExpansion*>& active_nodes){
    auto res = ShapeGroup();
    for (auto an : active_nodes){
        for (auto&& ntl : ((ExpansionAnim*)an)->timeline->timelines)
            for (auto&& sl : ntl->slices)
                res.push_back(new TimeSliceShape(sl));
    }
    return res;
}

ShapeGroup to_animated_shapes(const vector<BaseExpansion*>& active_nodes){
    auto res = ShapeGroup();
    for (auto an : active_nodes){
        auto temp = ((ExpansionAnim*)an);
        for (auto&& l : temp->tree->leaves)
            for (auto&& s : l->shapes)
                res.push_back(s);
    }
    return res;
}


void expand_init(History* h) {
    auto grammar = get_grammar(grammar_filename);
    auto init_step = matching_init();
    auto init_shapes = init_step->op.apply(ShapeGroup(), init_step->produced_tags, init_step->parameters, grammar->rn);
    auto init_partition = PartitionShapeGroup();
    init_partition.added = init_shapes;
    init_partition.remainder = ShapeGroup();
    init_partition.match = ShapeGroup();
    update_history(h, init_partition, init_step);
}

bool expand(History* h) {
    //retrieve active nodes
    auto front = get_active_nodes(h);
    //pass them to grammar core
    auto grammar = get_grammar(grammar_filename);
    auto grammar_step = matching(to_shapes(front));
    
    if (grammar_step.second != nullptr){
        //if an appliable rule has been found, apply it and retrieve results
//        printf("Rule applied : %s \n", grammar_step.second->rule_name_str.c_str());
        grammar_step.first.added = grammar_step.second->op.apply(grammar_step.first.match, grammar_step.second->produced_tags, grammar_step.second->parameters, grammar->rn);
        //update model
        update_history(h, grammar_step.first, grammar_step.second);
        return true;
    }
    else{
        printf("No more expansions available\n");
        return false;
    }
}

void expand_init(HistoryAnim* h) {
    
}

bool expand(HistoryAnim* h) {
    //retrieve active nodes
    
    auto front = get_active_nodes(h);
    //pass them to grammar core
    auto grammar = get_grammar(grammar_filename);
    
    auto active_slices = to_slices(front);
    // Mapping the shapes to their respective slices
    auto shapes_map = map<Shape*, TimeManager::NodeTimeLine*>();
    
    //EDOARDO FIX
    static map<TimeManager::TimeSlice*, TimeManager::NodeTimeLine*> sliceToTimeline;
    static map<AnimatedShape*, CSGTree::LeafNode*> shapeToNode;

    for (auto && as : active_slices){
        if (sliceToTimeline[((TimeSliceShape*)as)->slice] != nullptr)
            shapes_map[as] = sliceToTimeline[((TimeSliceShape*)as)->slice];
        else {
             shapes_map[as] = TimeManager::FindTimeLine(h->history.back()->timeline, ((TimeSliceShape*)as)->slice);
             sliceToTimeline[((TimeSliceShape*)as)->slice] = shapes_map[as];
        }
    }
    
    auto grammar_step = matching_slice(grammar, active_slices, shapes_map);
    
    if (grammar_step.second != nullptr){
        //if an appliable rule has been found, apply it and retrieve results
//        printf("[TIME] Rule applied : %s \n", grammar_step.second->rule_name_str.c_str());
        grammar_step.first.added = grammar_step.second->op.apply(grammar_step.first.match, grammar_step.second->produced_tags, grammar_step.second->parameters,
                                                           grammar->rn, nullptr, h->history.back()->timeline);
        //update model
        update_history(h, grammar_step.first, grammar_step.second);
        return true;
    }
    else{
//        printf("[TIME]  No more expansions available\n");
//        printf("[SPACE] Finding expansions \n");
        
        // Now we can start doing the spatial expansions
        auto anim_shapes = to_animated_shapes(front);
        
        // Mapping the shapes to their respective slices
        auto shapes_map = map<Shape*, TimeManager::NodeTimeLine*>();
        for (auto && as : anim_shapes){
            auto node = shapeToNode[(AnimatedShape*)as];
            if (node == nullptr){
                node = CSGTree::FindNode(h->history.back()->tree, (AnimatedShape*)as);
                shapeToNode[(AnimatedShape*)as] = node;
            }
            shapes_map[(AnimatedShape*)as] = TimeManager::FindTimeLine(h->history.back()->timeline, node);
        }
        grammar_step = matching_anim_shape(grammar, anim_shapes, shapes_map);
        
        if (grammar_step.second != nullptr){
            //if an appliable rule has been found, apply it and retrieve results
//            printf("[SPACE] Rule applied : %s on %d shapes\n", grammar_step.second->rule_name_str.c_str(), (int)grammar_step.first.match.size());
            
            // Got the shapes, now retrieve the Nodes in the Tree
            auto ntls = vector<TimeManager::NodeTimeLine*>();
            for (auto&& s : grammar_step.first.match){
                ntls.push_back(shapes_map[(AnimatedShape*)s]);
            }
            // Choose slices where to apply the animation
            //      The Rule should tell me that
            auto shapes = ShapeGroup();
            for (auto&& ntl : ntls){
                for (auto&& s : ntl->slices)
                    if (grammar_step.second->op.init_value == s->ts_tag || grammar_step.second->op.init_value == invert_tag(grammar, s->ts_tag))
                        shapes.push_back(new TimeSliceShape(s));
            }

            // Apply the animation to the selected slices
            grammar_step.first.added = grammar_step.second->op.apply(shapes, grammar_step.second->produced_tags, grammar_step.second->parameters,
                                                               grammar->rn, nullptr, h->history.back()->timeline);
            //update model
            update_history(h, grammar_step.first, grammar_step.second);
            return true;
        }
        else{
//            printf("[SPACE]  No more expansions available\n");
            return false;
        }
    }

    return false;
}

