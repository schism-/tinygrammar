//
//  grammar_core.hpp
//  tinygrammar
//
//  Created by tangles on 6/15/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef grammar_core_h
#define grammar_core_h

#include "common.h"
#include "shape.h"
#include "rule.h"

//TODO: - matching shapes
//      - matching rule
//      - loading grammar

enum {
    tangle_grammar = 0,
    learning_grammar,
    animation_grammar
};

struct Grammar {
    string                          name;
    vector<Rule*>                   rules;
    int                             seed = 0;
    ym_rng_pcg32                    rn;
    
    map<string, int> rule_mapping;
    map<string, int> tag_mapping;
    
    Grammar() {rules = vector<Rule*>(); rule_mapping = map<string, int>(); tag_mapping = map<string, int>();}
    
    ~Grammar() {};
};

Grammar* get_grammar(string filename);
vector<Rule*> get_rules(Grammar* g);

int add_rule_to_mapping(Grammar* grammar, string rulename);
vector<int> add_tags(Grammar* grammar, vector<string> tags);

ShapeGroup* matching(ShapeGroup* active_shapes);
ShapeGroup* matching_shapes(ShapeGroup* active_nodes);
Rule* matching_rule(ShapeGroup* matched);

#endif /* grammar_core_h */
