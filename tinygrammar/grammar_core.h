//
//  grammar_core.hpp
//  tinygrammar
//
//  Created by tangles on 6/15/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef grammar_core_h
#define grammar_core_h

#include "common.h"
#include "shape.h"

//TODO: - matching shapes
//      - matching rule
//      - loading grammar

AnnotatedShapes* matching(const vector<AnnotatedShapes*>& active_nodes);

#endif /* grammar_core_h */
