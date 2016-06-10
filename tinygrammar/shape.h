//
//  shape.hpp
//  tinygrammar
//
//  Created by tangles on 6/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef shape_h
#define shape_h

#include "common.h"

using namespace std;

struct Shape {
    ~Shape() {};
};


// |=====================================|
// |======== SHAPES for TANGLES =========|
// |=====================================|

struct ym_vec2r : vector<ym_vec<double, 2>> {
    using vector<ym_vec<double, 2>>::vector;
};

struct polyline2r : vector<ym_vec2r> {
    using vector<ym_vec2r>::vector;
};

struct polygon2r : vector<polyline2r> {
    using vector<polyline2r>::vector;
};

struct TangleShape : Shape {
    // grammar
    int                     tag = 0;
    int                     gid = 0;
    int                     tid = 0;
    
    // tangle shape
    ym_frame2f              frame = ym_identity_frame2f;
    polygon2r               poly;
    
    // guideline
    polyline2r              guideline;
    
    // drawing properties
    bool                    invert = false;
    vector<polyline2r>      viz_lines;
};

struct ScaffoldShape : Shape {

};

#endif /* shape_h */
