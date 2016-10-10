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
    
    Animator (){}
    Animator (int animator_name) : animator_name(animator_name) {}
    Animator (int animator_name, anim_params p) : animator_name(animator_name), params(p) {}
    Animator (int animator_name, AnimatorKeyframes akf) : animator_name(animator_name), akf(akf) {}
    
    ~Animator(){}
    
    vector<AnimatedShape*> operator() (const vector<AnimatedShape*>& shape, double delta, const rng& sampler = rng(), ShapeGroup* annotations = nullptr) {
        switch(animator_name){
            case anim_eulerian:
            {
                for (auto&& s : shape){
                    auto poly = s->poly;
                    auto frame = ym_frame2r({params[0], params[1]}, {params[2], params[3]}, {params[4], params[5]});
                    auto frame_diff = ym_identity_frame2r;
                    auto boh = ym_frame2r((frame_diff.m * (1.0 - delta) + frame.m * delta), (frame_diff.t * (1.0 - delta) + frame.t * delta));
                    s->poly = transform_polygon(boh, poly);
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
    
    vector<AnimatedShape*> operator() (vector<AnimatedShape*> shape, int frame) {
        switch(animator_name){
            case anim_single:
            {
                auto m = get_matrix(akf, frame);
                for (auto&& as : shape){
                    auto new_poly = transform(m, as->poly);
                    as->poly = new_poly;
                }
                return shape;
                break;
            }
            case anim_group:
            {
                auto m = get_matrix(akf, frame);
                for (auto&& as : shape){
                    auto new_poly = transform_group(m, as->poly);
                    as->poly = new_poly;
                }
                return shape;
                break;
            }
            case anim_morph:
            {
                return vector<AnimatedShape*>();
                break;
            }
            default:
            {
                printf("[Operator->op] ERROR: Shouldn't have gotten here. Invalid animator type\n");
                return vector<AnimatedShape*>();
                break;
            }
        }
    }
};

#endif /* animator_h */
