//
//  animator_matrix.h
//  tinygrammar
//
//  Created by Christian on 03/10/16.
//  Copyright © 2016 visgraph. All rights reserved.
//

#ifndef animator_matrix_h
#define animator_matrix_h

#include "shape.h"
#include "tangle_utilities.h"

struct AnimatorMatrix{
    
    array<ym_affine2r, matrix_num> mats;
    array<ym_vec2r, matrix_num> mats_centers;
    ym_range2r bounding_box;
    ym_vec2r step;
    
    ym_vec4f start_b_color = {0.0, 0.0, 0.0, 1.0};
    ym_vec4f end_b_color   = {0.0, 0.0, 0.0, 1.0};
    
    ym_vec4f start_f_color = {255.0, 255.0, 255.0, 1.0};
    ym_vec4f end_f_color   = {255.0, 255.0, 255.0, 1.0};
    
    bool has_trail = false;
    
    AnimatorMatrix () {}
    
    AnimatorMatrix (const ym_range2r& bb) {
        bounding_box = bb;
        step = bounding_box.max - bounding_box.min;
        step.x = step.x / matrix_resolution; step.y = step.y / matrix_resolution;
        for (auto i = 0; i < matrix_num; i++){
            mats[i] = ym_affine2r();
            auto temp_r = ym_range2r({bounding_box.min.x + (i % (int)matrix_resolution) * step.x,
                                      bounding_box.min.y + (i / (int)matrix_resolution) * step.y},
                                     {bounding_box.min.x + (i % (int)matrix_resolution + 1) * step.x,
                                      bounding_box.min.y + (i / (int)matrix_resolution + 1) * step.y});
            mats_centers[i] = ym_rcenter(temp_r);
        }
    }
    
    AnimatorMatrix (const ym_range2r& bb, const ym_affine2r& mat) {
        bounding_box = bb;
        step = bounding_box.max - bounding_box.min;
        step.x = step.x / matrix_resolution; step.y = step.y / matrix_resolution;
        for (auto i = 0; i < matrix_num; i++){
            mats[i] = mat;
            auto temp_r = ym_range2r({bounding_box.min.x + (i % (int)matrix_resolution) * step.x,
                                      bounding_box.min.y + (i / (int)matrix_resolution) * step.y},
                                     {bounding_box.min.x + ((i + 1) % (int)matrix_resolution) * step.x,
                                      bounding_box.min.y + ((i + 1) / (int)matrix_resolution) * step.y});
            mats_centers[i] = ym_rcenter(temp_r);
        }
    }
    
    ~AnimatorMatrix(){}
};

AnimatorMatrix move_towards_point(const ym_range2r& bb, const ym_vec2r& pos, double scale = 1.0);

ym_affine2r get_matrix(const AnimatorMatrix& am, const ym_vec2r& pos);
void set_matrix(AnimatorMatrix am, const ym_affine2r& mat, int x_idx, int y_idx);
polygon2r transform(const AnimatorMatrix& am, const polygon2r& poly, double incr);
polygon2r transform_group(const AnimatorMatrix& am, const polygon2r& poly, double incr);
AnimatedShape* transform_attributes(const AnimatorMatrix& am, AnimatedShape* shape, double frame);

AnimatorMatrix copy(const AnimatorMatrix& am);

struct AnimatorKeyframes{
    
    vector<AnimatorMatrix> keyframes;
    vector<int> keyframes_idx;
    double offset;
    int anim_type;
    
    AnimatorKeyframes () {
        keyframes = vector<AnimatorMatrix>();
        keyframes_idx = vector<int>();
        offset = 0.0;
        anim_type = anim_noop;
    }
    
    AnimatorKeyframes (const vector<AnimatorMatrix>& kf, const vector<int>& kf_i) : keyframes(kf), keyframes_idx(kf_i) { offset = 0.0; }
    
    AnimatorKeyframes (const AnimatorMatrix kf, int kf_num, int a_t, double off = 0.0) {
        keyframes = vector<AnimatorMatrix>();
        keyframes_idx = vector<int>();
        for (auto i = 0 ; i < kf_num; i++) {
            keyframes.push_back(kf);
            keyframes_idx.push_back(i);
        }
        offset = off;
        anim_type = a_t;
    }
    
    // TODO: we now suppose there are either one AM per frame, or one AM for all frames.
    // Remember to implement interpolation
};

AnimatorMatrix get_matrix(const AnimatorKeyframes& akf, int keyframe);
AnimatorMatrix get_matrix(const AnimatorKeyframes& akf, double delta);
AnimatorKeyframes copy(const AnimatorKeyframes& akf);

#endif /* animator_matrix_h */
