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
#include "animator_matrix.h"

struct Animator{
    int animator_name;
    anim_params params;
    AnimatorKeyframes akf;
    
    Animator (){animator_name = anim_noop;}
    Animator (int animator_name) : animator_name(animator_name) {}
    Animator (int animator_name, anim_params p) : animator_name(animator_name), params(p) {}
    Animator (int animator_name, AnimatorKeyframes akf) : animator_name(animator_name), akf(akf) {}
    
    ~Animator(){}
    
    vector<AnimatedShape*> operator() (vector<AnimatedShape*> shape, double frame, double incr) {
        switch(animator_name){
            case anim_single:
            {
                if (frame < akf.offset) return shape;
                auto m = get_matrix(akf, frame);
                
                for (auto&& as : shape){
                    auto new_poly = transform(m, as->poly, incr);
                    as->poly = new_poly;
                }
                
                return shape;
                break;
            }
            case anim_group:
            {
                if (frame < akf.offset) return shape;
                auto m = get_matrix(akf, frame);
                for (auto&& as : shape){
                    auto new_poly = transform_group(m, as->poly, incr);
                    as->poly = new_poly;
                }
                return shape;
                break;
            }
            case anim_attribute:
            {
                if (frame < akf.offset) return shape;
                auto m = get_matrix(akf, frame);
                for (auto&& as : shape){
                    as = transform_attributes(m, as, frame);
                }
                return shape;
                break;
            }
            case anim_morph:
            {
                return vector<AnimatedShape*>();
                break;
            }
            case anim_noop:
            {
                return shape;
                break;
            }
            default:
            {
                printf("[Operator->op] ERROR: Shouldn't have gotten here. Invalid animator type\n");
                return shape;
                break;
            }
        }
    }
};

#endif /* animator_h */
