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

using namespace std;

struct Rule {
    vector<int> matching_tags;
    vector<int> produced_tags;
};

#endif /* rule_hpp */
