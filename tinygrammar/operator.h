//
//  operator.hpp
//  tinygrammar
//
//  Created by tangles on 6/21/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef operator_h
#define operator_h

#include "common.h"
#include "tangle_utilities.h"

ShapeGroup tangle_split_operator(const ShapeGroup& shapes, rule_params parameters, rng& sampler, ShapeGroup* annotations = nullptr);
ShapeGroup init_operator(rule_params parameters, rng& sampler);

struct Operator{
    int operator_name;
    
    Operator (){}
    Operator (int operator_name) : operator_name(operator_name) {}
    ~Operator(){}

    ShapeGroup operator() (const ShapeGroup& shapes, rule_params parameters, rng& sampler, ShapeGroup* annotations = nullptr) {
        switch(operator_name){
            case op_split:
                return tangle_split_operator(shapes, parameters, sampler, annotations);
                break;
            case op_init:
                return init_operator(parameters, sampler);
                break;
            default:
                printf("[Operator->op] ERROR: Shouldn't have gotten here. Invalid op type\n");
                break;
        }
        return ShapeGroup();
    }
};


#endif /* operator_h */
