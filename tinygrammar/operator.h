//
//  operator.hpp
//  tinygrammar
//
//  Created by tangles on 6/21/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef operator_h
#define operator_h

#include <stdio.h>
#include "common.h"
#include "shape.h"

struct Operator{
    public:
        Operator (){}
        Operator (string operator_name) : operator_name(operator_name) {}
        ~Operator(){}
    
        ShapeGroup* operator() (ShapeGroup* shapes, ym_vec<int, PARAM_SIZE> parameters) { return new ShapeGroup(); }
    private:
        string operator_name;
};

#endif /* operator_h */
