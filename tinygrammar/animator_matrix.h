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
    vector<ym_affine2r> mats_;
    vector<ym_vec2r> mats_centers_;
    ym_affine2r cmat;
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
        mats_centers_.resize(matrix_num);
        mats_.resize(matrix_num);
        for (auto i = 0; i < matrix_num; i++){
            mats_[i] = ym_affine2r();
            auto temp_r = ym_range2r({bounding_box.min.x + (i % (int)matrix_resolution) * step.x,
                                      bounding_box.min.y + (i / (int)matrix_resolution) * step.y},
                                     {bounding_box.min.x + (i % (int)matrix_resolution + 1) * step.x,
                                      bounding_box.min.y + (i / (int)matrix_resolution + 1) * step.y});
            mats_centers_[i] = ym_rcenter(temp_r);
        }
    }
    
    AnimatorMatrix (const ym_range2r& bb, const ym_affine2r& mat) {
        bounding_box = bb;
        step = bounding_box.max - bounding_box.min;
        step.x = step.x / matrix_resolution; step.y = step.y / matrix_resolution;
        cmat = mat;
    }
    
    ~AnimatorMatrix(){}
};

AnimatorMatrix move_towards_point(const ym_range2r& bb, const ym_vec2r& pos, double scale = 1.0);
AnimatorMatrix morph_to_circle(const ym_range2r& bb, const ym_vec2r& pos, double scale = 1.0);

ym_affine2r get_matrix_nearest(const AnimatorMatrix& am, const ym_vec2r& pos);
void set_matrix(AnimatorMatrix am, const ym_affine2r& mat, int x_idx, int y_idx);
polygon2r transform(const AnimatorMatrix& am, const polygon2r& poly, double incr);
polygon2r transform_group(const AnimatorMatrix& am, const polygon2r& poly, double incr);
void transform_attributes(const AnimatorMatrix& am, AnimatedShape* shape, double frame);

AnimatorMatrix copy(const AnimatorMatrix& am);

struct AnimatorKeyframes{
    
    vector<AnimatorMatrix> keyframes;
    vector<double> keyframes_idx;
    double offset;
    int anim_type;
    
    AnimatorKeyframes () {
        keyframes = vector<AnimatorMatrix>();
        keyframes_idx = vector<double>();
        offset = 0.0;
        anim_type = anim_noop;
    }
    
    AnimatorKeyframes (const vector<AnimatorMatrix>& kf, const vector<double>& kf_i) : keyframes(kf), keyframes_idx(kf_i) { offset = 0.0; }
    
    AnimatorKeyframes (const AnimatorMatrix& kf, const vector<double>& kf_num, int a_t, double off = 0.0) {
        keyframes = vector<AnimatorMatrix>(kf_num.size(), kf);
        keyframes_idx = kf_num;
        offset = off;
        anim_type = a_t;
    }
    
    AnimatorKeyframes (const vector<AnimatorMatrix>& kf, const vector<double>& kf_num, int a_t, double off = 0.0) {
        if ((int)(kf.size()) != (int)(kf_num.size())){
            printf("[ERROR] AKF CONSTRUCTOR : kf length and kf_num doesn't match ");
            return;
        }
        keyframes = kf;
        keyframes_idx = kf_num;
        offset = off;
        anim_type = a_t;
    }
    
    // TODO: we now suppose there are either one AM per frame, or one AM for all frames.
    // Remember to implement interpolation
};

const AnimatorMatrix& get_matrix_nearest(const AnimatorKeyframes& akf, int keyframe);
pair<AnimatorMatrix, ym_vec2r> get_matrix(const AnimatorKeyframes& akf, double delta);
AnimatorKeyframes copy(const AnimatorKeyframes& akf);

#endif /* animator_matrix_h */
