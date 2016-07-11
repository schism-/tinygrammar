//
//  rule.hpp
//  tinygrammar
//
//  Created by tangles on 6/16/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef rule_h
#define rule_h

#include "common.h"
#include "operator.h"

using namespace std;

struct Rule {
    int rule_name;
    
    rule_tags matching_tags;
    rule_tags produced_tags;
    
    rule_params parameters;
    
    Operator op;
    
    Rule() {matching_tags = rule_tags(); produced_tags = rule_tags(); parameters = rule_params(); }
    Rule(rule_tags mt, rule_tags pt, rule_params pa) : matching_tags(mt), produced_tags(pt), parameters(pa) {};
    
    ~Rule() {};
};

#endif /* rule_hpp */
