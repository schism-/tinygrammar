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
    
    map<string, int> rule_mapping;
    map<string, int> tag_mapping;
    
    Grammar() {rules = vector<Rule*>(); rule_mapping = map<string, int>(); tag_mapping = map<string, int>();}
    
    ~Grammar() {};
};

int add_rule_to_mapping(Grammar* grammar, string rulename);
rule_tags add_tags(Grammar* grammar, vector<string> tags);
int tag_to_mapping(Grammar* grammar, string tag);
string mapping_to_tag(Grammar* grammar, int tag);
bool is_tag_invert(Grammar* grammar, int tag);
int invert_tag(Grammar* grammar, int tag);

Grammar* get_grammar(string filename);
vector<Rule*> get_inits(Grammar* g);
vector<Rule*> get_rules(Grammar* g);
Rule* matching_init();

pair<PartitionShapeGroup, Rule*> matching(const ShapeGroup& active_shapes);
pair<PartitionShapeGroup, Rule*> matching_slice(Grammar* g, const ShapeGroup& active_shapes, const map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});
pair<PartitionShapeGroup, Rule*> matching_anim_shape(Grammar* g, const ShapeGroup& active_shapes, const map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});

bool tag_in_rule(int tag, const rule_tags& tags);
Rule* tangle_match_rule(Grammar* grammar, int tag, const vector<int>& temporal_tags = {});

PartitionShapeGroup matching_shapes(const ShapeGroup& active_nodes, bool anim_shape = false, const map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});
Rule* matching_rule(const ShapeGroup& matched, bool anim_shape = false, const map<Shape*, TimeManager::NodeTimeLine*>& shape_map = {});

#endif /* grammar_core_h */
