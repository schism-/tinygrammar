//
//  animator.h
//  tinygrammar
//
//  Created by Christian on 23/09/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef animator_h
#define animator_h

#include "shape.h"
#include "tangle_utilities.h"

struct Animator{
    int animator_name;
    anim_params params;
    Animator (){}
    Animator (int animator_name) : animator_name(animator_name) {}
    Animator (int animator_name, anim_params p) : animator_name(animator_name), params(p) {}
    
    ~Animator(){}
    
    vector<AnimatedShape*> operator() (const vector<AnimatedShape*>& shape, float delta, const rng& sampler = rng(), ShapeGroup* annotations = nullptr) {
        switch(animator_name){
            case anim_eulerian:
            {
                for (auto&& s : shape){
                    auto poly = s->poly;
                    auto frame = ym_frame2r({params[0], params[1]}, {params[2], params[3]}, {params[4], params[5]});
                    s->poly = transform_polygon(frame, poly);
                }
                return shape;
                break;
            }
            case anim_perturb:
            {
                return vector<AnimatedShape*>();
                break;
            }
            default:
            {
                printf("[Operator->op] ERROR: Shouldn't have gotten here. Invalid animator type\n");
                break;
            }
        }
        return vector<AnimatedShape*>();
    }
};

#endif /* animator_h */
