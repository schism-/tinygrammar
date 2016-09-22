//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include "tangle_utilities.h"
#include "expansion_manager.h"
#include "svg.h"

int main(int argc, const char * argv[]) {
    #if 0
    auto em = make_history(0);
    expand_init(em);
    while (expand(em)){
        printf("...");
    };
    save_svg(em, {600,600}, {300, 300}, {1.0, 1.0});
    return 0;
    #endif
    
    auto tree = CSGTree::InitTree();
    auto s1 = make_polyline_rect({-100, -100}, {100, 100});
    auto s2 = make_polyline_rect({0, 0}, {200, 200});
    
    auto n1 = CSGTree::AddShape(tree, new AnimatedShape({s1}));
    auto n2 = CSGTree::AddShape(tree, new AnimatedShape({s2}));
    CSGTree::Difference(tree, n1, n2);
    save_svg(tree, {600,600}, {300, 300}, {1.0, 1.0});
    
    printf("end main");
}
