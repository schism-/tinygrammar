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
    
    vector<int> matching_tags;
    vector<int> produced_tags;
    
    ym_vec<int, PARAM_SIZE> parameters;
    
    Operator op;
    
    Rule() {matching_tags = vector<int>(); produced_tags = vector<int>(); parameters = ym_vec<int, PARAM_SIZE>(); }
    Rule(vector<int> mt, vector<int> pt, ym_vec<int, PARAM_SIZE> pa) : matching_tags(mt), produced_tags(pt), parameters(pa) {};
    
    ~Rule() {};
};

#endif /* rule_hpp */
