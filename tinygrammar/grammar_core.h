//
//  grammar_core.hpp
//  tinygrammar
//
//  Created by tangles on 6/15/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef grammar_core_h
#define grammar_core_h

#include "rule.h"
#include "time_manager.h"

struct Grammar {
    string                          name;
    vector<Rule*>                   rules;
    int                             seed = 0;
    rng                    rn;
    
    unordered_map<string, int> rule_mapping;
    unordered_map<string, int> tag_mapping;
    
    // Animation data
    double framerate;
    ym_range2r output_bbox;
    bool dry_run;
    
    Grammar() {rules = vector<Rule*>(); rule_mapping = unordered_map<string, int>(); tag_mapping = unordered_map<string, int>();}
    
    ~Grammar() {};
};

int add_rule_to_mapping(Grammar* grammar, const string& rulename);
rule_tags add_tags(Grammar* grammar, const vector<string>& tags);
inline int tag_to_mapping(Grammar* grammar, const string& tag);
inline string mapping_to_tag(Grammar* grammar, int tag);
bool is_tag_invert(Grammar* grammar, int tag);
int invert_tag(Grammar* grammar, int tag);

Grammar* get_grammar(const string& filename);
vector<Rule*> get_inits(Grammar* g);
vector<Rule*> get_rules(Grammar* g);
Rule* matching_init();

pair<PartitionShapeGroup, Rule*> matching(const ShapeGroup& active_shapes);
pair<PartitionShapeGroup, Rule*> matching_slice(Grammar* g, const ShapeGroup& active_shapes);
pair<PartitionShapeGroup, Rule*> matching_anim_shape(Grammar* g, const ShapeGroup& active_shapes, const unordered_map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});

inline bool tag_in_rule(int tag, const rule_tags& tags);
Rule* tangle_match_rule(Grammar* grammar, int tag, const vector<int>& temporal_tags = {});
Rule* tangle_match_rule(Grammar* grammar, int tag, const vector<AnimatedShape>& shapes);
Rule* tangle_match_rule(Grammar* grammar, int tag, const vector<TimeManager::TimeSlice>& slices);

PartitionShapeGroup matching_shapes(const ShapeGroup& active_nodes, bool anim_shape = false, const unordered_map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});
Rule* matching_rule(const ShapeGroup& matched, bool anim_shape = false, const unordered_map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});

PartitionShapeGroup matching_shapes_slice(const ShapeGroup& active_nodes);
Rule* matching_rule_slice(const ShapeGroup& matched);

inline bool tag_in_rule(int tag, const rule_tags& tags){
    for (auto i = 0; i < TAG_SIZE; i++){
        if (tag == tags[i]) return true;
    }
    return false;
}

inline int tag_to_mapping(Grammar* grammar, const string& tag) {
#if 0
    if (grammar->tag_mapping.count(tag) > 0) { return grammar->tag_mapping[tag]; }
    else {
        //printf("[TAG->MAPPING] [ERROR] tag not found : %s\n", tag.c_str());
        return -1;
    }
#else
    auto it = grammar->tag_mapping.find(tag);
    if(it == grammar->tag_mapping.end()) return -1;
    else return it->second;
#endif
}

inline int tag_to_mapping(Grammar* grammar) {
    static int ret = -1;
    if(ret < 0) {
        ret = tag_to_mapping(grammar, "");
    }
    return ret;
}

inline string mapping_to_tag(Grammar* grammar, int tag) {
    for( auto&& pair : grammar->tag_mapping ){
        if (pair.second == tag) return pair.first;
    }
    //    printf("[MAPPING->TAG] [ERROR] tag not found : %d\n", tag);
    return {};
}


#endif /* grammar_core_h */
