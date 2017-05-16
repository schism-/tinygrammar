//
//  rule.hpp
//  tinygrammar
//
//  Created by tangles on 6/16/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef rule_h
#define rule_h

#include "operator.h"

using namespace std;

struct Rule {
    int rule_name;
    string rule_name_str;
    
    rule_tags matching_tags;
    rule_tags produced_tags;
    
    rule_params parameters;
    
    Operator op;
    
    Rule() {matching_tags = rule_tags(); produced_tags = rule_tags(); parameters = rule_params(); }
    Rule(const rule_tags& mt, const rule_tags& pt, const rule_params& pa) : matching_tags(mt), produced_tags(pt), parameters(pa) {};
    
    ~Rule() {};
};

#endif /* rule_hpp */
