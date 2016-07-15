//
//  main.cpp
//  tinygrammar
//
//  Created by tangles on 6/8/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#include <iostream>
#include "expansion_manager.h"

int main(int argc, const char * argv[]) {
    auto em = make_history(0);
    expand_init(em);
    expand(em);
    return 0;
}
