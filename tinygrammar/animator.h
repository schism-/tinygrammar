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
    
    double cubicInOut(double t){
        if  (t < 0.5) return 4*t*t*t;
        else return (t-1) * (2 * t - 2) * (2 * t - 2) + 1;
    }
    
    double easing(double incr, double alpha_s, double alpha_e, double current_time, double total_dur){
        auto new_incr = incr / ((alpha_e - alpha_s) * total_dur);
        auto local_alpha = (current_time - alpha_s * total_dur) / ((alpha_e - alpha_s) * total_dur);
        auto local_alpha_prev = ym_clamp(local_alpha - incr, 0.0f, 1.0f);
        auto local_alpha_ease = cubicInOut(local_alpha);
        auto local_alpha_prev_ease = cubicInOut(local_alpha_prev);
        new_incr = (local_alpha_ease - local_alpha_prev_ease) / ((alpha_e - alpha_s) * total_dur);
        printf(" n_i %f | l_a %.4f | i %.4f | %.4f / %.4f | [%.2f, %.2f]->%.2f \n",
               new_incr, local_alpha, incr, current_time, total_dur, alpha_s, alpha_e, alpha_e - alpha_s);
        return new_incr;
    }
    
    vector<AnimatedShape*> operator() (vector<AnimatedShape*> shape, double current_time, double incr, double total_dur) {
        switch(animator_name){
            case anim_single:
            {
                auto m = get_matrix(akf, current_time / total_dur);
                auto alpha_s = m.second[0];
                auto alpha_e = m.second[1];
                
                if (alpha_e - alpha_s <= EPS_2)
                    printf("houston, we have a problem! \n");
                
//                if (alpha_e != 1.0 && abs(current_time - alpha_e * total_dur) <= EPS_2){
//                    printf(" \n");
//                    return shape;
//                }
                
                auto new_incr = easing(incr, alpha_s, alpha_e, current_time, total_dur);
                for (auto&& as : shape) as->poly = transform(m.first, as->poly, new_incr);
                
                return shape;
                break;
            }
            case anim_group:
            {
                auto m = get_matrix(akf, current_time / total_dur);
                auto alpha_s = m.second[0];
                auto alpha_e = m.second[1];
                auto alpha_diff = alpha_e - alpha_s;
                
                if (alpha_e - alpha_s <= EPS_2) printf("houston, we have a problem! \n");
                
//                if (alpha_e != 1.0 && abs(current_time - alpha_e * total_dur) <= EPS_2){
//                    printf(" \n");
//                    return shape;
//                }
                
                auto new_incr = easing(incr, alpha_s, alpha_e, current_time, total_dur);
                for (auto&& as : shape) as->poly = transform_group(m.first, as->poly, new_incr);
                
                return shape;
                break;
            }
            case anim_attribute:
            {
                if (current_time / total_dur < akf.offset) return shape;
                auto m = get_matrix(akf, current_time / total_dur);
                auto alpha_s = m.second[0];
                auto alpha_e = m.second[1];
                auto alpha_diff = alpha_e - alpha_s;
                
                if (alpha_e - alpha_s <= EPS_2) printf("houston, we have a problem! \n");
                
                if (alpha_e != 1.0 && abs(current_time - alpha_e * total_dur) <= EPS_2){
                    printf(" \n");
                    return shape;
                }
                
                auto new_incr = incr / (alpha_diff * total_dur);
                auto local_alpha = (current_time - alpha_s * total_dur) / (alpha_diff * total_dur);
                printf(" n_i %f | l_a %.4f | i %.4f | %.4f / %.4f | [%.2f, %.2f]->%.2f \n",
                       new_incr, local_alpha, incr, current_time, total_dur, alpha_s, alpha_e, alpha_e - alpha_s);
                
                for (auto&& as : shape) as = transform_attributes(m.first, as, cubicInOut(local_alpha));

                return shape;
                break;
            }
            case anim_morph:
            {
                printf("anim_morph \n");
                return vector<AnimatedShape*>();
                break;
            }
            case anim_noop:
            {
                printf("noop \n");
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
