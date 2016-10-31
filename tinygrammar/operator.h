//
//  operator.hpp
//  tinygrammar
//
//  Created by tangles on 6/21/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef operator_h
#define operator_h

#include "shape.h"
#include "time_manager.h"

ShapeGroup tangle_split_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, ShapeGroup* annotations = nullptr);
ShapeGroup init_operator(rule_tags tags, rule_params parameters, string init_value, rng& sampler);

ShapeGroup time_init_operator(rule_tags tags, rule_params parameters, string init_value, rng& rn);

struct Operator{
    int operator_name;
    string init_value;
    
    Operator (){}
    Operator (int operator_name) : operator_name(operator_name) {}
    Operator (int operator_name, string init) : operator_name(operator_name), init_value(init) {}
    ~Operator(){}

    ShapeGroup operator() (const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, ShapeGroup* annotations = nullptr) {
        switch(operator_name){
            case op_split:
                return tangle_split_operator(shapes, tags, parameters, sampler, annotations);
                break;
            case op_init:
                return init_operator(tags, parameters, init_value, sampler);
                break;
            case op_time_init:
                return time_init_operator(tags, parameters, init_value, sampler);
                break;
            default:
                printf("[Operator->op] ERROR: Shouldn't have gotten here. Invalid op type\n");
                break;
        }
        return ShapeGroup();
    }
};


#endif /* operator_h */
