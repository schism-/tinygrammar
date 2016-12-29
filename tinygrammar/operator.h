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
ShapeGroup init_operator(rule_tags tags, rule_params parameters, int init_value, rng& sampler);

ShapeGroup time_init_operator(rule_tags tags, rule_params parameters, int init_value, rng& rn, CSGTree::Tree* tree = nullptr);
ShapeGroup time_slice_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline = nullptr);

ShapeGroup affine_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline = nullptr);

ShapeGroup rotation_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline = nullptr);

ShapeGroup move_towards_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline);

ShapeGroup attributes_operator(const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler, TimeManager::TimeLine* timeline = nullptr);

struct Operator{
    int operator_name;
    int init_value;
    
    Operator (){}
    Operator (int operator_name) : operator_name(operator_name) {}
    Operator (int operator_name, int init) : operator_name(operator_name), init_value(init) {}
    ~Operator(){}

    ShapeGroup operator() (const ShapeGroup& shapes, rule_tags tags, rule_params parameters, rng& sampler,
                           ShapeGroup* annotations = nullptr, TimeManager::TimeLine* timeline = nullptr, CSGTree::Tree* tree = nullptr) {
        switch(operator_name){
            case op_split:
                return tangle_split_operator(shapes, tags, parameters, sampler, annotations);
                break;
            case op_init:
                return init_operator(tags, parameters, init_value, sampler);
                break;
            case op_time_init:
                return time_init_operator(tags, parameters, init_value, sampler, tree);
                break;
            case op_time_slice:
                return time_slice_operator(shapes, tags, parameters, sampler, timeline);
                break;
            case op_affine:    
                return affine_operator(shapes, tags, parameters, sampler, timeline);
                break;
            case op_affine_rot:
            {
                // 0 : group or single | 1 : angle | 2 : offset
                auto new_param = rule_params();
                new_param[0] = parameters[0];
                auto angle = parameters[1] * ym_pi / 180.0;
                new_param[1] = cos(angle);      new_param[2] = -1.0 * sin(angle);
                new_param[3] = sin(angle);      new_param[4] = cos(angle);
                new_param[5] = 0.0;             new_param[6] = 0.0;
                new_param[7] = parameters[2];
                return affine_operator(shapes, tags, new_param, sampler, timeline);
                break;
            }
            case op_affine_tran:
            {
                // 0 : group or single | 1 : tran_x | 2 : tran_y | 3 : offset
                auto new_param = rule_params();
                new_param[0] = parameters[0];
                new_param[1] = 1.0;             new_param[2] = 0.0;
                new_param[3] = 0.0;             new_param[4] = 1.0;
                new_param[5] = parameters[1];   new_param[6] = parameters[2];
                new_param[7] = parameters[3];
                return affine_operator(shapes, tags, new_param, sampler, timeline);
                break;
            }
            case op_affine_scale:
            {
                // 0 : group or single | 1 : scale factor | 2 : offset
                auto new_param = rule_params();
                new_param[0] = parameters[0];
                new_param[1] = parameters[1];   new_param[2] = 0.0;
                new_param[3] = 0.0;             new_param[4] = parameters[1];
                new_param[5] = 0.0;             new_param[6] = 0.0;
                new_param[7] = parameters[2];
                return affine_operator(shapes, tags, new_param, sampler, timeline);
                break;
            }
            case op_affine_rot_tran:
            {
                // 0 : group or single | 1 : angle | 2 : tran_x | 3 : tran_y | 4 : offset
                auto new_param = rule_params();
                new_param[0] = parameters[0];
                auto angle = parameters[1] * ym_pi / 180.0;
                new_param[1] = cos(angle);      new_param[2] = -1.0 * sin(angle);
                new_param[3] = sin(angle);      new_param[4] = cos(angle);
                new_param[5] = parameters[2];   new_param[6] = parameters[3];
                new_param[7] = parameters[4];
                return affine_operator(shapes, tags, new_param, sampler, timeline);
                break;
            }
            case op_affine_rot_scale:
            {
                // 0 : group or single | 1 : angle | 2 : scale factor | 3 : offset
                auto new_param = rule_params();
                new_param[0] = parameters[0];
                auto angle = parameters[1] * ym_pi / 180.0;
                auto rot = ym_affine2r({{cos(angle), -1.0 * sin(angle)}, {sin(angle), cos(angle)}, {0.0, 0.0}});
                auto scale = ym_affine2r({{parameters[2], 0.0}, {0.0, parameters[2]}, {0.0, 0.0}});
                ym_affine2r rot_scale = rot * scale;
                
                new_param[1] = rot_scale.x.x;   new_param[2] = rot_scale.x.y;
                new_param[3] = rot_scale.y.x;   new_param[4] = rot_scale.y.y;
                new_param[5] = rot_scale.t.x;   new_param[6] = rot_scale.t.y;
                new_param[7] = parameters[3];
                
                return affine_operator(shapes, tags, new_param, sampler, timeline);
                break;
            }
            case op_affine_scale_tran:
            {
                // 0 : group or single | 1 : scale factor | 2 : tran_x | 3 : tran_y | 4 : offset
                auto new_param = rule_params();
                new_param[0] = parameters[0];
                new_param[1] = parameters[1];   new_param[2] = 0.0;
                new_param[3] = 0.0;             new_param[4] = parameters[1];
                new_param[5] = parameters[2];   new_param[6] = parameters[3];
                new_param[7] = parameters[4];
                return affine_operator(shapes, tags, new_param, sampler, timeline);
                break;
            }
            case op_move_towards:
            {
                // 0 : group or single | 1 : pos_x | 2 : pos_y | 3 : offset
                return move_towards_operator(shapes, tags, parameters, sampler, timeline);
                break;
            }
            case op_attribute:
                return attributes_operator(shapes, tags, parameters, sampler, timeline);
                break;
            default:
                printf("[Operator->op] ERROR: Shouldn't have gotten here. Invalid op type\n");
                break;
        }
        return ShapeGroup();
    }
};


#endif /* operator_h */
